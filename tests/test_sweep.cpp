// Sweep helper tests.
#include <gtest/gtest.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>

#include "sim/constants.h"
#include "sim/simulation.h"

namespace {

struct SweepBest {
    int trucks_for_min_downtime = 0;
    int trucks_for_min_wait = 0;
    double min_downtime = std::numeric_limits<double>::infinity();
    double min_wait_ratio = std::numeric_limits<double>::infinity();
};

// Clamp a value to the [0, 1] range.
double Clamp01(double value) {
    if (value < 0.0) return 0.0;
    if (value > 1.0) return 1.0;
    return value;
}

}  // namespace

// Run a parameter sweep and write summary CSVs.
TEST(SweepTest, WritesResultsAndSummary) {
    const int max_stations = 10;
    const int max_truck_multiplier = 20;
    const sim::Minutes sim_minutes = sim::kSimulationMinutesDefault;
    const double eps = 1e-9;

    std::ofstream results("sweep_results.csv");
    std::ofstream summary("sweep_summary.csv");
    ASSERT_TRUE(results.good());
    ASSERT_TRUE(summary.good());

    results << "stations,trucks,station_utilization,station_downtime,truck_wait_ratio,total_loads\n";
    summary << "stations,best_trucks_min_downtime,min_downtime,best_trucks_min_wait,truck_wait_ratio\n";
    results << std::fixed << std::setprecision(6);
    summary << std::fixed << std::setprecision(6);

    for (int stations = 1; stations <= max_stations; ++stations) {
        SweepBest best;

        for (int trucks = 1; trucks <= stations * max_truck_multiplier; ++trucks) {
            sim::SimulationConfig config;
            config.truck_count = trucks;
            config.station_count = stations;
            config.simulation_minutes = sim_minutes;
            config.seed = static_cast<uint32_t>(1000 + stations * 100 + trucks);

            sim::Simulation simulation(config);
            sim::SimulationResult result = simulation.Run();

            double total_station_busy = 0.0;
            for (const auto& stats : result.station_stats) {
                total_station_busy += stats.busy_minutes;
            }

            const double denom_station = result.simulated_minutes * stations;
            const double station_utilization = denom_station > 0.0 ? (total_station_busy / denom_station) : 0.0;
            const double station_downtime = Clamp01(1.0 - station_utilization);

            double total_truck_wait = 0.0;
            for (const auto& stats : result.truck_stats) {
                total_truck_wait += stats.wait_minutes;
            }

            const double denom_truck = result.simulated_minutes * trucks;
            const double truck_wait_ratio = denom_truck > 0.0 ? (total_truck_wait / denom_truck) : 0.0;

            EXPECT_GE(station_utilization, -eps);
            EXPECT_LE(station_utilization, 1.0 + eps);
            EXPECT_GE(truck_wait_ratio, -eps);
            EXPECT_LE(truck_wait_ratio, 1.0 + eps);

            results << stations << ','
                    << trucks << ','
                    << station_utilization << ','
                    << station_downtime << ','
                    << truck_wait_ratio << ','
                    << result.total_loads_started
                    << '\n';

            if (station_downtime < best.min_downtime) {
                best.min_downtime = station_downtime;
                best.trucks_for_min_downtime = trucks;
            }
            if (truck_wait_ratio < best.min_wait_ratio) {
                best.min_wait_ratio = truck_wait_ratio;
                best.trucks_for_min_wait = trucks;
            }
        }

        summary << stations << ','
                << best.trucks_for_min_downtime << ','
                << best.min_downtime << ','
                << best.trucks_for_min_wait << ','
                << best.min_wait_ratio << '\n';

        EXPECT_GE(best.trucks_for_min_downtime, 1);
        EXPECT_GE(best.trucks_for_min_wait, 1);
    }
}
