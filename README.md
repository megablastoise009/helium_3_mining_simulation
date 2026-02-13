# Vast Interview Simulation

![CI](https://github.com/megablastoise009/helium_3_mining_simulation/actions/workflows/ci.yml/badge.svg)

Event-driven simulation for the Vast take-home challenge. The simulation runs 72 hours (configurable) and models mining trucks, travel, unload stations, and queueing behavior.

## Prereqs

- C++20 compiler
- CMake >= 3.20
- Ninja (recommended)
- Python 3 (only for plots)

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

## Run

Windows:

```powershell
build\vast_sim_cli.exe --trucks 10 --stations 3 --seed 42 --csv results.csv
```

Linux/macOS:

```bash
./build/vast_sim_cli --trucks 10 --stations 3 --seed 42 --csv results.csv
```

## Output

- Command line summary for each truck and station.
- CSV file with per-truck and per-station metrics.

## Sweep + Plots

The test suite includes a sweep that runs trucks from `1..(20 * stations)` for `stations=1..10`
and writes `sweep_results.csv` + `sweep_summary.csv` in the build directory. To plot the summary:

```powershell
python scripts/plot_sweep.py --summary build/sweep_summary.csv
```

## Docker

Build and run with Docker:

```powershell
docker build -t vast-sim .
docker run --rm vast-sim
```
