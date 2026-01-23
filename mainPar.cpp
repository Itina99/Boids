/**
 * mainPar.cpp
 * ===========
 * Parallel Boids Visualization (Real-time Interactive with OpenMP)
 *
 * This program provides a real-time visual simulation of the Boids flocking algorithm
 * using SFML graphics library with OpenMP parallelization for physics computation.
 *
 * Parallelization Strategy:
 * - Stage 1 (calculateRules + boundaries): Parallelized with #pragma omp parallel for
 * - Stage 2 (updateState): Parallelized with #pragma omp parallel for
 * - Rendering: Sequential (SFML is not thread-safe)
 *
 * Features:
 * - Real-time rendering at 60 FPS
 * - Multi-threaded physics computation
 * - Interactive window (can be closed by user)
 *
 * Usage: ./BoidsPar
 * Controls: Close window to exit
 * Environment: Set OMP_NUM_THREADS to control thread count
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h>  // OpenMP for parallel computation

#include "Boid.h"  // Import Boid class and constants

int main() {
    // Initialize random number generator with current time
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create window with dimensions from Boid.h constants
    sf::Vector2u windowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::RenderWindow window(sf::VideoMode(windowSize), "Boids Parallel");
    window.setFramerateLimit(60);  // Cap at 60 frames per second

    // Initialize flock with random starting positions
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    // Main Game Loop - runs until window is closed
    while (window.isOpen()) {

        // --- Event Handling ---
        // Check for user input (window close, etc.)
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- Physics Update (Parallel) ---

        // STAGE 1: Calculate flocking forces (PARALLEL)
        // Each thread processes a subset of boids independently
        #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.calculateRules(flock);      // Calculate flocking rules (cohesion, alignment, separation)
            boid.applyBoundaryForces();      // Keep boids within screen bounds
        }

        // STAGE 2: Update positions and velocities (PARALLEL)
        // Apply the calculated forces to update each boid's state
        #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.updateState();
        }

        // --- Rendering (Sequential - SFML is not thread-safe) ---
        window.clear(sf::Color(50, 50, 80));  // Clear with dark blue background

        // Draw all boids as oriented triangles
        for (Boid& boid : flock) {
            boid.draw(window);
        }

        window.display();  // Present rendered frame to screen
    }

    return 0;
}