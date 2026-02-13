#pragma once

#include <string>
#include <utility>
#include <vector>

#include "test_framework.h"

// Truck stats tests.
void TestTruckStatsTotalsAndEfficiency();
void TestTruckStatsEfficiencyZero();

// Station tests.
void TestStationAssignsWithoutWaitWhenIdle();
void TestStationQueuesWhenBusy();
void TestStationUnloadPartiallyWhenSimulationEnds();
void TestStationIgnoresArrivalsAfterSimulationEnd();
void TestStationUnloadEndsAtSimulationEnd();

// Event queue tests.
void TestEventQueueOrdersByTimeThenSequence();
void TestEventQueueEmptyReflectsState();

// Simulation tests.
void TestSimulationTotalsCoverFullWindow();
void TestSimulationHandlesZeroTrucksOrStations();
void TestSimulationShortDurationProducesNoUnloads();
void TestSimulationUnloadMinutesMatchStationBusyMinutes();
void TestSimulationTotalLoadsMatchPerTruckLoads();

// Sweep tests.
void TestSweepWritesResultsAndSummary();

// Return the ordered list of tests to run.
inline std::vector<std::pair<std::string, test::TestFn>> AllTests() {
    return {
        {"TruckStats totals and efficiency", &TestTruckStatsTotalsAndEfficiency},
        {"TruckStats efficiency is zero with no time", &TestTruckStatsEfficiencyZero},
        {"Station assigns without wait when idle", &TestStationAssignsWithoutWaitWhenIdle},
        {"Station queues when busy", &TestStationQueuesWhenBusy},
        {"Station unload partially when simulation ends", &TestStationUnloadPartiallyWhenSimulationEnds},
        {"Station ignores arrivals after simulation end", &TestStationIgnoresArrivalsAfterSimulationEnd},
        {"Station unload ends exactly at simulation end", &TestStationUnloadEndsAtSimulationEnd},
        {"EventQueue orders by time then sequence", &TestEventQueueOrdersByTimeThenSequence},
        {"EventQueue empty reflects state", &TestEventQueueEmptyReflectsState},
        {"Simulation totals cover full window", &TestSimulationTotalsCoverFullWindow},
        {"Simulation handles zero trucks or stations", &TestSimulationHandlesZeroTrucksOrStations},
        {"Simulation with short duration produces no unloads", &TestSimulationShortDurationProducesNoUnloads},
        {"Simulation unload minutes match station busy minutes", &TestSimulationUnloadMinutesMatchStationBusyMinutes},
        {"Simulation total loads matches per-truck loads", &TestSimulationTotalLoadsMatchPerTruckLoads},
        {"Sweep writes results and summary", &TestSweepWritesResultsAndSummary},
    };
}
