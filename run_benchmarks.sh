#!/bin/bash

# Assicurati che il nome dell'eseguibile sia corretto
EXE_NAME="cmake-build-debug/BenchmarkMain"

# Crea un nome file unico usando data e ora per non sovrascrivere
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUT_FILE="results_${TIMESTAMP}.csv"

# Lista di thread da testare
THREADS_TO_TEST="1 2 4 8 16"

if [ ! -f "$EXE_NAME" ]; then
    echo "Errore: Eseguibile '$EXE_NAME' non trovato!"
    echo "Per favore, compila prima il tuo progetto."
    exit 1
fi

echo "Avvio benchmark... (Questo potrebbe richiedere molto tempo)"
echo "I risultati verranno salvati in: $OUT_FILE"

# Stampa l'intestazione del file CSV
echo "Threads,Time_Seq,Time_S1,Time_S2,Time_S1S2_Ineff,Time_S1S2_Opt" > $OUT_FILE

# Esegui i test
for t in $THREADS_TO_TEST
do
    echo "Running test con $t thread(s)..."

    OUTPUT=$(OMP_NUM_THREADS=$t ./$EXE_NAME)

    # Estrai i dati usando i TAG UNICI
    T_SEQ=$(echo "$OUTPUT" | grep "TIME_SEQ:" | awk '{print $2}')
    T_S1=$(echo "$OUTPUT" | grep "TIME_S1:" | awk '{print $2}')
    T_S2=$(echo "$OUTPUT" | grep "TIME_S2:" | awk '{print $2}')
    T_INEFF=$(echo "$OUTPUT" | grep "TIME_S1S2_INEFF:" | awk '{print $2}')
    T_OPT=$(echo "$OUTPUT" | grep "TIME_S1S2_OPT:" | awk '{print $2}')

    # Scrivi la riga di dati nel file CSV
    echo "$t,$T_SEQ,$T_S1,$T_S2,$T_INEFF,$T_OPT" >> $OUT_FILE
done

echo "--- Fatto! ---"
echo "Benchmark completato. Risultati in '$OUT_FILE'"
echo ""
cat $OUT_FILE