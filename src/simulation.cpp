#include "sim/simulation.h"

#include <algorithm>
#include <random>

namespace sim {

// Initialize the simulation state and RNG.
Simulation::Simulation(SimulationConfig config) : config_(config) {
    if (config_.truck_count < 0) {
        config_.truck_count = 0;
    }
    if (config_.station_count < 0) {
        config_.station_count = 0;
    }
    if (config_.simulation_minutes < 0.0) {
        config_.simulation_minutes = 0.0;
    }

    const uint32_t seed = config_.seed.value_or(std::random_device{}());
    config_.seed = seed;
    rng_.seed(seed);
    mining_dist_ = std::uniform_real_distribution<double>(kMinMiningMinutes, kMaxMiningMinutes);
    end_time_ = config_.simulation_minutes;

    stations_.reserve(static_cast<std::size_t>(config_.station_count));
    for (int i = 0; i < config_.station_count; ++i) {
        stations_.emplace_back(i);
    }

    trucks_.resize(static_cast<std::size_t>(config_.truck_count));
}

// Run the event-driven simulation and return aggregate results.
SimulationResult Simulation::Run() {
    SimulationResult result;
    result.config = config_;
    result.simulated_minutes = end_time_;
    result.truck_stats = trucks_;

    if (config_.truck_count == 0 || config_.station_count == 0 || end_time_ <= 0.0) {
        result.station_stats.reserve(static_cast<std::size_t>(config_.station_count));
        for (const auto& station : stations_) {
            result.station_stats.push_back(station.stats());
        }
        return result;
    }

    for (int truck_id = 0; truck_id < config_.truck_count; ++truck_id) {
        StartMining(truck_id, 0.0);
    }

    while (!queue_.Empty()) {
        Event ev = queue_.Pop();
        if (ev.time > end_time_) {
            break;
        }

        switch (ev.type) {
            case EventType::MineComplete:
                ScheduleTravelToStation(ev.truck_id, ev.time);
                break;
            case EventType::ArriveStation:
                HandleArriveStation(ev.truck_id, ev.time);
                break;
            case EventType::UnloadComplete:
                ScheduleTravelToMine(ev.truck_id, ev.time);
                break;
            case EventType::ArriveMineSite:
                StartMining(ev.truck_id, ev.time);
                break;
        }
    }

    result.truck_stats = trucks_;
    result.total_loads_started = total_loads_started_;
    result.station_stats.reserve(stations_.size());
    for (const auto& station : stations_) {
        result.station_stats.push_back(station.stats());
    }

    return result;
}

// Start a mining phase for a truck and schedule completion.
void Simulation::StartMining(int truck_id, Minutes start_time) {
    if (start_time >= end_time_) {
        return;
    }

    const Minutes duration = SampleMiningMinutes();
    if (start_time + duration <= end_time_) {
        trucks_[static_cast<std::size_t>(truck_id)].mining_minutes += duration;
        queue_.Push({start_time + duration, EventType::MineComplete, truck_id, sequence_++});
    } else {
        trucks_[static_cast<std::size_t>(truck_id)].mining_minutes += end_time_ - start_time;
    }
}

// Schedule travel from mine site to station.
void Simulation::ScheduleTravelToStation(int truck_id, Minutes start_time) {
    if (start_time >= end_time_) {
        return;
    }

    if (start_time + kTravelMinutes <= end_time_) {
        trucks_[static_cast<std::size_t>(truck_id)].travel_minutes += kTravelMinutes;
        queue_.Push({start_time + kTravelMinutes, EventType::ArriveStation, truck_id, sequence_++});
    } else {
        trucks_[static_cast<std::size_t>(truck_id)].travel_minutes += end_time_ - start_time;
    }
}

// Schedule travel from station back to the mine site.
void Simulation::ScheduleTravelToMine(int truck_id, Minutes start_time) {
    if (start_time >= end_time_) {
        return;
    }

    if (start_time + kTravelMinutes <= end_time_) {
        trucks_[static_cast<std::size_t>(truck_id)].travel_minutes += kTravelMinutes;
        queue_.Push({start_time + kTravelMinutes, EventType::ArriveMineSite, truck_id, sequence_++});
    } else {
        trucks_[static_cast<std::size_t>(truck_id)].travel_minutes += end_time_ - start_time;
    }
}

// Handle a truck arriving at a station and enqueue unload completion.
void Simulation::HandleArriveStation(int truck_id, Minutes time) {
    const int station_id = SelectStation(time);
    MiningUnloadStation& station = stations_[static_cast<std::size_t>(station_id)];
    const UnloadDecision decision = station.AssignTruck(time, end_time_);

    TruckStats& truck = trucks_[static_cast<std::size_t>(truck_id)];
    truck.wait_minutes += decision.wait_minutes;

    if (!decision.starts_before_end) {
        return;
    }

    truck.unload_minutes += decision.actual_unload_minutes;
    truck.loads_started += 1;
    total_loads_started_ += 1;

    if (decision.unload_end <= end_time_) {
        queue_.Push({decision.unload_end, EventType::UnloadComplete, truck_id, sequence_++});
    }
}

// Choose a station based on earliest availability (first idle else shortest wait).
int Simulation::SelectStation(Minutes arrival_time) const {
    int first_idle = -1;
    for (std::size_t i = 0; i < stations_.size(); ++i) {
        if (stations_[i].IsIdleAt(arrival_time)) {
            first_idle = static_cast<int>(i);
            break;
        }
    }

    if (first_idle >= 0) {
        return first_idle;
    }

    int best = 0;
    Minutes best_wait = stations_[0].next_available_time() - arrival_time;
    for (std::size_t i = 1; i < stations_.size(); ++i) {
        const Minutes wait = stations_[i].next_available_time() - arrival_time;
        if (wait < best_wait) {
            best_wait = wait;
            best = static_cast<int>(i);
        }
    }

    return best;
}

// Sample a mining duration in minutes.
Minutes Simulation::SampleMiningMinutes() {
    return mining_dist_(rng_);
}

}  // namespace sim
