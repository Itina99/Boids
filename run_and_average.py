"""
run_and_average.py
==================
Optimized Benchmark Automation Script with Statistical Averaging

This script automates the execution of BenchmarkMain and calculates averaged results
across multiple runs to reduce variance and provide reliable performance metrics.

Key Optimizations:
1. Warm-up phase to load cache and libraries before real measurements
2. Sequential baseline computed once and reused (mode='seq')
3. Parallel strategies tested separately (mode='par') for efficiency
4. Multiple runs per configuration with statistical averaging
5. Outputs averaged results to CSV for plotting

Benchmark Strategy:
- Phase 1: Warm-up with small problem size
- Phase 2: Compute sequential baseline (N runs, fixed for all thread counts)
- Phase 3: Test parallel strategies for each thread configuration (N runs each)

Usage: python3 run_and_average.py
Output: results_averaged.csv
"""

import subprocess
import re
import statistics
import time
import sys

# --- CONFIGURATION ---
EXE_PATH = "./cmake-build-debug/BenchmarkMain"  # Path to benchmark executable
OUTPUT_CSV = "results_averaged.csv"             # Output CSV file
THREADS_TO_TEST = [1, 2, 4, 8, 16]              # Thread counts to benchmark
NUM_RUNS = 5                                     # Repetitions per configuration

# Real benchmark parameters (computationally intensive)
REAL_BOIDS = "5000"
REAL_STEPS = "1000"

# Warm-up parameters (fast execution for cache loading)
WARM_BOIDS = "500"
WARM_STEPS = "100"
# ----------------------

def parse_val(pattern, text):
    """
    Extract a float value from text using regex pattern

    Args:
        pattern: Regex pattern with one capture group for the number
        text: Text to search

    Returns:
        Float value if found, None otherwise
    """
    match = re.search(pattern, text)
    return float(match.group(1)) if match else None

def run_benchmark(threads, boids, steps, mode="all"):
    """
    Execute benchmark with specified parameters

    Args:
        threads: Number of OpenMP threads
        boids: Number of boids in simulation
        steps: Number of simulation steps
        mode: Execution mode - 'seq' (sequential only), 'par' (parallel only), 'all' (both)

    Returns:
        stdout of benchmark execution, or None on error
    """
    env = {"OMP_NUM_THREADS": str(threads)}
    print(f"   [CMD] Mode: {mode} | Threads: {threads}...", end="", flush=True)

    # Call BenchmarkMain with mode as 3rd argument
    result = subprocess.run(
        [EXE_PATH, boids, steps, mode],
        capture_output=True, text=True,
        env=dict(subprocess.os.environ, **env)
    )

    if result.returncode != 0:
        print(" ERROR!")
        print(result.stderr)
        return None

    print(" Done.")
    return result.stdout

print(f"--- OPTIMIZED BENCHMARK ({NUM_RUNS} runs) ---")

# 1. WARM-UP PHASE
# Run a fast benchmark to load libraries, cache, and stabilize CPU frequency
print("\n>>> Phase 1: Warm-up (to load cache and libraries)...")
run_benchmark(8, WARM_BOIDS, WARM_STEPS, mode="all")
time.sleep(2)  # Short pause to let system stabilize

# 2. COMPUTE SEQUENTIAL BASELINE (Once, then reused for all thread counts)
# This is the reference time for calculating speedup
print(f"\n>>> Phase 2: Computing Sequential Baseline ({NUM_RUNS} runs)...")
seq_times = []
for i in range(NUM_RUNS):
    # Use 1 thread, but for pure sequential it doesn't matter
    out = run_benchmark(1, REAL_BOIDS, REAL_STEPS, mode="seq")
    val = parse_val(r"TIME_SEQ:\s+([0-9.]+)", out)
    if val: seq_times.append(val)

if not seq_times:
    print("Critical error: No sequential time detected.")
    sys.exit(1)

AVG_SEQ_TIME = statistics.mean(seq_times)
print(f"   >>> AVERAGE SEQUENTIAL TIME (FIXED): {AVG_SEQ_TIME:.4f} s")


# 3. COMPUTE PARALLEL STRATEGIES (For each thread configuration)
final_results = []

print(f"\n>>> Phase 3: Computing Parallel Strategies ({NUM_RUNS} runs per config)...")
for t in THREADS_TO_TEST:
    print(f"\n--- Configuration: {t} Threads ---")

    # Accumulators for parallel strategies only
    times = {'Time_S1': [], 'Time_S2': [], 'Time_S1S2': [], 'Time_S1S2_Opt': []}

    # Run multiple times for statistical reliability
    for i in range(NUM_RUNS):
        out = run_benchmark(t, REAL_BOIDS, REAL_STEPS, mode="par")
        if out:
            # Extract times for all parallel strategies
            times['Time_S1'].append(parse_val(r"TIME_S1:\s+([0-9.]+)", out))
            times['Time_S2'].append(parse_val(r"TIME_S2:\s+([0-9.]+)", out))
            times['Time_S1S2'].append(parse_val(r"TIME_S1S2:\s+([0-9.]+)", out))
            times['Time_S1S2_Opt'].append(parse_val(r"TIME_S1S2_OPT:\s+([0-9.]+)", out))

    # Calculate averages for this thread configuration
    row = {'Threads': t, 'Time_Seq': AVG_SEQ_TIME}  # Use computed sequential baseline
    for key, val_list in times.items():
        # Filter out None values
        valid_vals = [v for v in val_list if v is not None]
        if valid_vals:
            row[key] = statistics.mean(valid_vals)
        else:
            row[key] = 0.0

    final_results.append(row)
    print(f"   Parallel Average for {t} Threads completed.")
    print(f"   Average Results: S1={row['Time_S1']:.4f}s, S2={row['Time_S2']:.4f}s, S1S2={row['Time_S1S2']:.4f}s, S1S2_Opt={row['Time_S1S2_Opt']:.4f}s")

# 4. SAVE RESULTS TO CSV
print(f"\n>>> Saving to {OUTPUT_CSV}...")
with open(OUTPUT_CSV, "w") as f:
    f.write("Threads,Time_Seq,Time_S1,Time_S2,Time_S1S2,Time_S1S2_Opt\n")
    for r in final_results:
        line = f"{r['Threads']},{r['Time_Seq']:.4f},{r['Time_S1']:.4f},{r['Time_S2']:.4f},{r['Time_S1S2']:.4f},{r['Time_S1S2_Opt']:.4f}\n"
        f.write(line)

print("\n--- DONE! ---")