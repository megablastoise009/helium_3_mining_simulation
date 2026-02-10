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

struct SimulationConfig {
    int truck_count = 5;
    int station_count = 2;
    Minutes simulation_minutes = kSimulationMinutesDefault;
    std::optional<uint32_t> seed;
};

struct SimulationResult {
    SimulationConfig config;
    Minutes simulated_minutes = 0.0;
    std::vector<TruckStats> truck_stats;
    std::vector<StationStats> station_stats;
    int total_loads_started = 0;
};

class Simulation {
public:
    explicit Simulation(SimulationConfig config);

    SimulationResult Run();

private:
    void StartMining(int truck_id, Minutes start_time);
    void ScheduleTravelToStation(int truck_id, Minutes start_time);
    void ScheduleTravelToMine(int truck_id, Minutes start_time);
    void HandleArriveStation(int truck_id, Minutes time);
    int SelectStation(Minutes arrival_time) const;
    Minutes SampleMiningMinutes();

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
