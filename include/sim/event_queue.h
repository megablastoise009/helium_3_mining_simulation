// Priority queue for simulation events.
#pragma once

#include <queue>
#include <vector>
#include "sim/event_types.h"

namespace sim {

class EventQueue {
public:
    /// @brief Add an event to the queue.
    void Push(const Event& ev);

    /// @brief Return true if no events remain.
    bool Empty() const;

    /// @brief Remove and return the next event by time/sequence.
    Event Pop();

private:
    struct Compare {
        // Order by earliest time, then lowest sequence.
        bool operator()(const Event& a, const Event& b) const;
    };

    std::priority_queue<Event, std::vector<Event>, Compare> queue_;
};

}  // namespace sim
