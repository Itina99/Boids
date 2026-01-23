/**
 * BenchmarkS1S2Dynamic.cpp
 * ========================
 * Benchmark for Strategy S1+S2 with Dynamic Scheduling
 *
 * Similar to the optimized S1+S2 strategy, but uses dynamic scheduling instead of static.
 * Dynamic scheduling assigns work chunks to threads at runtime, which can improve load
 * balancing when iterations have varying computational costs.
 *
 * Scheduling Comparison:
 * - Static (default): Each thread gets a fixed contiguous chunk of iterations
 * - Dynamic: Threads request new chunks as they finish, enabling better load balancing
 *
 * Trade-off: Dynamic scheduling has higher overhead but can be faster if work is unbalanced.
 *
 * Usage: ./BenchmarkS1S2Dynamic [num_boids] [num_steps]
 */

#include <iostream>
#include <vector>
#include <cstdlib>  // For atoi
#include <omp.h>
#include "Boid.h"

// Default simulation parameters
int NUM_STEPS = 1000;
const unsigned int SEED = 42;  // Fixed seed for reproducibility

int main(int argc, char* argv[]) {
    // 1. Parse command-line arguments from Python (if provided)
    if (argc >= 3) {
        NUM_BOIDS = std::atoi(argv[1]);  // Number of boids
        NUM_STEPS = std::atoi(argv[2]);  // Number of simulation steps
    }

    srand(SEED);  // Consistent initialization for fair comparison

    // Create flock with random initial positions
    std::vector<Boid> flock;
    flock.reserve(NUM_BOIDS);  // Memory optimization: pre-allocate capacity
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Dynamic Optimized (Single Region)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    // Main simulation loop
    for (int step = 0; step < NUM_STEPS; ++step) {
        // --- OPTIMAL STRUCTURE (Single Parallel Region) ---
        // Create thread team once per step to minimize overhead
#pragma omp parallel
        {
            // STAGE 1: Calculate rules with dynamic scheduling
            // Dynamic scheduling: threads get work chunks on-demand for better load balancing
#pragma omp for schedule(dynamic)
            for (int i = 0; i < NUM_BOIDS; ++i) {
                flock[i].calculateRules(flock);      // Calculate cohesion, alignment, separation
                flock[i].applyBoundaryForces();      // Apply screen boundary repulsion
            }
            // Implicit barrier: all threads wait until Stage 1 completes

            // STAGE 2: Update state with dynamic scheduling
#pragma omp for schedule(dynamic)
            for (int i = 0; i < NUM_BOIDS; ++i) {
                flock[i].updateState();
            }
        }
        // Implicit barrier at end of parallel region
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    // Formatted output for parser
    std::cout << "TIME_DYNAMIC: " << time_elapsed << " s" << std::endl;

    return 0;
}