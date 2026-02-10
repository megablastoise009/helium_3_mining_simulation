#include "test_framework.h"
#include "sim/simulation.h"

TEST_CASE("Simulation totals cover full window") {
    sim::SimulationConfig config;
    config.truck_count = 3;
    config.station_count = 1;
    config.simulation_minutes = 120.0;
    config.seed = 123u;

    sim::Simulation simulation(config);
    sim::SimulationResult result = simulation.Run();

    for (const auto& stats : result.truck_stats) {
        REQUIRE_NEAR(stats.total_minutes(), config.simulation_minutes, 1e-6);
    }

    for (const auto& stats : result.station_stats) {
        REQUIRE(stats.busy_minutes >= 0.0);
        REQUIRE(stats.busy_minutes <= config.simulation_minutes + 1e-6);
        REQUIRE(stats.utilization(config.simulation_minutes) <= 1.0 + 1e-6);
    }
}
