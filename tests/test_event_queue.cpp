// Event queue unit tests.
#include <gtest/gtest.h>
#include "sim/event_queue.h"

// Validate ordering by time then sequence.
TEST(EventQueueTest, OrdersByTimeThenSequence) {
    sim::EventQueue queue;
    queue.Push({10.0, sim::EventType::MineComplete, 0, 2});
    queue.Push({5.0, sim::EventType::MineComplete, 0, 3});
    queue.Push({10.0, sim::EventType::MineComplete, 0, 1});

    sim::Event first = queue.Pop();
    EXPECT_NEAR(first.time, 5.0, 1e-9);

    sim::Event second = queue.Pop();
    EXPECT_NEAR(second.time, 10.0, 1e-9);
    EXPECT_EQ(second.sequence, 1u);

    sim::Event third = queue.Pop();
    EXPECT_NEAR(third.time, 10.0, 1e-9);
    EXPECT_EQ(third.sequence, 2u);
}

// Empty should reflect whether events remain in the queue.
TEST(EventQueueTest, EmptyReflectsState) {
    sim::EventQueue queue;
    EXPECT_TRUE(queue.Empty());
    queue.Push({1.0, sim::EventType::MineComplete, 0, 0});
    EXPECT_FALSE(queue.Empty());
    queue.Pop();
    EXPECT_TRUE(queue.Empty());
}
