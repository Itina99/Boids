#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// --- Simulation Constants ---
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int NUM_BOIDS = 5000;

// --- MODIFICATO: Parametri Comportamentali ---
const float VISUAL_RANGE = 80.0f;     // Raggio visivo ampio
const float SEPARATION_RANGE = 10.0f; // Raggio di separazione PICCOLO (era 35)

// Limiti di Velocità
const float MAX_SPEED = 6.0f;
const float MIN_SPEED = 3.0f;

// Fattori delle Regole
const float COHESION_WEIGHT = 0.0005f; // (invariato)
const float SEPARATION_WEIGHT = 0.05f;   // (invariato)
const float ALIGNMENT_WEIGHT = 0.05f;    // (invariato)

// --- MODIFICATO: Bordi ---
const float EDGE_MARGIN = 300.0f;
const float AVOID_EDGE_FORCE = 0.7f; // Molto più alta (era 0.05)

// Bias
const float BIAS_WEIGHT = 0.03f; // Un fattore di forza (prova a bilanciarlo)
const sf::Vector2f BIAS_DIRECTION(1.0f, 0.0f);

// --- Boid Struct Declaration ---
struct Boid {
    // ... (membri invariati) ...
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::VertexArray shape;
    float angle_deg;

    // Constructor
    Boid(float x, float y);

    // Methods
    void calculateRules(const std::vector<Boid>& boids);
    void applyBoundaryForces();
    void updateState();
    void draw(sf::RenderWindow& window);
};