#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>

#include "Boid.h" // <-- Questo è FONDAMENTALE. Senza questo, main non sa cosa siano SCREEN_WIDTH e SCREEN_HEIGHT

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    sf::Vector2u windowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    sf::RenderWindow window(sf::VideoMode(windowSize), "Boids Sequential");
    window.setFramerateLimit(60);
    std::vector<Boid> flock;
    for (int i = 0; i < NUM_BOIDS; ++i) {
        flock.emplace_back(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
    }

    // Main Game Loop
    while (window.isOpen()) {

        // Event Handling
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // STAGE 1: CALCULATE
        for (Boid& boid : flock) {
            boid.calculateRules(flock);
            boid.applyBoundaryForces();
        }

        // STAGE 2: Apply adjustments and update
        for (Boid& boid : flock) {
            boid.updateState();
        }

        // --- Rendering ---
        window.clear(sf::Color(50, 50, 80)); // Dark blue background

        sf::RectangleShape marginBox;

        float innerWidth = SCREEN_WIDTH - (2 * EDGE_MARGIN);
        float innerHeight = SCREEN_HEIGHT - (2 * EDGE_MARGIN);

        marginBox.setSize(sf::Vector2f(innerWidth, innerHeight));
        marginBox.setPosition(sf::Vector2f(EDGE_MARGIN, EDGE_MARGIN));
        marginBox.setFillColor(sf::Color::Transparent);
        marginBox.setOutlineThickness(1.0f);

        marginBox.setOutlineColor(sf::Color(100, 100, 150, 100));

        window.draw(marginBox);

        for (Boid& boid : flock) {
            boid.draw(window);
        }

        window.display();
    }
    return 0;
}