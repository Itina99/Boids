# 🚀 Boids Simulation with OpenMP Optimization

This project implements a Boids flocking simulation and provides a deep-dive analysis of parallel optimization strategies using OpenMP. The goal is to parallelize an $O(N^2)$ brute-force algorithm and study its scalability, its limitations (Amdahl's Law), and the impact of hardware architecture (Cache, Hyper-Threading).

![Boids Flocking Demo](Boids.gif)

---

## 🎯 About The Project

The core algorithm is split into two phases:
1.  **STAGE 1 (Calculation):** An $O(N^2)$ loop where each boid calculates the flocking rules (Separation, Alignment, Cohesion) by reading the position of all other boids.
2.  **STAGE 2 (Update):** An $O(N)$ loop where each boid updates its own position.

This report analyzes the performance gained by parallelizing S1, S2, or both, and compares different scheduling strategies (`static` vs. `dynamic`).

---

## 🛠️ Requirements (Built With)

To compile and run this project, you will need:

* **C++:** A modern compiler (e.g., g++ or Clang) with C++17 support.
* **OpenMP:** The parallel computing library (usually included with the compiler).
* **CMake:** (version 3.10+) For building the project.
* **SFML:** (version 2.5+) For graphical visualization.
* **Python 3:** For data analysis.
* **Python Libraries:** `pandas` and `matplotlib`.

You can install the required Python libraries using pip:
```bash
pip install pandas matplotlib
```

---

## 🚀 How to Run (Getting Started)

### 1. Compilation

The project uses CMake. To compile all executables:
```bash
# 1. Clone the repository
git clone https://github.com/Itina99/Boids.git
cd Boids

# 2. Create a build directory
mkdir build
cd build

# 3. Configure with CMake
cmake ..

# 4. Compile
make
```

After compiling, the build/ (or cmake-build-debug/) directory will contain several executables.

### 2. Run the Visualization

To see the simulation in action:
```bash

# Runs the sequential version
./BoidsSeq

# Runs the parallel version (uses all available cores)
./BoidsPar
```

### 3. Run the Benchmarks (Recommended)

This is the basic experiment. The run_benchmarks.sh script launches the BenchmarkMain executable (which tests all 5 strategies) across a range of thread counts (1, 2, 4, 8, 16) and saves the results to a .csv file.
```bash

# Make the script executable (only need to do this once)
chmod +x run_benchmarks.sh

# Run the full scalability test (this may take a long time!)
./run_benchmarks.sh
```

This will create a new file named results.csv with all the data.

### 4. Run Specific Tests (Manual)

You can also launch individual benchmark executables. Remember to set the thread count using OMP_NUM_THREADS.
```bash

# Runs the full benchmark (5 strategies) but only with 8 threads
OMP_NUM_THREADS=8 ./BenchmarkMain

# Runs the separate test for 'dynamic' scheduling with 8 threads
OMP_NUM_THREADS=8 ./BenchmarkS1S2_Dynamic
```
### 5. Run with average
The main experiment with warm up and time average can be executed with the python script run_average.py. This can take a long time but is the more complete one
```bash
python run_average.py
```
---



## 📈 Analyzing the Results

To generate the Speedup/Efficiency tables and plots:

1. Run the benchmark (Step 3) to generate a .csv file (e.g., results_...csv).

2. Open the plots.py file in a text editor.

3. Modify the CSV_FILE variable to match the name of your results file:
```python

# Edit this line in plots.py
CSV_FILE = "results.csv" 
```

4. Run the Python script:
```bash
python plots.py
```

This will print the data tables to your terminal and open windows displaying the performance graphs.
