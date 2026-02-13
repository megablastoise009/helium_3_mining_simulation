#include "test_framework.h"
#include "sim/simulation.h"

// Ensure truck totals cover the full simulation window.
void TestSimulationTotalsCoverFullWindow() {
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

// Zero trucks or stations should yield empty or zeroed stats.
void TestSimulationHandlesZeroTrucksOrStations() {
    sim::SimulationConfig no_trucks;
    no_trucks.truck_count = 0;
    no_trucks.station_count = 2;
    no_trucks.simulation_minutes = 60.0;
    no_trucks.seed = 1u;

    sim::Simulation sim_no_trucks(no_trucks);
    sim::SimulationResult res_no_trucks = sim_no_trucks.Run();
    REQUIRE(res_no_trucks.truck_stats.empty());
    REQUIRE(res_no_trucks.station_stats.size() == 2);
    REQUIRE(res_no_trucks.total_loads_started == 0);

    sim::SimulationConfig no_stations;
    no_stations.truck_count = 2;
    no_stations.station_count = 0;
    no_stations.simulation_minutes = 60.0;
    no_stations.seed = 2u;

    sim::Simulation sim_no_stations(no_stations);
    sim::SimulationResult res_no_stations = sim_no_stations.Run();
    REQUIRE(res_no_stations.truck_stats.size() == 2);
    REQUIRE(res_no_stations.station_stats.empty());
    REQUIRE(res_no_stations.total_loads_started == 0);
}

// Very short simulations should not record unloads.
void TestSimulationShortDurationProducesNoUnloads() {
    sim::SimulationConfig config;
    config.truck_count = 1;
    config.station_count = 1;
    config.simulation_minutes = 10.0;
    config.seed = 42u;

    sim::Simulation simulation(config);
    sim::SimulationResult result = simulation.Run();

    REQUIRE(result.total_loads_started == 0);
    REQUIRE_NEAR(result.station_stats[0].busy_minutes, 0.0, 1e-9);
}

// Aggregate station busy time should match total unload time.
void TestSimulationUnloadMinutesMatchStationBusyMinutes() {
    sim::SimulationConfig config;
    config.truck_count = 5;
    config.station_count = 2;
    config.simulation_minutes = 180.0;
    config.seed = 7u;

    sim::Simulation simulation(config);
    sim::SimulationResult result = simulation.Run();

    double total_truck_unload = 0.0;
    for (const auto& stats : result.truck_stats) {
        total_truck_unload += stats.unload_minutes;
    }

    double total_station_busy = 0.0;
    for (const auto& stats : result.station_stats) {
        total_station_busy += stats.busy_minutes;
    }

    REQUIRE_NEAR(total_truck_unload, total_station_busy, 1e-6);
}

// Total loads started should match the sum across trucks.
void TestSimulationTotalLoadsMatchPerTruckLoads() {
    sim::SimulationConfig config;
    config.truck_count = 4;
    config.station_count = 2;
    config.simulation_minutes = 240.0;
    config.seed = 9u;

    sim::Simulation simulation(config);
    sim::SimulationResult result = simulation.Run();

    int total = 0;
    for (const auto& stats : result.truck_stats) {
        total += stats.loads_started;
    }

    REQUIRE(total == result.total_loads_started);
}
