#!/bin/bash

##############################################################################
# run_benchmarks.sh
# =================
# Basic Benchmark Automation Script (Single-Run, Shell-Based)
#
# This is a simple shell script that runs BenchmarkMain once for each thread
# configuration and saves results to CSV. Unlike run_and_average.py, this
# performs only a single run per configuration (no statistical averaging).
#
# Features:
# - Tests multiple thread configurations (1, 2, 4, 8, 16)
# - Extracts timing results using grep and awk
# - Saves results to CSV for analysis
#
# Usage: ./run_benchmarks.sh
# Output: results.csv
#
# Note: For more reliable results with statistical averaging, use
#       run_and_average.py instead.
##############################################################################

# Configuration
EXE_NAME="cmake-build-debug/BenchmarkMain"  # Path to benchmark executable
OUT_FILE="results.csv"                      # Output CSV file
THREADS_TO_TEST="1 2 4 8 16"                # Thread counts to test

# Check if executable exists
if [ ! -f "$EXE_NAME" ]; then
    echo "Error: Executable '$EXE_NAME' non found!"
    echo "Please, compile your project first."
    exit 1
fi

echo "Starting benchmark... (This may take a while)"
echo "Results will be saved in: $OUT_FILE"

# Create CSV header
echo "Threads,Time_Seq,Time_S1,Time_S2,Time_S1S2,Time_S1S2_Opt" > "$OUT_FILE"

# Run benchmark for each thread configuration
for t in $THREADS_TO_TEST
do
    echo "Running test with $t thread(s)..."

    # Execute benchmark with OMP_NUM_THREADS environment variable
    OUTPUT=$(OMP_NUM_THREADS=$t ./$EXE_NAME)

    # Extract timing values using grep and awk
    T_SEQ=$(echo "$OUTPUT" | grep "TIME_SEQ:" | awk '{print $2}')
    T_S1=$(echo "$OUTPUT" | grep "TIME_S1:" | awk '{print $2}')
    T_S2=$(echo "$OUTPUT" | grep "TIME_S2:" | awk '{print $2}')
    T_S1S2=$(echo "$OUTPUT" | grep "TIME_S1S2:" | awk '{print $2}')
    T_OPT=$(echo "$OUTPUT" | grep "TIME_S1S2_OPT:" | awk '{print $2}')

    # Append results to CSV
    echo "$t,$T_SEQ,$T_S1,$T_S2,$T_S1S2,$T_OPT" >> "$OUT_FILE"
done

echo "--- Done! ---"
echo "Benchmark completed. Results are in '$OUT_FILE'"
echo ""
cat "$OUT_FILE"  # Display results to console
