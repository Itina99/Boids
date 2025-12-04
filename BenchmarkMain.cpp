#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <iomanip>
#include "Boid.h" // Assumiamo ci sia il tuo header

// Valori di default
int NUM_STEPS = 1000;

std::vector<Boid> createFlock() {
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }
    return flock;
}

int main(int argc, char* argv[]) {
    // Arg 1: Boids, Arg 2: Steps, Arg 3: Mode ("seq", "par", "all")
    std::string mode = "all";

    if (argc >= 3) {
        NUM_BOIDS = std::atoi(argv[1]);
        NUM_STEPS = std::atoi(argv[2]);
    }
    if (argc >= 4) {
        mode = argv[3];
    }

    std::cout << "Avvio Benchmark [Mode: " << mode << "]..." << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;

    double time_seq = 0.0;

    // --- 1. SEQUENZIALE (Eseguito solo se mode è 'seq' o 'all') ---
    if (mode == "seq" || mode == "all") {
        std::cout << "Esecuzione: Sequenziale..." << std::endl;
        srand(42);
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        for (int step = 0; step < NUM_STEPS; ++step) {
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_seq = omp_get_wtime() - start;
        std::cout << "TIME_SEQ: " << time_seq << " s" << std::endl;
    }

    // --- PARALLELI (Eseguiti solo se mode è 'par' o 'all') ---
    if (mode == "par" || mode == "all") {

        // S1
        {
            std::cout << "Esecuzione: Parallelo (S1)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S1: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // S2
        {
            std::cout << "Esecuzione: Parallelo (S2)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S2: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // S1+S2
        {
            std::cout << "Esecuzione: Parallelo (S1+S2)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                #pragma omp parallel for
                for (Boid& boid : flock) { boid.updateState(); }
            }
            std::cout << "TIME_S1S2: " << (omp_get_wtime() - start) << " s" << std::endl;
        }

        // S1+S2 Opt
        {
            std::cout << "Esecuzione: Parallelo (S1+S2 Opt)..." << std::endl;
            srand(42);
            std::vector<Boid> flock = createFlock();
            double start = omp_get_wtime();
            for (int step = 0; step < NUM_STEPS; ++step) {
                #pragma omp parallel
                {
                    #pragma omp for
                    for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
                    #pragma omp for
                    for (Boid& boid : flock) { boid.updateState(); }
                }
            }
            std::cout << "TIME_S1S2_OPT: " << (omp_get_wtime() - start) << " s" << std::endl;
        }
    }

    return 0;
}