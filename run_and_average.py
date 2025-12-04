import subprocess
import re
import statistics
import time
import sys

# --- CONFIGURAZIONE ---
EXE_PATH = "./cmake-build-debug/BenchmarkMain"
OUTPUT_CSV = "results_averaged.csv"
THREADS_TO_TEST = [1, 2, 4, 8, 16]
NUM_RUNS = 5

# Parametri VERI
REAL_BOIDS = "5000"
REAL_STEPS = "1000"

# Parametri WARM-UP (Veloci)
WARM_BOIDS = "500"
WARM_STEPS = "100"
# ----------------------

def parse_val(pattern, text):
    match = re.search(pattern, text)
    return float(match.group(1)) if match else None

def run_benchmark(threads, boids, steps, mode="all"):
    """
    mode: 'seq' (solo sequenziale), 'par' (solo paralleli), 'all' (tutto)
    """
    env = {"OMP_NUM_THREADS": str(threads)}
    print(f"   [CMD] Mode: {mode} | Threads: {threads}...", end="", flush=True)

    # Chiamata con il 3° argomento MODE
    result = subprocess.run(
        [EXE_PATH, boids, steps, mode],
        capture_output=True, text=True,
        env=dict(subprocess.os.environ, **env)
    )

    if result.returncode != 0:
        print(" ERRORE!")
        print(result.stderr)
        return None

    print(" Fatto.")
    return result.stdout

print(f"--- BENCHMARK OTTIMIZZATO ({NUM_RUNS} run) ---")

# 1. WARM-UP (Mode 'all' ma veloce)
print("\n>>> Fase 1: Warm-up (per caricare cache e librerie)...")
run_benchmark(8, WARM_BOIDS, WARM_STEPS, mode="all")
time.sleep(2)

# 2. CALCOLO BASELINE SEQUENZIALE (Una tantum!)
print(f"\n>>> Fase 2: Calcolo Baseline Sequenziale ({NUM_RUNS} run)...")
seq_times = []
for i in range(NUM_RUNS):
    # Usiamo 1 thread, ma per il sequenziale puro non importa
    out = run_benchmark(1, REAL_BOIDS, REAL_STEPS, mode="seq")
    val = parse_val(r"TIME_SEQ:\s+([0-9.]+)", out)
    if val: seq_times.append(val)

if not seq_times:
    print("Errore critico: Nessun tempo sequenziale rilevato.")
    sys.exit(1)

AVG_SEQ_TIME = statistics.mean(seq_times)
print(f"   >>> TEMPO SEQUENZIALE MEDIO (FISSATO): {AVG_SEQ_TIME:.4f} s")


# 3. CALCOLO PARALLELO (Per ogni thread)
final_results = []

print(f"\n>>> Fase 3: Calcolo Strategie Parallele ({NUM_RUNS} run per config)...")
for t in THREADS_TO_TEST:
    print(f"\n--- Configurazione: {t} Thread ---")

    # Accumulatori solo per le parti parallele
    times = {'Time_S1': [], 'Time_S2': [], 'Time_S1S2': [], 'Time_S1S2_Opt': []}

    for i in range(NUM_RUNS):
        out = run_benchmark(t, REAL_BOIDS, REAL_STEPS, mode="par")
        if out:
            times['Time_S1'].append(parse_val(r"TIME_S1:\s+([0-9.]+)", out))
            times['Time_S2'].append(parse_val(r"TIME_S2:\s+([0-9.]+)", out))
            times['Time_S1S2'].append(parse_val(r"TIME_S1S2:\s+([0-9.]+)", out))
            times['Time_S1S2_Opt'].append(parse_val(r"TIME_S1S2_OPT:\s+([0-9.]+)", out))

    # Calcola medie
    row = {'Threads': t, 'Time_Seq': AVG_SEQ_TIME} # Usa il sequenziale calcolato prima
    for key, val_list in times.items():
        # Filtra i None
        valid_vals = [v for v in val_list if v is not None]
        if valid_vals:
            row[key] = statistics.mean(valid_vals)
        else:
            row[key] = 0.0

    final_results.append(row)
    print(f"   Media Parallela {t} Thread completata.")
    print(f"   Risultati medi: S1={row['Time_S1']:.4f}s, S2={row['Time_S2']:.4f}s, S1S2={row['Time_S1S2']:.4f}s, S1S2_Opt={row['Time_S1S2_Opt']:.4f}s")
# 4. SALVATAGGIO
print(f"\n>>> Salvataggio in {OUTPUT_CSV}...")
with open(OUTPUT_CSV, "w") as f:
    f.write("Threads,Time_Seq,Time_S1,Time_S2,Time_S1S2,Time_S1S2_Opt\n")
    for r in final_results:
        line = f"{r['Threads']},{r['Time_Seq']:.4f},{r['Time_S1']:.4f},{r['Time_S2']:.4f},{r['Time_S1S2']:.4f},{r['Time_S1S2_Opt']:.4f}\n"
        f.write(line)

print("\n--- DONE! ---")