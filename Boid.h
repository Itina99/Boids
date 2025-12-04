#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// --- Simulation Constants ---
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
inline int NUM_BOIDS = 300;

const float VISUAL_RANGE = 80.0f;
const float SEPARATION_RANGE = 10.0f;
const float MAX_SPEED = 6.0f;
const float MIN_SPEED = 3.0f;

// --- Rules constants ---
const float COHESION_WEIGHT = 0.0005f;
const float SEPARATION_WEIGHT = 0.05f;
const float ALIGNMENT_WEIGHT = 0.05f;

const float EDGE_MARGIN = 300.0f;
const float AVOID_EDGE_FORCE = 0.7f;

const float BIAS_WEIGHT = 0.03f;
const sf::Vector2f BIAS_DIRECTION(1.0f, 0.0f);// Bias towards right, useful to make boids group to a single flock

// --- Boid Struct Declaration ---
struct Boid {
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
    void draw(sf::RenderWindow& window) const;
};