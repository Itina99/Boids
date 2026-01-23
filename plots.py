"""
Boids Benchmark Visualization Script
=====================================
This script reads benchmark results from a CSV file and generates performance plots
including speedup, efficiency, and absolute execution times for different parallelization strategies.

Parallelization Strategies:
- S1: Only Stage 1 (calculateRules) parallelized
- S2: Only Stage 2 (updateState) parallelized
- S1S2: Both stages parallelized with separate regions
- S1S2_Opt: Both stages parallelized within a single parallel region (optimized)
"""

import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

# Configuration: CSV file containing averaged benchmark results
CSV_FILE = "results_averaged.csv"

# Check if the CSV file exists
if not os.path.exists(CSV_FILE):
    print(f"Error: File '{CSV_FILE}' not found.")
    sys.exit(1)

# Load the CSV file into a pandas DataFrame
try:
    df = pd.read_csv(CSV_FILE)
except Exception as e:
    print(f"Error uploading CSV: {e}")
    sys.exit(1)

# --- 1. Calculate Speedup & Efficiency Metrics ---
# Extract the baseline sequential time (from 1 thread configuration)
try:
    baseline_time = df.loc[df['Threads'] == 1, 'Time_Seq'].iloc[0]
except IndexError:
    print("Error: Impossible to find 'Threads == 1' o 'Time_Seq' inside the CSV.")
    print("Check if CSV is empty or corrupted.")
    sys.exit(1)

print(f"--- Benchmark based on Sequential Time: {baseline_time:.4f}s ---")

# List of parallelization strategies to analyze
strategies = ['Time_S1', 'Time_S2', 'Time_S1S2', 'Time_S1S2_Opt']
speedup_data = {'Threads': df['Threads']}
efficiency_data = {'Threads': df['Threads']}

# Calculate speedup and efficiency for each strategy
# Speedup = T_baseline / T_parallel
# Efficiency = Speedup / N_threads
for strat in strategies:
    speedup_col_name = f"Speedup_{strat.replace('Time_', '')}"
    efficiency_col_name = f"Efficiency_{strat.replace('Time_', '')}"

    speedup_data[speedup_col_name] = baseline_time / df[strat]
    efficiency_data[efficiency_col_name] = (baseline_time / df[strat]) / df['Threads']

# Create DataFrames for speedup and efficiency results
df_speedup = pd.DataFrame(speedup_data)
df_efficiency = pd.DataFrame(efficiency_data)

# Print speedup results to console
print("\n========= SPEEDUP  =========")
print(df_speedup.to_string(index=False, float_format="%.2fx"))

# Print efficiency results to console
print("\n========= EFFICIENCY (E(N)) =========")
print(df_efficiency.to_string(index=False, float_format="%.2f%%"))

print("\n Plots generation...")
plt.style.use('ggplot')

def setup_plot(title, ylabel, xlabel='Number of Threads'):
    """
    Configure common plot settings for all graphs

    Args:
        title: Plot title
        ylabel: Y-axis label
        xlabel: X-axis label (default: 'Number of Threads')
    """
    plt.title(title, fontsize=15, fontweight='bold')
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.legend(loc='best', fontsize=10)
    plt.xticks(df['Threads'])

# --- Plot 1: Speedup Graph ---
# Shows how much faster the parallel versions are compared to baseline
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Threads'], 'k--', label='Ideal Speedup y=x)')  # Ideal linear speedup
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1'], 'o-', label='Speedup (Solo S1)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2'], 's-', label='Speedup (S1+S2)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2_Opt'], '^-', label='Speedup (S1+S2 Opt.)')
setup_plot('Speedup Scalability (Acceleration)', 'Speedup (T_base / T_N)')
plt.ylim(bottom=0)

# --- Plot 2: Efficiency Graph ---
# Shows how efficiently threads are utilized (1.0 = 100% efficient)
plt.figure(figsize=(10, 7))
plt.axhline(1.0, color='k', linestyle='--', label='Ideal Efficiency (100%)')  # Ideal efficiency line
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1'], 'o-', label='Efficiency (only S1)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2'], 's-', label='Efficiency (S1+S2)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2_Opt'], '^-', label='Efficiency (S1+S2 Opt.)')
setup_plot('Parallelization Efficiency', 'Efficiency (Speedup / N)')
plt.ylim(0, 1.2)

# --- Plot 3: Absolute Execution Times ---
# Shows raw execution times for all strategies (log scale for better visibility)
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Time_Seq'], 'o-', label='Sequential (Baseline)')
plt.plot(df['Threads'], df['Time_S1'], 's-', label='Only S1')
plt.plot(df['Threads'], df['Time_S2'], 'd-', label='Only S2')
plt.plot(df['Threads'], df['Time_S1S2'], '^-', label='S1+S2')
plt.plot(df['Threads'], df['Time_S1S2_Opt'], 'x-', label='S1+S2 Opt.')
setup_plot('Absolute Execution Times', 'Time (seconds)')
plt.yscale('log')  # Use logarithmic scale for better visualization
plt.yticks([100, 200, 300, 400, 500, 700, 1000],
           labels=['100s', '200s', '300s', '400s', '500s', '700s', '1000s'])

# Display all plots
plt.show()
print("\nEnd Analysis.")