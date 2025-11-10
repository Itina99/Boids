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
    // Il tuo setup è invariato
    srand(SEED);
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    // Aggiornata la stampa
    std::cout << "Benchmark Parallelo (S1 + S2, 1 Regione Ottimale)..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double start_time = omp_get_wtime();

    for (int step = 0; step < NUM_STEPS; ++step) {

        // --- VERSIONE OTTIMALE CON FOR-EACH ---

        // Apri la regione parallela UNA SOLA VOLTA
#pragma omp parallel
        {
            // STAGE 1: Calculate
            // "for" assegna il lavoro ai thread GIA' ATTIVI
#pragma omp for
            for (Boid& boid : flock) { // <-- Questa sintassi è corretta
                boid.calculateRules(flock);
                boid.applyBoundaryForces();
            }

#pragma omp for
            for (Boid& boid : flock) { // <-- E anche questa
                boid.updateState();
            }

        } // Fine regione parallela
        // --- FINE MODIFICA ---
    }

    double end_time = omp_get_wtime();
    double time_elapsed = end_time - start_time;

    // Aggiunto tag per lo script bash
    std::cout << "TIME_S1S2_OPT: " << time_elapsed << " s" << std::endl;

    return 0;
}