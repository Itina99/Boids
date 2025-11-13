#include "Boid.h"
#include <cmath>

// --- Vector Math Helper Functions ---
namespace {
    float magnitude(sf::Vector2f v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }
}


// --- Boid Class Method Definitions ---
Boid::Boid(float x, float y) {
    position = sf::Vector2f(x, y);
    const float angle = rand() % 360 * 3.14159f / 180.0f;
    velocity = sf::Vector2f(std::cos(angle), std::sin(angle));
    angle_deg = 0.0f;
    shape.setPrimitiveType(sf::PrimitiveType::Triangles);
    shape.resize(3);

    shape[0].position = sf::Vector2f(6.0f, 0.0f);
    shape[1].position = sf::Vector2f(-3.0f, -3.0f);
    shape[2].position = sf::Vector2f(-3.0f, 3.0f);

    for(int i = 0; i < 3; ++i) {
        shape[i].color = sf::Color::White;
    }

}

void Boid::calculateRules(const std::vector<Boid>& boids) {
    sf::Vector2f cohesion(0, 0);
    sf::Vector2f separation(0, 0);
    sf::Vector2f alignment(0, 0);

    int cohesionNeighbors = 0;
    int separationNeighbors = 0;
    int alignmentNeighbors = 0;


    for (const Boid& other : boids) {
        float distance = magnitude(other.position - this->position);
        if (&other == this) continue;

        // Coesion and alignment rules
        if (distance > 0 && distance < VISUAL_RANGE) {
            cohesion += other.position;
            cohesionNeighbors++;
            alignment += other.velocity;
            alignmentNeighbors++;
        }

        // Separation rule
        if (distance > 0 && distance < SEPARATION_RANGE) {
            sf::Vector2f away = this->position - other.position;
            separation += away;
            separationNeighbors++;
        }
    }
    this->acceleration = sf::Vector2f(0, 0);

    if (cohesionNeighbors > 0) {
        cohesion /= static_cast<float>(cohesionNeighbors);
        cohesion -= this->position;
        this->acceleration += cohesion * COHESION_WEIGHT;
    }

    if (alignmentNeighbors > 0) {
        alignment /= static_cast<float>(alignmentNeighbors);
        alignment -= this->velocity;
        this->acceleration += alignment * ALIGNMENT_WEIGHT;
    }

    if (separationNeighbors > 0) {
        this->acceleration += separation * SEPARATION_WEIGHT;
    }
    this->acceleration += BIAS_DIRECTION * BIAS_WEIGHT;
}

void Boid::applyBoundaryForces() {
    sf::Vector2f steer(0, 0);
    float force_strength = 0;

    // Left edge
    if (position.x < EDGE_MARGIN) {
        const float penetration = EDGE_MARGIN - position.x;
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.x = force_strength;
    }
    // Right edge
    else if (position.x > SCREEN_WIDTH - EDGE_MARGIN) {
        const float penetration = position.x - (SCREEN_WIDTH - EDGE_MARGIN);
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.x = -force_strength;
    }
    // Top edge
    if (position.y < EDGE_MARGIN) {
        const float penetration = EDGE_MARGIN - position.y;
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = force_strength;
    }
    // Bottom edge
    else if (position.y > SCREEN_HEIGHT - EDGE_MARGIN) {
        const float penetration = position.y - (SCREEN_HEIGHT - EDGE_MARGIN);
        const float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = -force_strength;
    }

    this->acceleration += steer;
}


void Boid::updateState() {
    velocity += acceleration;
    if (const float speed = magnitude(velocity); speed > MAX_SPEED) {
        velocity = (velocity / speed) * MAX_SPEED;
    }
    else if (speed < MIN_SPEED) {
        if (speed == 0.0f) {
            velocity = sf::Vector2f(0, MIN_SPEED);
        } else {
            velocity = (velocity / speed) * MIN_SPEED;
        }
    }
    if (velocity.x != 0.0f || velocity.y != 0.0f) {
        const float angle_rad = std::atan2(velocity.y, velocity.x);
        this->angle_deg = angle_rad * 180.0f / 3.14159f;
    }

    position += velocity;
}
void Boid::draw(sf::RenderWindow& window) const
{
    sf::Transform transform;
    transform.translate(position);
    transform.rotate(sf::degrees(angle_deg));
    sf::RenderStates states;
    states.transform = transform;
    window.draw(shape, states);
}
