/**
 * BenchmarkSeq.cpp
 * ================
 * Sequential Baseline Benchmark (No Parallelization)
 *
 * This is the baseline implementation with no parallel optimizations.
 * All execution is done sequentially on a single thread. This serves as
 * the reference point for calculating speedup and efficiency of parallel versions.
 *
 * Stage 1 (SEQUENTIAL): Calculate flocking rules for each boid
 * Stage 2 (SEQUENTIAL): Update position and velocity for each boid
 *
 * Usage: ./BenchmarkSeq
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>  // Included only for timer function omp_get_wtime()

#include "Boid.h"

const int NUM_STEPS = 1000;  // Number of simulation steps to run

int main() {
    srand(static_cast<unsigned int>(time(NULL)));  // Random seed for boid positions

    // Initialize flock with random positions
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Sequential (Baseline)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    // Main simulation loop - completely sequential
    for (int step = 0; step < NUM_STEPS; ++step) {

        // STAGE 1: Calculate rules for all boids
        // Each boid examines all other boids to calculate flocking forces
        for (Boid& boid : flock) {
            boid.calculateRules(flock);      // Calculate cohesion, alignment, separation
            boid.applyBoundaryForces();      // Apply screen boundary repulsion
        }

        // STAGE 2: Apply updates to all boids
        // Update positions and velocities based on calculated forces
        for (Boid& boid : flock) {
            boid.updateState();
        }
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    std::cout << "Total Time (Sequential): " << time_elapsed << " seconds" << std::endl;
    return 0;
}