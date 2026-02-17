// Event types and payload for the simulation queue.
#pragma once

#include <cstddef>
#include "sim/constants.h"

namespace sim {

enum class EventType {
    MineComplete,
    ArriveStation,
    UnloadComplete,
    ArriveMineSite,
};

struct Event {
    Minutes time = 0.0;
    EventType type = EventType::MineComplete;
    int truck_id = 0;
    std::size_t sequence = 0;
};

}  // namespace sim
