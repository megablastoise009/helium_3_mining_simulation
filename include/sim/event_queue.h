#pragma once

#include <queue>
#include <vector>
#include "sim/event.h"

namespace sim {

class EventQueue {
public:
    // Add an event to the queue.
    void Push(const Event& ev) { queue_.push(ev); }
    // Return true if no events remain.
    bool Empty() const { return queue_.empty(); }
    // Remove and return the next event by time/sequence.
    Event Pop() {
        Event ev = queue_.top();
        queue_.pop();
        return ev;
    }

private:
    struct Compare {
        // Order by earliest time, then lowest sequence.
        bool operator()(const Event& a, const Event& b) const {
            if (a.time != b.time) {
                return a.time > b.time;
            }
            return a.sequence > b.sequence;
        }
    };

    std::priority_queue<Event, std::vector<Event>, Compare> queue_;
};

}  // namespace sim
