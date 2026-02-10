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
buildast_sim_cli.exe --trucks 10 --stations 3 --seed 42 --csv results.csv
```

## Output

- Command line summary for each truck and station.
- CSV file with per-truck and per-station metrics.
