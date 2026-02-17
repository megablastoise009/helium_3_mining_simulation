// Unload station model and station-level statistics.
#pragma once

#include "sim/constants.h"

namespace sim {

struct StationStats {
    Minutes busy_minutes = 0.0;
    Minutes total_time_trucks_waited_at_station = 0.0;
    int trucks_started = 0;

    // Busy minutes divided by total simulation minutes.
    double utilization(Minutes sim_minutes) const;

    // Average wait time per truck that started unloading.
    double average_wait() const;
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
    // Create a station with a stable id.
    explicit MiningUnloadStation(int id);

    // Return the station id.
    int id() const;

    // Return the next time this station is free.
    Minutes next_available_time() const;

    // Return true if the station is idle at time t.
    bool IsIdleAt(Minutes t) const;

    // Return aggregate station stats.
    const StationStats& stats() const;

    // Assign a truck arrival to this station and update station stats.
    UnloadDecision AssignTruck(Minutes arrival_time, Minutes simulation_end_minutes);

private:
    int id_ = 0;
    Minutes next_available_time_ = 0.0;
    StationStats stats_;
};

}  // namespace sim
