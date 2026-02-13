FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -G Ninja
RUN cmake --build build
RUN ctest --test-dir build --output-on-failure

# Optional: plotting support
RUN pip3 install --no-cache-dir matplotlib

CMD ["./build/vast_sim_cli", "--trucks", "10", "--stations", "3", "--seed", "42", "--csv", "results.csv"]
