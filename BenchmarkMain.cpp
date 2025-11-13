#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>
#include <iomanip>

#include "Boid.h"

const int NUM_STEPS = 1000;
const unsigned int SEED = 42;

std::vector<Boid> createFlock() {
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }
    return flock;
}

int main() {
    std::cout << "Starting Banchmark Comparison" << std::endl;
    std::cout << "Boids: " << NUM_BOIDS << ", Steps: " << NUM_STEPS << std::endl;
    #pragma omp parallel
    {
        #pragma omp master
        {
            std::cout << "Numero di Thread OpenMP: " << omp_get_num_threads() << std::endl;
        }
    }
    std::cout << "---" << std::endl;

    double time_seq = 0.0, time_par_s1 = 0.0, time_par_s2 = 0.0, time_par_s1s2_ineff = 0.0, time_par_s1s2_opt = 0.0;

    // --- 1. BENCHMARK SEQUENZIALE (BASELINE) ---
    {
        std::cout << "Execution: Sequential (Baseline)..." << std::endl;
        srand(SEED);
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        for (int step = 0; step < NUM_STEPS; ++step) {
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_seq = omp_get_wtime() - start;
        std::cout << "TIME_SEQ: " << time_seq << " s" << std::endl << "---" << std::endl;
    }

    // --- 2. BENCHMARK PARALLEL (ONLY STAGE 1) ---
    {
        std::cout << "Execution: Parallel (Only S1)..." << std::endl;
        srand(SEED);
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        for (int step = 0; step < NUM_STEPS; ++step) {
            #pragma omp parallel for
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_par_s1 = omp_get_wtime() - start;
        std::cout << "TIME_S1: " << time_par_s1 << " s" << std::endl << "---" << std::endl;
    }

    // --- 3. BENCHMARK PARALLEL (ONLY STAGE 2) ---
    {
        std::cout << "Execution: Parallel (Only S2)..." << std::endl;
        srand(SEED);
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        for (int step = 0; step < NUM_STEPS; ++step) {
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }
            #pragma omp parallel for
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_par_s2 = omp_get_wtime() - start;
        std::cout << "TIME_S2: " << time_par_s2 << " s" << std::endl << "---" << std::endl;
    }

    // --- 4. BENCHMARK PARALLEL (S1 + S2, 2 Regions) ---
    {
        std::cout << "Execution: Parallel (S1 + S2, 2 Regions)..." << std::endl;
        srand(SEED);
        std::vector<Boid> flock = createFlock();
        double start = omp_get_wtime();
        for (int step = 0; step < NUM_STEPS; ++step) {
            #pragma omp parallel for
            for (Boid& boid : flock) { boid.calculateRules(flock); boid.applyBoundaryForces(); }

            #pragma omp parallel for
            for (Boid& boid : flock) { boid.updateState(); }
        }
        time_par_s1s2_ineff = omp_get_wtime() - start;
        std::cout << "TIME_S1S2: " << time_par_s1s2_ineff << " s" << std::endl << "---" << std::endl;
    }

    // --- 5. BENCHMARK PARALLEL (S1 + S2, OPTIMAL, 1 Region) ---
    {
        std::cout << "Execution: Parallel (S1 + S2, 1 Region Opt.)..." << std::endl;
        srand(SEED);
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
        time_par_s1s2_opt = omp_get_wtime() - start;
        std::cout << "TIME_S1S2_OPT: " << time_par_s1s2_opt << " s" << std::endl << "---" << std::endl;
    }

    // --- 6. RESULTS ---
    std::cout << "========= RECAP BENCHMARK =========" << std::endl;
    std::cout << std::fixed << std::setprecision(4);

    std::cout << "Test                        | Time (s)  | Speedup" << std::endl;
    std::cout << "-------------------------------------------------------------------" << std::endl;

    std::cout << "Sequential                 | " << std::setw(10) << time_seq << " | "
              << std::setw(8) << (time_seq / time_seq) << "x" << std::endl;

    std::cout << "Parallel (Only S1)         | " << std::setw(10) << time_par_s1 << " | "
              << std::setw(8) << (time_seq / time_par_s1) << "x" << std::endl;

    std::cout << "Parallel (Solo S2)         | " << std::setw(10) << time_par_s2 << " | "
              << std::setw(8) << (time_seq / time_par_s2) << "x" << std::endl;

    std::cout << "Parallel (S1+S2)     | " << std::setw(10) << time_par_s1s2_ineff << " | "
              << std::setw(8) << (time_seq / time_par_s1s2_ineff) << "x" << std::endl;

    std::cout << "Parallel (S1+S2 Optimal)  | " << std::setw(10) << time_par_s1s2_opt << " | "
              << std::setw(8) << (time_seq / time_par_s1s2_opt) << "x" << std::endl;
    return 0;
}