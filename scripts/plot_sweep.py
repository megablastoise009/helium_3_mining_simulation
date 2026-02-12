import argparse
import csv
from pathlib import Path
import sys


def load_summary(path: Path):
    stations = []
    best_trucks_downtime = []
    best_trucks_wait = []

    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            stations.append(int(row["stations"]))
            best_trucks_downtime.append(int(row["best_trucks_min_downtime"]))
            best_trucks_wait.append(int(row["best_trucks_min_wait"]))

    return stations, best_trucks_downtime, best_trucks_wait


def plot(stations, trucks, out_path: Path, title: str, ylabel: str):
    try:
        import matplotlib.pyplot as plt
    except Exception as exc:
        print("matplotlib is required to plot results:", exc)
        print("Install with: pip install matplotlib")
        sys.exit(1)

    plt.figure(figsize=(7, 4))
    plt.plot(stations, trucks, marker="o")
    plt.title(title)
    plt.xlabel("Number of stations")
    plt.ylabel(ylabel)
    plt.grid(True, linestyle="--", alpha=0.4)
    plt.tight_layout()
    plt.savefig(out_path)
    plt.close()


def main():
    parser = argparse.ArgumentParser(description="Plot sweep summary results.")
    parser.add_argument(
        "--summary",
        default="sweep_summary.csv",
        help="Path to sweep_summary.csv (default: sweep_summary.csv)",
    )
    parser.add_argument(
        "--out-downtime",
        default="sweep_station_downtime.png",
        help="Output PNG for station downtime plot",
    )
    parser.add_argument(
        "--out-wait",
        default="sweep_truck_wait.png",
        help="Output PNG for truck wait plot",
    )
    args = parser.parse_args()

    summary_path = Path(args.summary)
    if not summary_path.exists():
        print(f"Summary file not found: {summary_path}")
        sys.exit(1)

    stations, best_trucks_downtime, best_trucks_wait = load_summary(summary_path)

    plot(
        stations,
        best_trucks_downtime,
        Path(args.out_downtime),
        "Trucks That Minimize Station Downtime",
        "Number of trucks",
    )
    plot(
        stations,
        best_trucks_wait,
        Path(args.out_wait),
        "Trucks That Minimize Truck Queue Time",
        "Number of trucks",
    )

    print(f"Wrote {args.out_downtime}")
    print(f"Wrote {args.out_wait}")


if __name__ == "__main__":
    main()
