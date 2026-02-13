#include "test_framework.h"
#include "sim/truck.h"

// Verify total time and efficiency calculations.
TEST_CASE("TruckStats totals and efficiency") {
    sim::TruckStats stats;
    stats.mining_minutes = 100.0;
    stats.travel_minutes = 20.0;
    stats.wait_minutes = 10.0;
    stats.unload_minutes = 5.0;

    REQUIRE_NEAR(stats.total_minutes(), 135.0, 1e-9);
    REQUIRE_NEAR(stats.efficiency(), 100.0 / 135.0, 1e-9);
}
