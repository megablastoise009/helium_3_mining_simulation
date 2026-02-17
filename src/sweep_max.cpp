// Sweep the simulation to find trucks needed for max loads per station count.
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "sim/constants.h"
#include "sim/simulation.h"

namespace {

struct SweepOptions {
    int stations_max = 10;
    int trucks_multiplier = 200;
    int min_trucks = 0;
    sim::Minutes simulation_minutes = sim::kSimulationMinutesDefault;
    uint32_t seed_base = 1000u;
    std::string results_path = "sweep_max_results.csv";
    std::string summary_path = "sweep_max_summary.csv";
};

// Parse CLI arguments into sweep options.
bool ParseArgs(int argc, char** argv, SweepOptions& options, std::string& error) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        auto require_value = [&](const char* name) -> bool {
            if (i + 1 >= argc) {
                error = std::string("Missing value for ") + name;
                return false;
            }
            return true;
        };

        try {
            if (arg == "--stations-max") {
                if (!require_value("--stations-max")) return false;
                options.stations_max = std::stoi(argv[++i]);
            } else if (arg == "--trucks-multiplier") {
                if (!require_value("--trucks-multiplier")) return false;
                options.trucks_multiplier = std::stoi(argv[++i]);
            } else if (arg == "--min-trucks") {
                if (!require_value("--min-trucks")) return false;
                options.min_trucks = std::stoi(argv[++i]);
            } else if (arg == "--hours") {
                if (!require_value("--hours")) return false;
                options.simulation_minutes = std::stod(argv[++i]) * sim::kMinutesPerHour;
            } else if (arg == "--seed-base") {
                if (!require_value("--seed-base")) return false;
                options.seed_base = static_cast<uint32_t>(std::stoul(argv[++i]));
            } else if (arg == "--results") {
                if (!require_value("--results")) return false;
                options.results_path = argv[++i];
            } else if (arg == "--summary") {
                if (!require_value("--summary")) return false;
                options.summary_path = argv[++i];
            } else if (arg == "--help" || arg == "-h") {
                return true;
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

// Print CLI usage details.
void PrintHelp() {
    std::cout << "Usage: vast_sim_sweep_max [options]\n"
              << "Options:\n"
              << "  --stations-max <n>       Maximum station count (default 10)\n"
              << "  --trucks-multiplier <n>  Max trucks per station (default 200)\n"
              << "  --min-trucks <n>         Minimum trucks per sweep (default 0)\n"
              << "  --hours <h>              Simulation hours (default 72)\n"
              << "  --seed-base <n>          Base RNG seed (default 1000)\n"
              << "  --results <path>         Results CSV path\n"
              << "  --summary <path>         Summary CSV path\n"
              << "  --help                   Show this help message\n";
}

}  // namespace

// Entry point for the max-load sweep.
int main(int argc, char** argv) {
    SweepOptions options;
    std::string error;
    if (!ParseArgs(argc, argv, options, error)) {
        std::cerr << error << '\n';
        PrintHelp();
        return 1;
    }

    if (options.stations_max <= 0 || options.trucks_multiplier <= 0) {
        std::cerr << "Invalid sweep configuration.\n";
        return 1;
    }
    if (options.min_trucks < 0) {
        std::cerr << "Minimum trucks must be >= 0.\n";
        return 1;
    }

    std::ofstream results(options.results_path);
    std::ofstream summary(options.summary_path);
    if (!results || !summary) {
        std::cerr << "Failed to open output CSV files.\n";
        return 1;
    }

    results << "stations,trucks,total_loads\n";
    summary << "stations,trucks_at_max,max_total_loads\n";
    results << std::fixed << std::setprecision(2);
    summary << std::fixed << std::setprecision(2);

    for (int stations = 1; stations <= options.stations_max; ++stations) {
        const int max_trucks = stations * options.trucks_multiplier;
        int max_loads = -1;
        int trucks_at_max = -1;

        for (int trucks = options.min_trucks; trucks <= max_trucks; ++trucks) {
            sim::SimulationConfig config;
            config.truck_count = trucks;
            config.station_count = stations;
            config.simulation_minutes = options.simulation_minutes;
            config.seed = options.seed_base + static_cast<uint32_t>(stations * 100000 + trucks);

            sim::Simulation simulation(config);
            sim::SimulationResult result = simulation.Run();

            results << stations << ',' << trucks << ',' << result.total_loads_started << '\n';

            if (result.total_loads_started > max_loads) {
                max_loads = result.total_loads_started;
                trucks_at_max = trucks;
            }
        }

        summary << stations << ',' << trucks_at_max << ',' << max_loads << '\n';
    }

    std::cout << "Wrote " << options.results_path << " and " << options.summary_path << '\n';
    return 0;
}
