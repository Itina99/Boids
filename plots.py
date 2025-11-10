import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

# --- Configurazione ---
# 1. Assicurati che il nome sia corretto
CSV_FILE = "results.csv"
# ---

# Controlla se il file esiste
if not os.path.exists(CSV_FILE):
    print(f"Errore: File '{CSV_FILE}' non trovato.")
    sys.exit(1)

# Carica i dati
try:
    df = pd.read_csv(CSV_FILE)
except Exception as e:
    print(f"Errore nel caricamento del CSV: {e}")
    sys.exit(1)

# --- 1. Calcolo Metriche (Speedup & Efficienza) ---
try:
    # Usa il tempo sequenziale a 1 thread come baseline fisso
    baseline_time = df.loc[df['Threads'] == 1, 'Time_Seq'].iloc[0]
except IndexError:
    print("Errore: Impossibile trovare 'Threads == 1' o 'Time_Seq' nel CSV.")
    print("Controlla che il CSV non sia corrotto o vuoto.")
    sys.exit(1)

print(f"--- Benchmark Basato su Tempo Sequenziale: {baseline_time:.4f}s ---")

# 2. USA I NOMI ESATTI DELLE COLONNE DEL TUO CSV
#    (Time_S1S2 invece di Time_S1S2_Ineff)
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

# --- 2. Stampa le Tabelle dei Dati ---
print("\n========= TABELLA SPEEDUP (ACCELERAZIONE) =========")
print(df_speedup.to_string(index=False, float_format="%.2fx"))

print("\n========= TABELLA EFFICIENZA (E(N)) =========")
print(df_efficiency.to_string(index=False, float_format="%.2f%%"))

# --- 3. Creazione Grafici ---
print("\nGenerazione grafici in corso...")
plt.style.use('ggplot')

def setup_plot(title, ylabel, xlabel='Numero di Thread'):
    plt.title(title, fontsize=15, fontweight='bold')
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.legend(loc='best', fontsize=10)
    plt.xticks(df['Threads'])

# --- Grafico 1: Speedup ---
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Threads'], 'k--', label='Speedup Ideale (y=x)')
# Aggiornati i label per corrispondere ai dati
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1'], 'o-', label='Speedup (Solo S1)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2'], 's-', label='Speedup (S1+S2 Ineff.)')
plt.plot(df_speedup['Threads'], df_speedup['Speedup_S1S2_Opt'], '^-', label='Speedup (S1+S2 Opt.)')
setup_plot('Scalabilità Speedup (Accelerazione)', 'Speedup (T_base / T_N)')
plt.ylim(bottom=0)

# --- Grafico 2: Efficienza ---
plt.figure(figsize=(10, 7))
plt.axhline(1.0, color='k', linestyle='--', label='Efficienza Ideale (100%)')
# Aggiornati i label
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1'], 'o-', label='Efficienza (Solo S1)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2'], 's-', label='Efficienza (S1+S2 Ineff.)')
plt.plot(df_efficiency['Threads'], df_efficiency['Efficiency_S1S2_Opt'], '^-', label='Efficienza (S1+S2 Opt.)')
setup_plot('Efficienza di Parallelizzazione', 'Efficienza (Speedup / N)')
plt.ylim(0, 1.2)

# --- Grafico 3: Tempi Assoluti ---
plt.figure(figsize=(10, 7))
plt.plot(df['Threads'], df['Time_Seq'], 'o-', label='Sequenziale (Baseline)')
plt.plot(df['Threads'], df['Time_S1'], 's-', label='Solo S1')
plt.plot(df['Threads'], df['Time_S2'], 'd-', label='Solo S2')
# Aggiornati i label
plt.plot(df['Threads'], df['Time_S1S2'], '^-', label='S1+S2 Ineff.')
plt.plot(df['Threads'], df['Time_S1S2_Opt'], 'x-', label='S1+S2 Opt.')
setup_plot('Tempi di Ecuzezione Assoluti', 'Tempo (secondi)')
plt.yscale('log')
plt.yticks([100, 200, 300, 400, 500, 700, 1000],
           labels=['100s', '200s', '300s', '400s', '500s', '700s', '1000s'])

plt.show()
print("\nAnalisi completata.")