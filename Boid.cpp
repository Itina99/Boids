/**
 * Boid.cpp
 * ========
 * Implementation of Boid class - flocking behavior simulation
 *
 * This file implements the core flocking algorithm based on Craig Reynolds' Boids model.
 * Each boid follows three simple rules that produce emergent flocking behavior:
 * 1. Cohesion: Move towards the center of mass of nearby boids
 * 2. Alignment: Match velocity with nearby boids
 * 3. Separation: Avoid colliding with nearby boids
 */

#include "Boid.h"
#include <cmath>

// --- Vector Math Helper Functions ---
namespace {
    /**
     * Calculate the Euclidean magnitude (length) of a 2D vector
     * @param v Input vector
     * @return Length of the vector
     */
    float magnitude(sf::Vector2f v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
}


// --- Boid Class Method Definitions ---

/**
 * Constructor: Initialize a boid at the given position with random velocity
 * @param x Initial x-coordinate
 * @param y Initial y-coordinate
 */
Boid::Boid(float x, float y) {
    position = sf::Vector2f(x, y);

    // Initialize velocity with random direction
    const float angle = rand() % 360 * 3.14159f / 180.0f;
    velocity = sf::Vector2f(std::cos(angle), std::sin(angle));

    angle_deg = 0.0f;

    // Create triangular shape for visualization
    shape.setPrimitiveType(sf::PrimitiveType::Triangles);
    shape.resize(3);

    // Define triangle vertices (points forward along x-axis)
    shape[0].position = sf::Vector2f(6.0f, 0.0f);      // Front point
    shape[1].position = sf::Vector2f(-3.0f, -3.0f);    // Back-left point
    shape[2].position = sf::Vector2f(-3.0f, 3.0f);     // Back-right point

    // Set color to white
    for(int i = 0; i < 3; ++i) {
        shape[i].color = sf::Color::White;
    }
}

/**
 * Calculate flocking forces based on neighboring boids
 * This is the most computationally expensive part of the simulation (O(n^2) complexity)
 *
 * @param boids Reference to all boids in the simulation
 */
void Boid::calculateRules(const std::vector<Boid>& boids) {
    // Accumulators for each flocking rule
    sf::Vector2f cohesion(0, 0);      // Attraction to group center
    sf::Vector2f separation(0, 0);    // Repulsion from nearby boids
    sf::Vector2f alignment(0, 0);     // Match average velocity

    // Count neighbors for each rule (for averaging)
    int cohesionNeighbors = 0;
    int separationNeighbors = 0;
    int alignmentNeighbors = 0;

    // Examine all other boids to find neighbors
    for (const Boid& other : boids) {
        float distance = magnitude(other.position - this->position);
        if (&other == this) continue;  // Skip self

        // Cohesion and alignment rules (within visual range)
        if (distance > 0 && distance < VISUAL_RANGE) {
            cohesion += other.position;     // Accumulate positions for center of mass
            cohesionNeighbors++;
            alignment += other.velocity;    // Accumulate velocities for averaging
            alignmentNeighbors++;
        }

        // Separation rule (within close separation range)
        if (distance > 0 && distance < SEPARATION_RANGE) {
            sf::Vector2f away = this->position - other.position;  // Vector pointing away
            separation += away;
            separationNeighbors++;
        }
    }

    // Reset acceleration for this frame
    this->acceleration = sf::Vector2f(0, 0);

    // Apply cohesion: Steer towards average position of neighbors
    if (cohesionNeighbors > 0) {
        cohesion /= static_cast<float>(cohesionNeighbors);  // Average position
        cohesion -= this->position;                          // Direction to center
        this->acceleration += cohesion * COHESION_WEIGHT;
    }

    // Apply alignment: Match average velocity of neighbors
    if (alignmentNeighbors > 0) {
        alignment /= static_cast<float>(alignmentNeighbors);  // Average velocity
        alignment -= this->velocity;                           // Velocity difference
        this->acceleration += alignment * ALIGNMENT_WEIGHT;
    }

    // Apply separation: Move away from crowded neighbors
    if (separationNeighbors > 0) {
        this->acceleration += separation * SEPARATION_WEIGHT;
    }

    // Apply directional bias to encourage unified flock formation
    this->acceleration += BIAS_DIRECTION * BIAS_WEIGHT;
}

/**
 * Apply forces to keep boids within screen boundaries
 * Uses a gradual repulsion that increases quadratically as boid approaches edge
 */
/**
 * Apply forces to keep boids within screen boundaries
 * Uses a gradual repulsion that increases quadratically as boid approaches edge
 */
void Boid::applyBoundaryForces() {
    sf::Vector2f steer(0, 0);
    float force_strength = 0;

    // Left edge: Push boid to the right
    if (position.x < EDGE_MARGIN) {
        const float penetration = EDGE_MARGIN - position.x;
        const float ramp = penetration / EDGE_MARGIN;  // Normalize to [0, 1]
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);  // Quadratic increase
        steer.x = force_strength;
    }
    // Right edge: Push boid to the left
    else if (position.x > SCREEN_WIDTH - EDGE_MARGIN) {
        const float penetration = position.x - (SCREEN_WIDTH - EDGE_MARGIN);
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.x = -force_strength;
    }
    // Top edge: Push boid down
    if (position.y < EDGE_MARGIN) {
        const float penetration = EDGE_MARGIN - position.y;
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = force_strength;
    }
    // Bottom edge: Push boid up
    else if (position.y > SCREEN_HEIGHT - EDGE_MARGIN) {
        const float penetration = position.y - (SCREEN_HEIGHT - EDGE_MARGIN);
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = -force_strength;
    }

    // Add boundary repulsion to accumulated acceleration
    this->acceleration += steer;
}

/**
 * Update boid's position and velocity based on accumulated forces
 * This method enforces speed limits and updates orientation
 */
void Boid::updateState() {
    // Apply accumulated acceleration to velocity
    velocity += acceleration;

    // Enforce maximum speed limit
    if (const float speed = magnitude(velocity); speed > MAX_SPEED) {
        velocity = (velocity / speed) * MAX_SPEED;  // Normalize and scale to max
    }
    // Enforce minimum speed limit
    else if (speed < MIN_SPEED) {
        if (speed == 0.0f) {
            velocity = sf::Vector2f(0, MIN_SPEED);  // Avoid division by zero
        } else {
            velocity = (velocity / speed) * MIN_SPEED;  // Normalize and scale to min
        }
    }

    // Update orientation angle based on velocity direction
    if (velocity.x != 0.0f || velocity.y != 0.0f) {
        const float angle_rad = std::atan2(velocity.y, velocity.x);
        this->angle_deg = angle_rad * 180.0f / 3.14159f;  // Convert to degrees
    }

    // Update position based on velocity
    position += velocity;
}

/**
 * Render the boid to the window
 * Applies translation and rotation transforms to orient the triangle correctly
 *
 * @param window SFML render window to draw to
 */
void Boid::draw(sf::RenderWindow& window) const
{
    sf::Transform transform;
    transform.translate(position);           // Move to boid's position
    transform.rotate(sf::degrees(angle_deg)); // Rotate to face direction of movement
    sf::RenderStates states;
    states.transform = transform;
    window.draw(shape, states);              // Draw the triangle
}
