#!/bin/bash

EXE_NAME="cmake-build-debug/BenchmarkMain"

OUT_FILE="results.csv"

THREADS_TO_TEST="1 2 4 8 16"

if [ ! -f "$EXE_NAME" ]; then
    echo "Error: Executable '$EXE_NAME' non found!"
    echo "Please, compile your project first."
    exit 1
fi

echo "Starting benchmark... (This may take a while)"
echo "Results will be saved in: $OUT_FILE"

echo "Threads,Time_Seq,Time_S1,Time_S2,Time_S1S2,Time_S1S2_Opt" > "$OUT_FILE"


for t in $THREADS_TO_TEST
do
    echo "Running test with $t thread(s)..."

    OUTPUT=$(OMP_NUM_THREADS=$t ./$EXE_NAME)
    T_SEQ=$(echo "$OUTPUT" | grep "TIME_SEQ:" | awk '{print $2}')
    T_S1=$(echo "$OUTPUT" | grep "TIME_S1:" | awk '{print $2}')
    T_S2=$(echo "$OUTPUT" | grep "TIME_S2:" | awk '{print $2}')
    T_S1S2=$(echo "$OUTPUT" | grep "TIME_S1S2:" | awk '{print $2}')
    T_OPT=$(echo "$OUTPUT" | grep "TIME_S1S2_OPT:" | awk '{print $2}')

    echo "$t,$T_SEQ,$T_S1,$T_S2,$T_S1S2,$T_OPT" >> "$OUT_FILE"
done

echo "--- Done! ---"
echo "Benchmark completed. Results are in '$OUT_FILE'"
echo ""
cat "$OUT_FILE"