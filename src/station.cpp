// Unload station implementation.
#include "sim/station.h"

#include <algorithm>

namespace sim {

// Busy minutes divided by total simulation minutes.
double StationStats::utilization(Minutes sim_minutes) const {
    return sim_minutes > 0.0 ? (busy_minutes / sim_minutes) : 0.0;
}

// Average wait time per truck that started unloading.
double StationStats::average_wait() const {
    return trucks_started > 0 ? (total_time_trucks_waited_at_station / trucks_started) : 0.0;
}

// Create a station with a stable id.
MiningUnloadStation::MiningUnloadStation(int id) : id_(id) {}

// Return the station id.
int MiningUnloadStation::id() const {
    return id_;
}

// Return the next time this station is free.
Minutes MiningUnloadStation::next_available_time() const {
    return next_available_time_;
}

// Return true if the station is idle at time t.
bool MiningUnloadStation::IsIdleAt(Minutes t) const {
    return t >= next_available_time_;
}

// Return aggregate station stats.
const StationStats& MiningUnloadStation::stats() const {
    return stats_;
}

// Assign a truck arrival to this station and update station stats.
UnloadDecision MiningUnloadStation::AssignTruck(Minutes arrival_time, Minutes simulation_end_minutes) {
    UnloadDecision decision;
    decision.station_id = id_;
    // the uload will begin when the truck arrives, and the station becomes available
    decision.unload_start = std::max(arrival_time, next_available_time_);
    decision.wait_minutes = std::max(0.0, std::min(decision.unload_start, simulation_end_minutes) - arrival_time);
    decision.starts_before_end = decision.unload_start < simulation_end_minutes;
    decision.unload_end = decision.unload_start + kUnloadMinutes;
    decision.completes_before_end = decision.unload_end <= simulation_end_minutes;

    // Update station stats to account for tuck unloading
    if (decision.starts_before_end)
    {
        // Either standard unload time, or cut short?
        decision.actual_unload_minutes = std::min(kUnloadMinutes, simulation_end_minutes - decision.unload_start);

        stats_.busy_minutes += decision.actual_unload_minutes;
        
        // Keep track of how long trucks waited at this station
        stats_.total_time_trucks_waited_at_station += (decision.unload_start - arrival_time);
        stats_.trucks_started += 1;
    }
    
    else
    {
        decision.actual_unload_minutes = 0.0;
    }

    next_available_time_ = decision.unload_end;
    return decision;
}

}  // namespace sim
