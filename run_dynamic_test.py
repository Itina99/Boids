"""
run_dynamic_test.py
===================
Dynamic Scheduling Benchmark Test Script

This script tests the performance of the dynamic scheduling strategy (BenchmarkS1S2Dynamic)
with a fixed thread count (8 threads by default). It compares how dynamic scheduling
performs relative to the standard static scheduling.

Dynamic vs Static Scheduling:
- Static: Each thread gets a fixed, contiguous chunk of iterations (default in OpenMP)
- Dynamic: Threads request work chunks on-demand at runtime (better load balancing)

Trade-offs:
- Dynamic has higher overhead due to runtime work distribution
- Dynamic can be faster when work per iteration varies significantly
- For uniform workloads (like Boids), static is often better

Test Strategy:
1. Warm-up phase to stabilize system
2. Multiple runs with statistical averaging
3. Fixed thread count (8) for focused comparison

Usage: python3 run_dynamic_test.py
Output: Console output with averaged time
"""

import subprocess
import re
import statistics
import time
import os

# --- CONFIGURATION ---
# Path to dynamic scheduling benchmark executable
EXE_PATH = "./cmake-build-debug/BenchmarkS1S2Dynamic"
NUM_RUNS = 3        # Number of runs for averaging
THREADS = 8         # Fixed thread count for this test

# Real benchmark parameters
REAL_BOIDS = "5000"
REAL_STEPS = "1000"

# Warm-up parameters (fast execution)
WARM_BOIDS = "500"
WARM_STEPS = "100"

def run_test(boids, steps, label):
    """
    Execute the dynamic scheduling benchmark

    Args:
        boids: Number of boids in simulation
        steps: Number of simulation steps
        label: Descriptive label for console output

    Returns:
        Execution time in seconds, or None on error
    """
    # Set OpenMP thread count via environment variable
    env = dict(os.environ)
    env["OMP_NUM_THREADS"] = str(THREADS)

    print(f"   Esecuzione {label}...", end="", flush=True)
    result = subprocess.run(
        [EXE_PATH, boids, steps],
        capture_output=True, text=True, env=env
    )

    if result.returncode != 0:
        print(" ERRORE!")
        print(result.stderr)
        return None

    # Parse execution time from output
    match = re.search(r"TIME_DYNAMIC:\s+([0-9.]+)", result.stdout)
    if match:
        t = float(match.group(1))
        print(f" {t:.4f} s")
        return t
    return None

print(f"--- TEST DYNAMIC SCHEDULING (8 Thread - Single Region) ---")

# 1. Warm-up Phase
# Run a quick test to load libraries and cache
print("\n>>> Fase 1: Warm-up...")
run_test(WARM_BOIDS, WARM_STEPS, "[Warmup]")
time.sleep(1)  # Brief pause for system stabilization

# 2. Real Benchmark with Multiple Runs
# Run multiple times and average to reduce noise
print(f"\n>>> Fase 2: Esecuzione {NUM_RUNS} run...")
times = []
for i in range(NUM_RUNS):
    t = run_test(REAL_BOIDS, REAL_STEPS, f"[Run {i+1}/{NUM_RUNS}]")
    if t: times.append(t)

# Calculate and display average result
if times:
    avg_time = statistics.mean(times)
    print(f"\n==========================================")
    print(f" RISULTATO MEDIO (Dynamic, 8 Thread): {avg_time:.4f} s")
    print(f"==========================================")
else:
    print("Nessun dato raccolto.")