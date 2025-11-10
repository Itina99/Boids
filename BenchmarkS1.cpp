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

int main() {
    srand(static_cast<unsigned int>(time(NULL)));

    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Parallelo (Solo STAGE 1)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    for (int step = 0; step < NUM_STEPS; ++step) {

        // STAGE 1: Calculate (Parallelo)
    #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.calculateRules(flock);
            boid.applyBoundaryForces();
        }

        // STAGE 2: Apply (Sequenziale)
        for (Boid& boid : flock) {
            boid.updateState();
        }
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    std::cout << "Tempo totale (Parallelo S1): " << time_elapsed << " secondi" << std::endl;
    return 0;
}