//
// Created by itina99 on 03/11/25.
//
#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h> // Incluso solo per la funzione timer omp_get_wtime()

#include "Boid.h"

const int NUM_STEPS = 1000;

int main() {
    // 1. Setup
    srand(static_cast<unsigned int>(time(NULL)));

    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Sequenziale (Baseline)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    // 2. Misurazione del Tempo
    double start_time = omp_get_wtime();

    // 3. Loop di Calcolo (Tutto sequenziale)
    for (int step = 0; step < NUM_STEPS; ++step) {

        // STAGE 1: Calculate (Sequenziale)
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

    // 4. Stampa del Risultato
    std::cout << "Tempo totale (Sequenziale): " << time_elapsed << " secondi" << std::endl;
    return 0;
}