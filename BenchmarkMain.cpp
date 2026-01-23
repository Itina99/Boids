/**
 * BenchmarkMain.cpp
 * =================
 * Unified benchmark program that tests all parallelization strategies for Boids simulation.
 *
 * This program can run in three modes:
 * - "seq": Only sequential baseline
 * - "par": Only parallel strategies (S1, S2, S1S2, S1S2_Opt)
 * - "all": Both sequential and parallel (default)
 *
 * Parallelization Strategies:
 * - Sequential: No parallelization (baseline)
 * - S1: Parallelizes Stage 1 (calculateRules + applyBoundaryForces)
 * - S2: Parallelizes Stage 2 (updateState)
 * - S1S2: Parallelizes both stages with separate parallel regions
 * - S1S2_Opt: Parallelizes both stages within a single parallel region (reduces overhead)
 *
 * Usage: ./BenchmarkMain [num_boids] [num_steps] [mode]
 *   num_boids: Number of boids in the simulation
 *   num_steps: Number of simulation steps
 *   mode: "seq", "par", or "all"
 */

#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <iomanip>
#include "Boid.h" // Includes Boid class and simulation constants

// Default simulation parameters
int NUM_STEPS = 1000;

/**
 * Creates a flock of boids with random initial positions
 * Uses fixed seed (42) for reproducibility in main()
 *
 * @return Vector of Boid objects with random positions
 */
std::vector<Boid> createFlock() {
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }
    return flock;
}

int main(int argc, char* argv[]) {
    // Command-line arguments: Arg 1: Boids, Arg 2: Steps, Arg 3: Mode ("seq", "par", "all")
    std::string mode = "all"; // Default: run both sequential and parallel

    // Parse command-line arguments
    if (argc >= 3) {
        NUM_BOIDS = std::atoi(argv[1]);  // Number of boids
        NUM_STEPS = std::atoi(argv[2]);  // Number of simulation steps
    }
    if (argc >= 4) {
        mode = argv[3];  // Execution mode: "seq", "par", or "all"
    }

    std::cout << "Avvio Benchmark [Mode: " << mode << "]..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double time_seq = 0.0;

    // --- 1. SEQUENTIAL (Executed only if mode is 'seq' or 'all') ---
    if (mode == "seq" || mode == "all") {
        std::cout << "Esecuzione: Sequenziale..." << std::endl;
        srand(42);  // Fixed seed for reproducibility
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        // Main simulation loop: two-stage update (calculate rules, then update state)
        for (int step = 0; step < NUM_STEPS; ++step) {
            // Stage 1: Calculate rules and apply boundary forces
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
            // Stage 2: Update positions and velocities
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_seq = omp_get_wtime() - start;
        std::cout << "TIME_SEQ: " << time_seq << " s" << std::endl;
    }

    // --- PARALLEL (Executed only if mode is 'par' or 'all') ---
    if (mode == "par" || mode == "all") {

        // --- 2. STRATEGY S1: Parallelize only Stage 1 (calculateRules + boundaries) ---
        {
            std::cout << "Esecuzione: Parallelo (S1)..." << std::endl;
            srand(42);  // Same seed for fair comparison
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                // Parallel stage 1: calculate rules and apply boundary forces
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                // Sequential stage 2: update state
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S1: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // --- 3. STRATEGY S2: Parallelize only Stage 2 (updateState) ---
        {
            std::cout << "Esecuzione: Parallelo (S2)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                // Sequential stage 1: calculate rules and apply boundary forces
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                // Parallel stage 2: update state
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S2: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // --- 4. STRATEGY S1+S2: Parallelize both stages (separate parallel regions) ---
        {
            std::cout << "Esecuzione: Parallelo (S1+S2)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                // Parallel stage 1: creates thread team, then destroys it
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                // Parallel stage 2: creates new thread team (overhead!)
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S1S2: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // --- 5. STRATEGY S1+S2 Opt: Both stages in single parallel region (reduced overhead) ---
        {
            std::cout << "Esecuzione: Parallelo (S1+S2 Opt)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                // Single parallel region: create thread team once per step
                #pragma omp parallel
                {
                    // Stage 1: work sharing with 'for' directive
                    #pragma omp for
                    for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                    // Implicit barrier here ensures all boids finish stage 1
                    // Stage 2: work sharing with 'for' directive (same thread team)
                    #pragma omp for
                    for (Boid& boid : flock) { boid.updateState(); }
                }
            }
            std::cout << "TIME_S1S2_OPT: " << (omp_get_wtime() - start) << " s" << std::endl;
        }
    }

    return 0;
}