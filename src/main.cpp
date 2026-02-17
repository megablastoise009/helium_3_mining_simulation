// CLI entry point and CSV output.
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "sim/constants.h"
#include "sim/simulation.h"

namespace {

struct CliOptions {
    int trucks = 5;
    int stations = 2;
    double hours = sim::kSimulationHoursDefault;
    bool has_seed = false;
    uint32_t seed = 0;
    std::string csv_path = "results.csv";
    bool show_help = false;
};

// Print CLI usage text.
void PrintHelp() {
    std::cout << "Usage: vast_sim_cli [options]\n"
              << "Options:\n"
              << "  --trucks <n>     Number of mining trucks (default 5)\n"
              << "  --stations <n>   Number of unload stations (default 2)\n"
              << "  --hours <h>      Simulation hours (default 72)\n"
              << "  --seed <n>       RNG seed (optional)\n"
              << "  --csv <path>     Output CSV path (default results.csv)\n"
              << "  --help           Show this help message\n";
}

// Parse CLI arguments into options or return an error message.
bool ParseArgs(int argc, char** argv, CliOptions& options, std::string& error) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            options.show_help = true;
            return true;
        }

        auto require_value = [&](const char* name) -> bool {
            if (i + 1 >= argc) {
                error = std::string("Missing value for ") + name;
                return false;
            }
            return true;
        };

        try {
            if (arg == "--trucks") {
                if (!require_value("--trucks")) return false;
                options.trucks = std::stoi(argv[++i]);
            } else if (arg == "--stations") {
                if (!require_value("--stations")) return false;
                options.stations = std::stoi(argv[++i]);
            } else if (arg == "--hours") {
                if (!require_value("--hours")) return false;
                options.hours = std::stod(argv[++i]);
            } else if (arg == "--seed") {
                if (!require_value("--seed")) return false;
                options.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
                options.has_seed = true;
            } else if (arg == "--csv") {
                if (!require_value("--csv")) return false;
                options.csv_path = argv[++i];
            } else {
                error = std::string("Unknown argument: ") + arg;
                return false;
            }
        } catch (const std::exception&) {
            error = std::string("Invalid value for argument: ") + arg;
            return false;
        }
    }

    return true;
}

// Write per-truck and per-station metrics to a CSV file.
bool WriteCsv(const std::string& path, const sim::SimulationResult& result) {
    std::ofstream out(path);
    if (!out) {
        std::cerr << "Failed to open CSV path: " << path << "\n";
        return false;
    }

    out << "entity_type,id,loads_started,mining_minutes,travel_minutes,wait_minutes,unload_minutes,efficiency,"
           "station_busy_minutes,station_utilization,avg_wait_minutes,trucks_started\n";

    out << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < result.truck_stats.size(); ++i) {
        const auto& stats = result.truck_stats[i];
        out << "truck," << i << ','
            << stats.loads_started << ','
            << stats.mining_minutes << ','
            << stats.travel_minutes << ','
            << stats.wait_minutes << ','
            << stats.unload_minutes << ','
            << stats.efficiency() << ",,,,\n";
    }

    for (std::size_t i = 0; i < result.station_stats.size(); ++i) {
        const auto& stats = result.station_stats[i];
        out << "station," << i << ",,,,,,"
            << stats.busy_minutes << ','
            << stats.utilization(result.simulated_minutes) << ','
            << stats.average_wait() << ','
            << stats.trucks_started << '\n';
    }

    return true;
}

// Print a readable summary to stdout.
void PrintSummary(const sim::SimulationResult& result) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Simulation Summary\n";
    std::cout << "Trucks: " << result.config.truck_count
              << ", Stations: " << result.config.station_count
              << ", Duration (hours): " << (result.simulated_minutes / sim::kMinutesPerHour)
              << ", Seed: " << (result.config.seed.has_value() ? std::to_string(*result.config.seed) : "n/a")
              << '\n';
    std::cout << "Total unloads started: " << result.total_loads_started << "\n\n";

    std::cout << "Per-truck stats:\n";
    for (std::size_t i = 0; i < result.truck_stats.size(); ++i) {
        const auto& stats = result.truck_stats[i];
        std::cout << "Truck " << i
                  << " | loads: " << stats.loads_started
                  << " | mining: " << stats.mining_minutes
                  << " | travel: " << stats.travel_minutes
                  << " | wait: " << stats.wait_minutes
                  << " | unload: " << stats.unload_minutes
                  << " | efficiency: " << stats.efficiency()
                  << '\n';
    }

    std::cout << "\nPer-station stats:\n";
    for (std::size_t i = 0; i < result.station_stats.size(); ++i) {
        const auto& stats = result.station_stats[i];
        std::cout << "Station " << i
                  << " | trucks started: " << stats.trucks_started
                  << " | busy: " << stats.busy_minutes
                  << " | utilization: " << stats.utilization(result.simulated_minutes)
                  << " | avg wait: " << stats.average_wait()
                  << '\n';
    }
}

}  // namespace

// Entry point for the CLI simulation.
int main(int argc, char** argv) {
    CliOptions options;
    std::string error;
    if (!ParseArgs(argc, argv, options, error)) {
        std::cerr << error << '\n';
        PrintHelp();
        return 1;
    }

    if (options.show_help) {
        PrintHelp();
        return 0;
    }

    if (options.trucks < 0 || options.stations <= 0 || options.hours <= 0.0) {
        std::cerr << "Trucks must be >= 0; stations and hours must be positive values.\n";
        return 1;
    }

    sim::SimulationConfig config;
    config.truck_count = options.trucks;
    config.station_count = options.stations;
    config.simulation_minutes = options.hours * sim::kMinutesPerHour;
    if (options.has_seed) {
        config.seed = options.seed;
    }

    sim::Simulation simulation(config);
    const sim::SimulationResult result = simulation.Run();

    PrintSummary(result);

    if (!WriteCsv(options.csv_path, result)) {
        return 1;
    }

    std::cout << "\nCSV written to: " << options.csv_path << '\n';
    return 0;
}
