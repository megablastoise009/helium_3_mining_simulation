# helium_3_mining Simulation

![CI](https://github.com/megablastoise009/helium_3_mining_simulation/actions/workflows/ci.yml/badge.svg)

Event-driven simulation for the helium_3_mining take-home challenge. The simulation runs 72 hours (configurable) and models mining trucks, travel, unload stations, and queueing behavior.

## Prereqs

- C++20 compiler
- CMake >= 3.20
- Ninja (recommended)
- Python 3 (only for plots)

## Docker

```powershell
docker build -t helium_3_mining-sim .
docker run --rm helium_3_mining-sim
```

## Build

On Windows, run from a Developer Command Prompt so the MSVC toolchain is available.

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

If you don't have Ninja, omit `-G Ninja`.

## Tests

```powershell
ctest --test-dir build --output-on-failure
```

Tests use GoogleTest and a vendored copy is included under `third_party/googletest`
so builds work offline. If you remove the vendored copy, you can either install a system
GTest and pass `-DUSE_SYSTEM_GTEST=ON`, or point CMake at a local checkout with
`-DGTEST_SOURCE_DIR=...`.

## Run

```powershell
build\helium_3_mining_sim_cli.exe --trucks 10 --stations 3 --seed 42 --csv results.csv
```

## Output

Command line summary for each truck and station, plus a CSV with per-truck and per-station metrics.

## Design Notes

- Event-driven simulation using a time ordered priority queue.
- Station selection: first idle station, otherwise the shortest wait.
- Mining duration is uniform in the 1–5 hour range; a seed makes runs reproducible.

## Example Output

```
Simulation Summary
Trucks: 3, Stations: 2, Duration (hours): 72.00, Seed: 42
Total unloads started: 53
```

## Sweep + Plots

The test suite includes a sweep that runs trucks from `1..(20 * stations)` for `stations=1..10`
and writes `sweep_results.csv` + `sweep_summary.csv` in the build directory. To plot the summary:

```powershell
python scripts/plot_sweep.py --summary build/sweep_summary.csv
```

For a max-load sweep that runs trucks from `0..(200 * stations)`:

```powershell
build\helium_3_mining_sim_sweep_max.exe --stations-max 10 --trucks-multiplier 200 --results build\sweep_max_results.csv --summary build\sweep_max_summary.csv
python scripts/plot_sweep_max.py --summary build\sweep_max_summary.csv
```
