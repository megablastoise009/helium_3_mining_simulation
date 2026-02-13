#include "sim/event_queue.h"

namespace sim {

// Add an event to the queue.
void EventQueue::Push(const Event& ev) {
    queue_.push(ev);
}

// Return true if no events remain.
bool EventQueue::Empty() const {
    return queue_.empty();
}

// Remove and return the next event by time/sequence.
Event EventQueue::Pop() {
    Event ev = queue_.top();
    queue_.pop();
    return ev;
}

// Order by earliest time, then lowest sequence.
bool EventQueue::Compare::operator()(const Event& a, const Event& b) const {
    if (a.time != b.time) {
        return a.time > b.time;
    }
    return a.sequence > b.sequence;
}

}  // namespace sim
