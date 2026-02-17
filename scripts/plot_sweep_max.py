# Plotting utilities for the max-load sweep output.
import argparse
import csv
from pathlib import Path
import sys


# Load the sweep summary into arrays for plotting.
def load_summary(path: Path):
    stations = []
    trucks_at_max = []
    max_loads = []

    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            stations.append(int(row["stations"]))
            trucks_at_max.append(int(row["trucks_at_max"]))
            max_loads.append(int(float(row["max_total_loads"])))

    return stations, trucks_at_max, max_loads


# Render a line plot and write it to disk.
def plot(x_values, y_values, out_path: Path, title: str, ylabel: str):
    try:
        import matplotlib.pyplot as plt
    except Exception as exc:
        print("matplotlib is required to plot results:", exc)
        print("Install with: pip install matplotlib")
        sys.exit(1)

    plt.figure(figsize=(7, 4))
    plt.plot(x_values, y_values, marker="o")
    plt.title(title)
    plt.xlabel("Number of stations")
    plt.ylabel(ylabel)
    plt.grid(True, linestyle="--", alpha=0.4)
    plt.tight_layout()
    plt.savefig(out_path)
    plt.close()


# Parse CLI args and generate both plots.
def main():
    parser = argparse.ArgumentParser(description="Plot sweep max-load results.")
    parser.add_argument(
        "--summary",
        default="sweep_max_summary.csv",
        help="Path to sweep_max_summary.csv (default: sweep_max_summary.csv)",
    )
    parser.add_argument(
        "--out",
        default="sweep_max_trucks.png",
        help="Output PNG for trucks-at-max plot",
    )
    parser.add_argument(
        "--out-loads",
        default="sweep_max_loads.png",
        help="Output PNG for max-loads plot",
    )
    args = parser.parse_args()

    summary_path = Path(args.summary)
    if not summary_path.exists():
        print(f"Summary file not found: {summary_path}")
        sys.exit(1)

    stations, trucks_at_max, max_loads = load_summary(summary_path)

    plot(
        stations,
        trucks_at_max,
        Path(args.out),
        "Trucks Needed to Reach Max Mined Material",
        "Number of trucks",
    )
    plot(
        stations,
        max_loads,
        Path(args.out_loads),
        "Max Mined Material vs. Stations",
        "Total loads started",
    )

    print(f"Wrote {args.out}")
    print(f"Wrote {args.out_loads}")


if __name__ == "__main__":
    main()
