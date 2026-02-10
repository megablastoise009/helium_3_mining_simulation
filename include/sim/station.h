#pragma once

#include <algorithm>
#include "sim/constants.h"

namespace sim {

struct StationStats {
    Minutes busy_minutes = 0.0;
    Minutes total_wait_minutes = 0.0;
    int trucks_started = 0;

    double utilization(Minutes sim_minutes) const {
        return sim_minutes > 0.0 ? (busy_minutes / sim_minutes) : 0.0;
    }

    double average_wait() const {
        return trucks_started > 0 ? (total_wait_minutes / trucks_started) : 0.0;
    }
};

struct UnloadDecision {
    int station_id = 0;
    Minutes unload_start = 0.0;
    Minutes unload_end = 0.0;
    Minutes wait_minutes = 0.0;
    Minutes actual_unload_minutes = 0.0;
    bool starts_before_end = false;
    bool completes_before_end = false;
};

class MiningUnloadStation {
public:
    explicit MiningUnloadStation(int id) : id_(id) {}

    int id() const { return id_; }
    Minutes next_available_time() const { return next_available_time_; }
    bool IsIdleAt(Minutes t) const { return t >= next_available_time_; }
    const StationStats& stats() const { return stats_; }

    UnloadDecision AssignTruck(Minutes arrival_time, Minutes simulation_end_minutes) {
        UnloadDecision decision;
        decision.station_id = id_;
        decision.unload_start = std::max(arrival_time, next_available_time_);
        decision.wait_minutes = std::max(0.0, std::min(decision.unload_start, simulation_end_minutes) - arrival_time);
        decision.starts_before_end = decision.unload_start < simulation_end_minutes;
        decision.unload_end = decision.unload_start + kUnloadMinutes;
        decision.completes_before_end = decision.unload_end <= simulation_end_minutes;
        if (decision.starts_before_end) {
            decision.actual_unload_minutes = std::min(kUnloadMinutes, simulation_end_minutes - decision.unload_start);
            stats_.busy_minutes += decision.actual_unload_minutes;
            stats_.total_wait_minutes += (decision.unload_start - arrival_time);
            stats_.trucks_started += 1;
        } else {
            decision.actual_unload_minutes = 0.0;
        }
        next_available_time_ = decision.unload_end;
        return decision;
    }

private:
    int id_ = 0;
    Minutes next_available_time_ = 0.0;
    StationStats stats_;
};

}  // namespace sim
