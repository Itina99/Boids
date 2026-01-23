/**
 * BenchmarkS1.cpp
 * ===============
 * Benchmark for Strategy S1: Parallelize only Stage 1 (calculateRules + applyBoundaryForces)
 *
 * This tests the performance gain from parallelizing the computationally expensive
 * rule calculation phase, while keeping the update phase sequential.
 *
 * Stage 1 (PARALLEL): Calculate flocking rules for each boid based on neighbors
 * Stage 2 (SEQUENTIAL): Update position and velocity based on calculated forces
 *
 * Usage: ./BenchmarkS1
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>

#include "Boid.h"

const int NUM_STEPS = 1000;  // Number of simulation steps to run

int main() {
    srand(static_cast<unsigned int>(time(NULL)));  // Random seed for boid positions

    // Initialize flock with random positions
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Parallel (Only STAGE 1)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    // Main simulation loop
    for (int step = 0; step < NUM_STEPS; ++step) {
        // STAGE 1: Calculate rules (PARALLEL)
        // Each thread independently calculates forces for a subset of boids
        #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.calculateRules(flock);      // Calculate cohesion, alignment, separation
            boid.applyBoundaryForces();      // Apply screen boundary repulsion
        }

        // STAGE 2: Apply updates (SEQUENTIAL)
        // Update positions based on calculated forces
        for (Boid& boid : flock) {
            boid.updateState();
        }
    }
    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    std::cout << "Total Time (Parallel S1): " << time_elapsed << " seconds " << std::endl;
    return 0;
}