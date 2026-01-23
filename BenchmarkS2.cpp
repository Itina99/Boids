/**
 * BenchmarkS2.cpp
 * ===============
 * Benchmark for Strategy S2: Parallelize only Stage 2 (updateState)
 *
 * This tests the performance gain from parallelizing the state update phase,
 * while keeping the rule calculation phase sequential.
 *
 * Stage 1 (SEQUENTIAL): Calculate flocking rules for each boid based on neighbors
 * Stage 2 (PARALLEL): Update position and velocity based on calculated forces
 *
 * Note: Stage 2 is typically less computationally intensive than Stage 1,
 * so this strategy may show less speedup than S1.
 *
 * Usage: ./BenchmarkS2
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

    std::cout << "Benchmark Parallel (Only STAGE 2)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    // Main simulation loop
    for (int step = 0; step < NUM_STEPS; ++step) {

        // STAGE 1: Calculate rules (SEQUENTIAL)
        // Calculate forces sequentially for all boids
        for (Boid& boid : flock) {
            boid.calculateRules(flock);      // Calculate cohesion, alignment, separation
            boid.applyBoundaryForces();      // Apply screen boundary repulsion
        }

        // STAGE 2: Apply updates (PARALLEL)
        // Each thread independently updates position/velocity for a subset of boids
        #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.updateState();
        }
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    std::cout << "Total time (Parallel S2): " << time_elapsed << " seconds" << std::endl;
    return 0;
}