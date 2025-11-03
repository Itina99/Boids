#include "Boid.h"
#include <cmath>

// --- Vector Math Helper Functions ---
// We place them in an anonymous namespace to keep them private to this file.
namespace {
    float magnitude(sf::Vector2f v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    sf::Vector2f normalize(sf::Vector2f v) {
        float mag = magnitude(v);
        if (mag > 0) {
            return v / mag;
        }
        return sf::Vector2f(0, 0);
    }

    void limit(sf::Vector2f &v, float max) {
        float mag = magnitude(v);
        if (mag > max) {
            v = normalize(v) * max;
        }
    }
} // end anonymous namespace


// --- Boid Class Method Definitions ---

Boid::Boid(float x, float y) {
    position = sf::Vector2f(x, y);
    float angle = (rand() % 360) * 3.14159f / 180.0f;
    velocity = sf::Vector2f(std::cos(angle), std::sin(angle));
    angle_deg = 0.0f; // Inizializza l'angolo

    // --- CORREZIONE: Inizializza shape come triangolo ---
    shape.setPrimitiveType(sf::PrimitiveType::Triangles); // <-- CORRETTO
    shape.resize(3);

    // Definisci i vertici del triangolo (punta in avanti, base dietro)
    // Queste coordinate sono relative al punto (0,0)
    shape[0].position = sf::Vector2f(6.0f, 0.0f);   // Punta (davanti)
    shape[1].position = sf::Vector2f(-3.0f, -3.0f); // Sinistra (dietro)
    shape[2].position = sf::Vector2f(-3.0f, 3.0f);  // Destra (dietro)

    for(int i = 0; i < 3; ++i) {
        shape[i].color = sf::Color::White;
    }

}

// --- MODIFICATO: Logica di Calcolo Regole ---
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

        // Regole di Coesione e Allineamento
        if (distance > 0 && distance < VISUAL_RANGE) {
            cohesion += other.position;
            cohesionNeighbors++;
            alignment += other.velocity;
            alignmentNeighbors++;
        }

        // Regola di Separazione
        if (distance > 0 && distance < SEPARATION_RANGE) {
            // Loro usano un calcolo più semplice: solo la differenza
            sf::Vector2f away = this->position - other.position;
            separation += away; // NON normalizziamo o dividiamo per distanza
            separationNeighbors++;
        }
    }

    // Resetta l'accelerazione (che ora usiamo come "v_adjustment")
    this->acceleration = sf::Vector2f(0, 0);

    // --- Finalizza i calcoli (Nuova Logica) ---
    if (cohesionNeighbors > 0) {
        cohesion /= (float)cohesionNeighbors;   // Media
        cohesion -= this->position;             // Vettore (target - current)
        // NON usiamo limita(cohesion, MAX_FORCE)
        this->acceleration += cohesion * COHESION_WEIGHT; // Applica il fattore
    }

    if (alignmentNeighbors > 0) {
        alignment /= (float)alignmentNeighbors; // Media
        alignment -= this->velocity;            // Vettore (target - current)
        // NON usiamo limita(alignment, MAX_FORCE)
        this->acceleration += alignment * ALIGNMENT_WEIGHT; // Applica il fattore
    }

    if (separationNeighbors > 0) {
        // Loro NON fanno la media, sommano solo le repulsioni
        // (l'abbiamo già fatto nel loop)
        // NON usiamo limita(separation, MAX_FORCE)
        this->acceleration += separation * SEPARATION_WEIGHT; // Applica il fattore
    }
    this->acceleration += BIAS_DIRECTION * BIAS_WEIGHT;
}

// --- MODIFICATO: Logica dei Bordi ---
// La nostra logica (quadratica) è più fluida della loro (if/else rigido).
// La teniamo, ma il AVOID_EDGE_FORCE (ora 0.05) la renderà
// una "micro-correzione" in linea con le altre regole.
// La logica quadratica c'è ancora.
void Boid::applyBoundaryForces() {
    sf::Vector2f steer(0, 0);
    float force_strength = 0;

    // Left edge
    if (position.x < EDGE_MARGIN) {
        float penetration = EDGE_MARGIN - position.x;
        float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.x = force_strength;
    }
    // Right edge
    else if (position.x > SCREEN_WIDTH - EDGE_MARGIN) {
        float penetration = position.x - (SCREEN_WIDTH - EDGE_MARGIN);
        float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.x = -force_strength;
    }
    // Top edge
    if (position.y < EDGE_MARGIN) {
        float penetration = EDGE_MARGIN - position.y;
        float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = force_strength;
    }
    // Bottom edge
    else if (position.y > SCREEN_HEIGHT - EDGE_MARGIN) {
        float penetration = position.y - (SCREEN_HEIGHT - EDGE_MARGIN);
        float ramp = penetration / EDGE_MARGIN;
        force_strength = AVOID_EDGE_FORCE * (ramp * ramp);
        steer.y = -force_strength;
    }

    this->acceleration += steer;
}


void Boid::updateState() {
    velocity += acceleration;

    // Applica Limiti Min/Max Velocità
    float speed = magnitude(velocity); // <-- CORRETTO

    if (speed > MAX_SPEED) {
        velocity = (velocity / speed) * MAX_SPEED;
    }
    else if (speed < MIN_SPEED) {
        if (speed == 0.0f) {
            velocity = sf::Vector2f(0, MIN_SPEED);
        } else {
            velocity = (velocity / speed) * MIN_SPEED;
        }
    }

    // Calcola l'angolo
    if (velocity.x != 0.0f || velocity.y != 0.0f) {
        float angle_rad = std::atan2(velocity.y, velocity.x);
        this->angle_deg = angle_rad * 180.0f / 3.14159f;
    }

    position += velocity;
}

// Disegna il boid sulla finestra
void Boid::draw(sf::RenderWindow& window) {
    // --- CORREZIONE: Applica la trasformazione al momento del disegno ---

    // 1. Crea una matrice di trasformazione
    sf::Transform transform;

    // 2. Applica la posizione (traslazione)
    transform.translate(position);

    // 3. Applica la rotazione
    transform.rotate(sf::degrees(angle_deg)); // Fixed explicit constructor issue

    // 4. Crea un "RenderStates" che usa la nostra trasformazione
    sf::RenderStates states;
    states.transform = transform;

    // 5. Disegna il VertexArray usando gli stati
    window.draw(shape, states);
}