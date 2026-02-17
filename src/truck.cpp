// Truck stats implementation.
#include "sim/truck.h"

namespace sim {

// Total minutes spent across all activities.
Minutes TruckStats::total_minutes() const {
    return mining_minutes + travel_minutes + wait_minutes + unload_minutes;
}

// Ratio of mining time to total time.
double TruckStats::efficiency() const {
    const Minutes total = total_minutes();
    return total > 0.0 ? (mining_minutes / total) : 0.0;
}

}  // namespace sim
