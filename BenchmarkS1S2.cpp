//
// Created by itina99 on 03/11/25.
//

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>

#include "Boid.h"

const int NUM_STEPS = 1000;
const unsigned int SEED = 42;

int main() {
    srand(SEED);
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }
    std::cout << "Benchmark Parallel (S1 + S2, 1 Region Opt)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    for (int step = 0; step < NUM_STEPS; ++step) {
        #pragma omp parallel
        {
            // STAGE 1: Calculate
            #pragma omp for
            for (Boid& boid : flock) {
                boid.calculateRules(flock);
                boid.applyBoundaryForces();
            }
            // STAGE 2: Apply
            #pragma omp for
            for (Boid& boid : flock) {
                boid.updateState();
            }
        }
    }
    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;
    std::cout << "TIME_S1S2_OPT: " << time_elapsed << " s" << std::endl;

    return 0;
}