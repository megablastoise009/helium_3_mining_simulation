#include "test_framework.h"
#include "sim/event_queue.h"

// Validate ordering by time then sequence.
TEST_CASE("EventQueue orders by time then sequence") {
    sim::EventQueue queue;
    queue.Push({10.0, sim::EventType::MineComplete, 0, 2});
    queue.Push({5.0, sim::EventType::MineComplete, 0, 3});
    queue.Push({10.0, sim::EventType::MineComplete, 0, 1});

    sim::Event first = queue.Pop();
    REQUIRE_NEAR(first.time, 5.0, 1e-9);

    sim::Event second = queue.Pop();
    REQUIRE_NEAR(second.time, 10.0, 1e-9);
    REQUIRE(second.sequence == 1);

    sim::Event third = queue.Pop();
    REQUIRE_NEAR(third.time, 10.0, 1e-9);
    REQUIRE(third.sequence == 2);
}

// Empty should reflect whether events remain in the queue.
TEST_CASE("EventQueue empty reflects state") {
    sim::EventQueue queue;
    REQUIRE(queue.Empty());
    queue.Push({1.0, sim::EventType::MineComplete, 0, 0});
    REQUIRE(!queue.Empty());
    queue.Pop();
    REQUIRE(queue.Empty());
}
