// Simulation configuration, results, and engine interface.
#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include <vector>
#include "sim/constants.h"
#include "sim/event_queue.h"
#include "sim/station.h"
#include "sim/truck.h"

namespace sim {

// Configuration for a simulation
struct SimulationConfig {
    int truck_count = 5;
    int station_count = 2;
    Minutes simulation_minutes = kSimulationMinutesDefault;
    std::optional<uint32_t> seed;
};

// Results after a run simulation
struct SimulationResult {
    SimulationConfig config;
    Minutes simulated_minutes = 0.0;
    std::vector<TruckStats> truck_stats;
    std::vector<StationStats> station_stats;
    int total_loads_started = 0;
};

class Simulation {
public:
    /// @brief Initialize a simulation with the provided configuration.
    explicit Simulation(SimulationConfig config);

    /// @brief Run the simulation and return aggregated results.
    SimulationResult Run();

private:
    /// @brief Begin a mining phase for a truck.
    void StartMining(int truck_id, Minutes start_time);

    /// @brief Schedule travel from mine site to a station.
    void ScheduleTravelToStation(int truck_id, Minutes start_time);

    /// @brief Schedule travel from station back to the mine site.
    void ScheduleTravelToMine(int truck_id, Minutes start_time);

    /// @brief Handle a truck arrival at a station.
    void HandleArriveStation(int truck_id, Minutes time);

    /// @brief Pick the best station for a truck arriving at arrival_time.
    int SelectStation(Minutes arrival_time) const;

    /// @brief Sample a mining duration within configured bounds.
    Minutes SampleMiningMinutes();

    // Member data:
    SimulationConfig config_;
    Minutes end_time_ = 0.0;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> mining_dist_;
    std::vector<MiningUnloadStation> stations_;
    std::vector<TruckStats> trucks_;
    EventQueue queue_;
    std::size_t sequence_ = 0;
    int total_loads_started_ = 0;
};

}  // namespace sim
