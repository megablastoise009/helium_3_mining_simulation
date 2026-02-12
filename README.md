# Vast Interview Simulation

Event-driven simulation for the Vast take-home challenge. The simulation runs 72 hours (configurable) and models mining trucks, travel, unload stations, and queueing behavior.

## Build

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Run

```powershell
build_ninja\vast_sim_cli.exe --trucks 10 --stations 3 --seed 42 --csv results.csv
```

## Output

- Command line summary for each truck and station.
- CSV file with per-truck and per-station metrics.

## Sweep + Plots

The test suite includes a sweep that runs trucks from `1..(20 * stations)` for `stations=1..10`
and writes `sweep_results.csv` + `sweep_summary.csv` in the build directory. To plot the summary:

```powershell
python scripts/plot_sweep.py --summary build_ninja/sweep_summary.csv
```
