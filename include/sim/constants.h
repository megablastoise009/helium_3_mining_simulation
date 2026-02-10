#pragma once

namespace sim {
using Minutes = double;

constexpr Minutes kMinutesPerHour = 60.0;
constexpr Minutes kSimulationHoursDefault = 72.0;
constexpr Minutes kSimulationMinutesDefault = kSimulationHoursDefault * kMinutesPerHour;
constexpr Minutes kTravelMinutes = 30.0;
constexpr Minutes kUnloadMinutes = 5.0;
constexpr Minutes kMinMiningMinutes = 60.0;
constexpr Minutes kMaxMiningMinutes = 300.0;
}  // namespace sim
