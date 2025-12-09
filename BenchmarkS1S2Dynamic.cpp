#include <iostream>
#include <vector>
#include <cstdlib> // Per atoi
#include <omp.h>
#include "Boid.h"  // Assicurati che questo file esista

// Valori di default
int NUM_STEPS = 1000;
const unsigned int SEED = 42;

int main(int argc, char* argv[]) {
    // 1. Parsing argomenti da Python (se presenti)
    if (argc >= 3) {
        NUM_BOIDS = std::atoi(argv[1]);
        NUM_STEPS = std::atoi(argv[2]);
    }

    srand(SEED);

    // Creazione Flock
    std::vector<Boid> flock;
    flock.reserve(NUM_BOIDS); // Piccola ottimizzazione memory
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    std::cout << "Benchmark Dynamic Optimized (Single Region)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    for (int step = 0; step < NUM_STEPS; ++step) {
        // --- STRUTTURA OTTIMALE (Single Region) ---
        // Creiamo il team di thread UNA volta sola
#pragma omp parallel
        {
            // Usiamo schedule(dynamic)
#pragma omp for schedule(dynamic)
            for (int i = 0; i < NUM_BOIDS; ++i) {
                flock[i].calculateRules(flock);
                flock[i].applyBoundaryForces();
            }

            // Usiamo schedule(dynamic)
#pragma omp for schedule(dynamic)
            for (int i = 0; i < NUM_BOIDS; ++i) {
                flock[i].updateState();
            }
        }
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    // Output formattato per il parser
    std::cout << "TIME_DYNAMIC: " << time_elapsed << " s" << std::endl;

    return 0;
}