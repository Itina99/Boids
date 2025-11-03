#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>

#include "Boid.h" // <-- Questo è FONDAMENTALE. Senza questo, main non sa cosa siano SCREEN_WIDTH e SCREEN_HEIGHT

int main() {
    // Initialize random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // --- Correzione qui ---
    // Inizializza le dimensioni usando un sf::Vector2u
    sf::Vector2u windowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Passa il Vector2u a VideoMode e crea la finestra
    sf::RenderWindow window(sf::VideoMode(windowSize), "Boids Sequential");
    // --- Fine Correzione ---

    window.setFramerateLimit(60);

    // Create the vector of Boids
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

        // --- Update Logic (SEQUENTIAL) ---

        for (Boid& boid : flock) {
            boid.calculateRules(flock);
            boid.applyBoundaryForces(); // <-- ASSICURATI CHE QUESTA RIGA CI SIA!
        }

        // STAGE 2: APPLY
        // (Questa parte era stata rimossa perché ora updateState fa tutto)
        // La logica è stata spostata, ora STAGE 1 e 2 sono diversi

        // --- NUOVO FLUSSO CORRETTO ---
        // STAGE 1: Calculate all adjustments
        for (Boid& boid : flock) {
            boid.calculateRules(flock);     // Calcola le 3 regole
            boid.applyBoundaryForces(); // Calcola la forza dei bordi
        }

        // STAGE 2: Apply adjustments and update
        for (Boid& boid : flock) {
            boid.updateState(); // Applica le forze, limita la velocità, muove
        }

        // --- Rendering ---
        window.clear(sf::Color(50, 50, 80)); // Dark blue background

        // --- DISEGNO DEL MARGINE (Come linea di contorno) ---
        sf::RectangleShape marginBox;

        // Calcola la dimensione dell'area "sicura" interna
        float innerWidth = SCREEN_WIDTH - (2 * EDGE_MARGIN);
        float innerHeight = SCREEN_HEIGHT - (2 * EDGE_MARGIN);

        marginBox.setSize(sf::Vector2f(innerWidth, innerHeight));
        marginBox.setPosition(sf::Vector2f(EDGE_MARGIN, EDGE_MARGIN));
        marginBox.setFillColor(sf::Color::Transparent); // Riempimento trasparente
        marginBox.setOutlineThickness(1.0f); // Spessore della linea (sottile)

        // Un colore grigio-blu, semi-trasparente per non disturbare
        marginBox.setOutlineColor(sf::Color(100, 100, 150, 100));

        window.draw(marginBox);
        // --- FINE DISEGNO MARGINE ---

        for (Boid& boid : flock) {
            boid.draw(window);
        }

        window.display();
    }
    return 0;
}