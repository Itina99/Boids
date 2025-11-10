#!/bin/bash

# --- Configurazione ---
# 1. Assicurati che il nome dell'eseguibile sia corretto
#    (quello che hai compilato da BenchmarkS1S2.cpp)
EXE_NAME="cmake-build-debug/BenchmarkS1S2"

# 2. Creiamo un file CSV separato (con timestamp)
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUT_FILE="results_s1s2_only_${TIMESTAMP}.csv"
# ---

THREADS_TO_TEST="1 2 4 8 16"

if [ ! -f "$EXE_NAME" ]; then
    echo "Errore: Eseguibile '$EXE_NAME' non trovato!"
    echo "Per favore, compila prima il tuo progetto (BenchmarkS1S2.cpp)."
    exit 1
fi

echo "Avvio benchmark di scalabilità (Solo S1S2 Ottimale)..."
echo "I risultati verranno salvati in: $OUT_FILE"

# Stampa l'intestazione del file CSV
echo "Threads,Time_S1S2_Opt" > $OUT_FILE

# Esegui i test
for t in $THREADS_TO_TEST
do
    echo "Running test (S1S2) con $t thread(s)..."

    # Esegui l'eseguibile S1S2
    OUTPUT=$(OMP_NUM_THREADS=$t ./$EXE_NAME)

    # Estrai l'UNICO tag che ci interessa
    T_OPT=$(echo "$OUTPUT" | grep "TIME_S1S2_OPT:" | awk '{print $2}')

    # Scrivi la riga di dati nel file CSV
    echo "$t,$T_OPT" >> $OUT_FILE
done

echo "--- Fatto! ---"
echo "Benchmark completato. Risultati in '$OUT_FILE'"
echo ""
cat $OUT_FILE