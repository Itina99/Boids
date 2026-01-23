/**
 * mainSeq.cpp
 * ===========
 * Sequential Boids Visualization (Real-time Interactive)
 *
 * This program provides a real-time visual simulation of the Boids flocking algorithm
 * using SFML graphics library. All computation is done sequentially (single-threaded).
 *
 * Features:
 * - Real-time rendering at 60 FPS
 * - Interactive window (can be closed by user)
 * - Dark blue background with white triangular boids
 *
 * Usage: ./BoidsSeq
 * Controls: Close window to exit
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>

#include "Boid.h"  // Essential: imports Boid class and simulation constants

int main() {
    // Initialize random number generator with current time
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create window with dimensions from Boid.h constants
    sf::Vector2u windowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::RenderWindow window(sf::VideoMode(windowSize), "Boids Sequential");
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

        // --- Physics Update (Sequential) ---

        // STAGE 1: Calculate flocking forces for all boids
        // Each boid examines all neighbors to compute cohesion, alignment, separation
        for (Boid& boid : flock) {
            boid.calculateRules(flock);      // Calculate flocking rules
            boid.applyBoundaryForces();      // Keep boids within screen bounds
        }

        // STAGE 2: Update positions and velocities
        // Apply the calculated forces to update each boid's state
        for (Boid& boid : flock) {
            boid.updateState();
        }

        // --- Rendering ---
        window.clear(sf::Color(50, 50, 80));  // Clear with dark blue background

        // Draw all boids as oriented triangles
        for (Boid& boid : flock) {
            boid.draw(window);
        }

        window.display();  // Present rendered frame to screen
    }
    return 0;
}