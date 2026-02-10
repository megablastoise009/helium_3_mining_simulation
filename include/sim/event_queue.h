#pragma once

#include <queue>
#include <vector>
#include "sim/event.h"

namespace sim {

class EventQueue {
public:
    void Push(const Event& ev) { queue_.push(ev); }
    bool Empty() const { return queue_.empty(); }
    Event Pop() {
        Event ev = queue_.top();
        queue_.pop();
        return ev;
    }

private:
    struct Compare {
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
