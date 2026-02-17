# Software Design Description (SDD)

## Summary: `include/sim/constants.h`

Defines simulation time units and constants:
- `Minutes` is a `double` used for all durations.
- `kSimulationHoursDefault = 72` and `kSimulationMinutesDefault = 72 * 60`.
- Fixed durations: `kTravelMinutes = 30`, `kUnloadMinutes = 5`.
- Mining duration bounds: `kMinMiningMinutes = 60`, `kMaxMiningMinutes = 300`.

These values drive the timing model and are referenced by the simulation engine, station logic, and CLI defaults.

## Summary: `include/sim/event_queue.h`

Defines `EventQueue`, a small wrapper around `std::priority_queue` used to order simulation events.  
Public API:
- `Push(const Event&)`: add an event.
- `Empty()`: check if any events remain.
- `Pop()`: remove and return the next event.

Ordering is defined by the nested `Compare` functor:
- Earlier `time` has higher priority.
- If `time` ties, lower `sequence` has higher priority.

## Summary: `include/sim/event_types.h`

Defines the event payload and enum used by the simulation queue:
- `EventType` enumerates the simulation state transitions (mine complete, arrive station, unload complete, arrive mine site).
- `Event` carries the scheduled `time`, `type`, `truck_id`, and a `sequence` for deterministic tie-breaking.

## Summary: `src/event_queue.cpp`

Implements the `EventQueue` methods and comparison logic:
- `Push` forwards to the underlying priority queue.
- `Empty` mirrors the queue’s empty state.
- `Pop` returns the top element and removes it.
- `Compare::operator()` enforces time-first, sequence-second ordering.

The `sequence` field provides deterministic ordering for events that occur at the same time.

## Summary: `include/sim/station.h`

Declares the unload station interface and station-level stats:
- `StationStats` aggregates busy time, total wait time, and truck count, and provides utilization/average wait helpers.
- `MiningUnloadStation` exposes station state and the `AssignTruck` method used by the simulation.

## Summary: `src/station.cpp`

Implements unload station behavior:
- Computes wait time and unload windows for arriving trucks.
- Updates station stats and next-available time.
- Handles partial unloads when the simulation ends mid-service.

## Summary: `include/sim/truck.h`

Declares `TruckStats`, which aggregates per-truck time spent mining, traveling, waiting, and unloading,
plus helper methods for total time and efficiency.

## Summary: `src/truck.cpp`

Implements `TruckStats` helper methods for total time and efficiency ratio.

## Summary: `include/sim/simulation.h`

Declares the simulation interface and data structures:
- `SimulationConfig` for parameterizing a run (trucks, stations, duration, seed).
- `SimulationResult` for aggregated output metrics.
- `Simulation` class interface (constructor + `Run`) and private helpers used by the engine.

## Summary: `src/simulation.cpp`

Implements the event-driven simulation:
- Initializes the RNG and event queue.
- Schedules per-truck mining, travel, and unloading events.

Private helpers used by the engine:
- Mining:
  - `StartMining(truck_id, start_time)`
  - `SampleMiningMinutes()`
- Travel:
  - `ScheduleTravelToStation(truck_id, start_time)`
  - `ScheduleTravelToMine(truck_id, start_time)`
- Station interaction:
  - `HandleArriveStation(truck_id, time)`
  - `SelectStation(arrival_time)`

## Note: `std::vector::reserve`

Several collections pre-allocate capacity using `reserve(...)` before filling vectors.
This does not change the vector size or create elements; it only reduces reallocations
and keeps references/iterators stable during repeated `push_back`/`emplace_back`.
