import subprocess
import re
import statistics
import time
import os

# --- CONFIGURAZIONE ---
# Assicurati che il percorso sia giusto (es. ./BenchmarkDynamic o ./cmake-build-debug/BenchmarkDynamic)
EXE_PATH = "./cmake-build-debug/BenchmarkS1S2Dynamic"
NUM_RUNS = 3
THREADS = 8

REAL_BOIDS = "5000"
REAL_STEPS = "1000"
WARM_BOIDS = "500"
WARM_STEPS = "100"

def run_test(boids, steps, label):
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

    # Parsing del tempo
    match = re.search(r"TIME_DYNAMIC:\s+([0-9.]+)", result.stdout)
    if match:
        t = float(match.group(1))
        print(f" {t:.4f} s")
        return t
    return None

print(f"--- TEST DYNAMIC SCHEDULING (8 Thread - Single Region) ---")

# 1. Warm-up
print("\n>>> Fase 1: Warm-up...")
run_test(WARM_BOIDS, WARM_STEPS, "[Warmup]")
time.sleep(1)

# 2. Benchmark Reale
print(f"\n>>> Fase 2: Esecuzione {NUM_RUNS} run...")
times = []
for i in range(NUM_RUNS):
    t = run_test(REAL_BOIDS, REAL_STEPS, f"[Run {i+1}/{NUM_RUNS}]")
    if t: times.append(t)

if times:
    avg_time = statistics.mean(times)
    print(f"\n==========================================")
    print(f" RISULTATO MEDIO (Dynamic, 8 Thread): {avg_time:.4f} s")
    print(f"==========================================")
else:
    print("Nessun dato raccolto.")