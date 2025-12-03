#include <SFML/Graphics.hpp>
#include <iostream>

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    sf::CircleShape shape;

    sf::Vector2f initialPosition;

    Particle(float x, float y, float m, float radius) {
        position = sf::Vector2f(x, y);
        velocity = sf::Vector2f(0.f, 0.f);
        mass = m;

        initialPosition = sf::Vector2f(x, y); 

        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Cyan);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
    }

    void update(sf::Vector2f force, float dt) {
        sf::Vector2f acceleration = force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;
        velocity *= 0.998f; 
        shape.setPosition(position);
    }

    void reset() {
        // Kembalikan posisi ke awal
        position = initialPosition;
        // PENTING: Reset juga kecepatannya ke nol
        velocity = sf::Vector2f(0.f, 0.f);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Simulasi Fisika 2D (Tekan 'R' untuk Reset)");
    window.setFramerateLimit(60); 

    Particle particle(400.f, 300.f, 10.f, 20.f);
    sf::Clock clock;
    float forceStrength = 10000.f;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) {
            dt = 0.1f;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    particle.reset(); // Panggil fungsi reset
                }
            }
        }
        
        sf::Vector2f totalForce(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            totalForce.y -= forceStrength;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            totalForce.y += forceStrength;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            totalForce.x -= forceStrength;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            totalForce.x += forceStrength;
        }

        particle.update(totalForce, dt);

        window.clear(sf::Color(10, 10, 30));
        particle.draw(window);
        window.display();
    }

    return 0;
}