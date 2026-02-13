#include "test_framework.h"
#include "sim/station.h"

// Idle station should start unloading immediately.
TEST_CASE("Station assigns without wait when idle") {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(0.0, 100.0);

    REQUIRE_NEAR(decision.wait_minutes, 0.0, 1e-9);
    REQUIRE(decision.starts_before_end);
    REQUIRE_NEAR(decision.actual_unload_minutes, sim::kUnloadMinutes, 1e-9);
    REQUIRE_NEAR(station.stats().busy_minutes, sim::kUnloadMinutes, 1e-9);
    REQUIRE(station.stats().trucks_started == 1);
}

// Busy station should queue and add wait time.
TEST_CASE("Station queues when busy") {
    sim::MiningUnloadStation station(0);
    station.AssignTruck(0.0, 100.0);
    sim::UnloadDecision decision = station.AssignTruck(0.0, 100.0);

    REQUIRE_NEAR(decision.wait_minutes, sim::kUnloadMinutes, 1e-9);
    REQUIRE(decision.starts_before_end);
    REQUIRE(station.stats().trucks_started == 2);
}

// Unload should truncate if the simulation ends mid-unload.
TEST_CASE("Station unload partially when simulation ends") {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(8.0, 10.0);

    REQUIRE(decision.starts_before_end);
    REQUIRE_NEAR(decision.actual_unload_minutes, 2.0, 1e-9);
    REQUIRE_NEAR(station.stats().busy_minutes, 2.0, 1e-9);
}

// If arrival is after simulation end, stats should not change.
TEST_CASE("Station ignores arrivals after simulation end") {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(10.0, 5.0);

    REQUIRE(!decision.starts_before_end);
    REQUIRE_NEAR(decision.actual_unload_minutes, 0.0, 1e-9);
    REQUIRE_NEAR(station.stats().busy_minutes, 0.0, 1e-9);
    REQUIRE(station.stats().trucks_started == 0);
}

// If unload ends exactly at simulation end, it should count fully.
TEST_CASE("Station unload ends exactly at simulation end") {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(5.0, 10.0);

    REQUIRE(decision.starts_before_end);
    REQUIRE(decision.completes_before_end);
    REQUIRE_NEAR(decision.actual_unload_minutes, sim::kUnloadMinutes, 1e-9);
    REQUIRE_NEAR(station.stats().busy_minutes, sim::kUnloadMinutes, 1e-9);
    REQUIRE(station.stats().trucks_started == 1);
}
