#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <omp.h> // Incluso OpenMP

#include "Boid.h" // Importa la classe e le costanti

int main() {
    // Setup (identico al sequenziale)
    srand(static_cast<unsigned int>(time(NULL)));
    sf::Vector2u windowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::RenderWindow window(sf::VideoMode(windowSize), "Boids Parallel (Test Rollback)");
    window.setFramerateLimit(60);

    // Creazione flock (identico al sequenziale)
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    // Main Game Loop
    while (window.isOpen()) {

        // --- Event Handling ---
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- Update Logic (PARALLEL - VERSIONE 1) ---

        // STAGE 1: Calculate
        // Usiamo il for-each loop originale
        #pragma omp parallel for
        for (Boid& boid : flock) {
            boid.calculateRules(flock);
            boid.applyBoundaryForces();
        }

        // STAGE 2: Apply
        for (Boid& boid : flock) {
            boid.updateState();
        }

        // --- Rendering (COMPLETO) ---

        window.clear(sf::Color(50, 50, 80)); // <-- ASSICURIAMOCI CHE SIA QUI

        // Disegno del margine
        sf::RectangleShape marginBox;
        float innerWidth = SCREEN_WIDTH - (2 * EDGE_MARGIN);
        float innerHeight = SCREEN_HEIGHT - (2 * EDGE_MARGIN);
        marginBox.setSize(sf::Vector2f(innerWidth, innerHeight));
        marginBox.setPosition(sf::Vector2f(EDGE_MARGIN, EDGE_MARGIN));
        marginBox.setFillColor(sf::Color::Transparent);
        marginBox.setOutlineThickness(1.0f);
        marginBox.setOutlineColor(sf::Color(100, 100, 150, 100));
        window.draw(marginBox);

        // Disegno dei boid
        for (Boid& boid : flock) {
            boid.draw(window);
        }

        window.display();
    }

    return 0;
}