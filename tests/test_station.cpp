// Unload station unit tests.
#include <gtest/gtest.h>
#include "sim/station.h"

// Idle station should start unloading immediately.
TEST(StationTest, AssignsWithoutWaitWhenIdle) {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(0.0, 100.0);

    EXPECT_NEAR(decision.wait_minutes, 0.0, 1e-9);
    EXPECT_TRUE(decision.starts_before_end);
    EXPECT_NEAR(decision.actual_unload_minutes, sim::kUnloadMinutes, 1e-9);
    EXPECT_NEAR(station.stats().busy_minutes, sim::kUnloadMinutes, 1e-9);
    EXPECT_EQ(station.stats().trucks_started, 1);
}

// Busy station should queue and add wait time.
TEST(StationTest, QueuesWhenBusy) {
    sim::MiningUnloadStation station(0);
    station.AssignTruck(0.0, 100.0);
    sim::UnloadDecision decision = station.AssignTruck(0.0, 100.0);

    EXPECT_NEAR(decision.wait_minutes, sim::kUnloadMinutes, 1e-9);
    EXPECT_TRUE(decision.starts_before_end);
    EXPECT_EQ(station.stats().trucks_started, 2);
}

// Unload should truncate if the simulation ends mid-unload.
TEST(StationTest, UnloadPartiallyWhenSimulationEnds) {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(8.0, 10.0);

    EXPECT_TRUE(decision.starts_before_end);
    EXPECT_NEAR(decision.actual_unload_minutes, 2.0, 1e-9);
    EXPECT_NEAR(station.stats().busy_minutes, 2.0, 1e-9);
}

// If arrival is after simulation end, stats should not change.
TEST(StationTest, IgnoresArrivalsAfterSimulationEnd) {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(10.0, 5.0);

    EXPECT_FALSE(decision.starts_before_end);
    EXPECT_NEAR(decision.actual_unload_minutes, 0.0, 1e-9);
    EXPECT_NEAR(station.stats().busy_minutes, 0.0, 1e-9);
    EXPECT_EQ(station.stats().trucks_started, 0);
}

// If unload ends exactly at simulation end, it should count fully.
TEST(StationTest, UnloadEndsAtSimulationEnd) {
    sim::MiningUnloadStation station(0);
    sim::UnloadDecision decision = station.AssignTruck(5.0, 10.0);

    EXPECT_TRUE(decision.starts_before_end);
    EXPECT_TRUE(decision.completes_before_end);
    EXPECT_NEAR(decision.actual_unload_minutes, sim::kUnloadMinutes, 1e-9);
    EXPECT_NEAR(station.stats().busy_minutes, sim::kUnloadMinutes, 1e-9);
    EXPECT_EQ(station.stats().trucks_started, 1);
}
