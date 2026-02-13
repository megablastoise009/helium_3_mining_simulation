#pragma once

#include <queue>
#include <vector>
#include "sim/event.h"

namespace sim {

class EventQueue {
public:
    // Add an event to the queue.
    void Push(const Event& ev);
    // Return true if no events remain.
    bool Empty() const;
    // Remove and return the next event by time/sequence.
    Event Pop();

private:
    struct Compare {
        // Order by earliest time, then lowest sequence.
        bool operator()(const Event& a, const Event& b) const;
    };

    std::priority_queue<Event, std::vector<Event>, Compare> queue_;
};

}  // namespace sim
