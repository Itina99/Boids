/**
 * Boid.h
 * ======
 * Header file for Boid class - implements flocking behavior simulation
 *
 * Boids are bird-like entities that exhibit emergent flocking behavior
 * based on three simple rules:
 * 1. Cohesion: Steer towards the average position of neighbors
 * 2. Alignment: Match velocity with nearby boids
 * 3. Separation: Avoid crowding neighbors
 *
 * Additional features:
 * - Boundary avoidance to keep boids within screen bounds
 * - Speed limiting to ensure realistic movement
 * - Directional bias to encourage unified flock formation
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// --- Simulation Constants ---
const int SCREEN_WIDTH = 1200;       // Width of simulation area (pixels)
const int SCREEN_HEIGHT = 800;       // Height of simulation area (pixels)
inline int NUM_BOIDS = 300;          // Number of boids in simulation (can be modified)

// --- Perception Ranges ---
const float VISUAL_RANGE = 80.0f;       // Distance within which boids perceive neighbors
const float SEPARATION_RANGE = 10.0f;   // Critical distance for separation behavior
const float MAX_SPEED = 6.0f;           // Maximum allowed velocity magnitude
const float MIN_SPEED = 3.0f;           // Minimum allowed velocity magnitude

// --- Flocking Rule Weights ---
// These control the strength of each behavioral rule
const float COHESION_WEIGHT = 0.0005f;     // Weight for attraction to group center
const float SEPARATION_WEIGHT = 0.05f;     // Weight for repulsion from nearby boids
const float ALIGNMENT_WEIGHT = 0.05f;      // Weight for velocity matching

// --- Boundary Avoidance Parameters ---
const float EDGE_MARGIN = 300.0f;       // Distance from edge where avoidance starts
const float AVOID_EDGE_FORCE = 0.7f;    // Strength of boundary repulsion force

// --- Directional Bias ---
const float BIAS_WEIGHT = 0.03f;                        // Weight for directional bias
const sf::Vector2f BIAS_DIRECTION(1.0f, 0.0f);         // Bias towards right (encourages unified flock)

// --- Boid Class Declaration ---
struct Boid {
    sf::Vector2f position;      // Current position in 2D space
    sf::Vector2f velocity;      // Current velocity vector
    sf::Vector2f acceleration;  // Accumulated forces (reset each frame)
    sf::VertexArray shape;      // Visual representation (triangle)
    float angle_deg;            // Orientation angle in degrees

    // Constructor: Initialize boid at given position
    Boid(float x, float y);

    // Core simulation methods
    void calculateRules(const std::vector<Boid>& boids);  // Calculate flocking forces
    void applyBoundaryForces();                           // Apply screen boundary repulsion
    void updateState();                                   // Update position and velocity
    void draw(sf::RenderWindow& window) const;            // Render boid to window
};