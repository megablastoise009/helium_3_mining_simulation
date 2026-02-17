// Truck stats unit tests.
#include <gtest/gtest.h>
#include "sim/truck.h"

// Verify total time and efficiency calculations.
TEST(TruckStatsTest, TotalsAndEfficiency) {
    sim::TruckStats stats;
    stats.mining_minutes = 100.0;
    stats.travel_minutes = 20.0;
    stats.wait_minutes = 10.0;
    stats.unload_minutes = 5.0;

    EXPECT_NEAR(stats.total_minutes(), 135.0, 1e-9);
    EXPECT_NEAR(stats.efficiency(), 100.0 / 135.0, 1e-9);
}

// Efficiency should be zero when total time is zero.
TEST(TruckStatsTest, EfficiencyZeroWhenNoTime) {
    sim::TruckStats stats;
    EXPECT_NEAR(stats.total_minutes(), 0.0, 1e-9);
    EXPECT_NEAR(stats.efficiency(), 0.0, 1e-9);
}
