import pandas as pd
import matplotlib.pyplot as plt
import os
import sys


CSV_FILE = "results.csv"

if not os.path.exists(CSV_FILE):
    print(f"Error: File '{CSV_FILE}' not found.")
    sys.exit(1)
try:
    df = pd.read_csv(CSV_FILE)
except Exception as e:
    print(f"Error uploading CSV: {e}")
    sys.exit(1)

# --- 1. Speedup & Efficiency ---
try:
    baseline_time = df.loc[df['Threads'] == 1, 'Time_Seq'].iloc[0]
except IndexError:
    print("Error: Impossible to find 'Threads == 1' o 'Time_Seq' inside the CSV.")
    print("Check if CSV is empty or corrupted.")
    sys.exit(1)

print(f"--- Benchmark based on Sequential Time: {baseline_time:.4f}s ---")

strategies = ['Time_S1', 'Time_S2', 'Time_S1S2', 'Time_S1S2_Opt']
speedup_data = {'Threads': df['Threads']}
efficiency_data = {'Threads': df['Threads']}

for strat in strategies:
    speedup_col_name = f"Speedup_{strat.replace('Time_', '')}"
    efficiency_col_name = f"Efficiency_{strat.replace('Time_', '')}"

    speedup_data[speedup_col_name] = baseline_time / df[strat]
    efficiency_data[efficiency_col_name] = (baseline_time / df[strat]) / df['Threads']

df_speedup = pd.DataFrame(speedup_data)
df_efficiency = pd.DataFrame(efficiency_data)

print("\n========= SPEEDUP  =========")
print(df_speedup.to_string(index=False, float_format="%.2fx"))

print("\n========= EFFICIENCY (E(N)) =========")
print(df_efficiency.to_string(index=False, float_format="%.2f%%"))

print("\n Plots generation...")
plt.style.use('ggplot')

def setup_plot(title, ylabel, xlabel='Number of Threads'):
    plt.title(title, fontsize=15, fontweight='bold')
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.legend(loc='best', fontsize=10)
    plt.xticks(df['Threads'])

# --- Speedup ---
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Threads'], 'k--', label='Ideal Speedup y=x)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1'], 'o-', label='Speedup (Solo S1)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2'], 's-', label='Speedup (S1+S2)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2_Opt'], '^-', label='Speedup (S1+S2 Opt.)')
setup_plot('Speedup Scalability (Acceleration)', 'Speedup (T_base / T_N)')
plt.ylim(bottom=0)

# --- Efficiency ---
plt.figure(figsize=(10, 7))
plt.axhline(1.0, color='k', linestyle='--', label='Ideal Efficiency (100%)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1'], 'o-', label='Efficiency (only S1)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2'], 's-', label='Efficiency (S1+S2)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2_Opt'], '^-', label='Efficiency (S1+S2 Opt.)')
setup_plot('Parallelization Efficiency', 'Efficiency (Speedup / N)')
plt.ylim(0, 1.2)

# --- Absolute Times ---
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Time_Seq'], 'o-', label='Sequential (Baseline)')
plt.plot(df['Threads'], df['Time_S1'], 's-', label='Only S1')
plt.plot(df['Threads'], df['Time_S2'], 'd-', label='Only S2')
plt.plot(df['Threads'], df['Time_S1S2'], '^-', label='S1+S2')
plt.plot(df['Threads'], df['Time_S1S2_Opt'], 'x-', label='S1+S2 Opt.')
setup_plot('Absolute Execution Times', 'Time (seconds)')
plt.yscale('log')
plt.yticks([100, 200, 300, 400, 500, 700, 1000],
           labels=['100s', '200s', '300s', '400s', '500s', '700s', '1000s'])

plt.show()
print("\nEnd Analysis.")