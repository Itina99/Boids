/**
 * BenchmarkS1S2.cpp
 * =================
 * Benchmark for Strategy S1+S2 Optimized: Both stages in a single parallel region
 *
 * This is the most efficient parallel strategy. It parallelizes both Stage 1 and Stage 2
 * within a SINGLE parallel region, which significantly reduces thread management overhead
 * compared to creating separate parallel regions for each stage.
 *
 * Stage 1 (PARALLEL): Calculate flocking rules within parallel region
 * Stage 2 (PARALLEL): Update state within same parallel region
 *
 * Key advantage: Thread team is created once per step, not twice. The implicit barrier
 * between #pragma omp for directives ensures synchronization between stages.
 *
 * Usage: ./BenchmarkS1S2
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>

#include "Boid.h"

const int NUM_STEPS = 1000;      // Number of simulation steps to run
const unsigned int SEED = 42;    // Fixed seed for reproducible results

int main() {
    srand(SEED);  // Consistent initialization for fair comparison

    // Initialize flock with random positions
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Parallel (S1 + S2, 1 Region Opt)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    // Main simulation loop
    for (int step = 0; step < NUM_STEPS; ++step) {
        // Single parallel region for both stages (reduces overhead)
        #pragma omp parallel
        {
            // STAGE 1: Calculate rules (work-sharing directive)
            #pragma omp for
            for (Boid& boid : flock) {
                boid.calculateRules(flock);      // Calculate cohesion, alignment, separation
                boid.applyBoundaryForces();      // Apply screen boundary repulsion
            }
            // Implicit barrier here: all threads wait until Stage 1 is complete

            // STAGE 2: Apply updates (work-sharing directive)
            #pragma omp for
            for (Boid& boid : flock) {
                boid.updateState();
            }
        }
        // Implicit barrier at end of parallel region
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;
    std::cout << "TIME_S1S2_OPT: " << time_elapsed << " s" << std::endl;

    return 0;
}