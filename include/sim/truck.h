#pragma once

#include "sim/constants.h"

namespace sim {

struct TruckStats {
    Minutes mining_minutes = 0.0;
    Minutes travel_minutes = 0.0;
    Minutes wait_minutes = 0.0;
    Minutes unload_minutes = 0.0;
    int loads_started = 0;

    // Total minutes spent across all activities.
    Minutes total_minutes() const {
        return mining_minutes + travel_minutes + wait_minutes + unload_minutes;
    }

    // Ratio of mining time to total time.
    double efficiency() const {
        const Minutes total = total_minutes();
        return total > 0.0 ? (mining_minutes / total) : 0.0;
    }
};

}  // namespace sim
