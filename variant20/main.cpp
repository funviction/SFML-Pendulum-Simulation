#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <string>
#include <algorithm>
#include <optional> // Обов'язково для подій в SFML 3

const float G = 980.f; // Пікселів/с^2
const float PI = 3.14159265f;

struct PendulumState {
    float angle;
    float angularVelocity;
    float length;
    float initialAngle;
    bool isPaused;
};

void handleInput(sf::RenderWindow& window, PendulumState& state) {
    // Новий синтаксис обробки подій для SFML 3
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Up) {
                state.length = std::clamp(state.length - 10.f, 50.f, 500.f);
            }
            if (key->code == sf::Keyboard::Key::Down) {
                state.length = std::clamp(state.length + 10.f, 50.f, 500.f);
            }
            if (key->code == sf::Keyboard::Key::Right) {
                state.initialAngle = std::clamp(state.initialAngle + 0.1f, -PI / 2, PI / 2);
                if (state.isPaused) state.angle = state.initialAngle;
            }
            if (key->code == sf::Keyboard::Key::Left) {
                state.initialAngle = std::clamp(state.initialAngle - 0.1f, -PI / 2, PI / 2);
                if (state.isPaused) state.angle = state.initialAngle;
            }
            if (key->code == sf::Keyboard::Key::Space) {
                state.isPaused = !state.isPaused;
            }
            if (key->code == sf::Keyboard::Key::R) {
                state.angle = state.initialAngle;
                state.angularVelocity = 0.f;
            }
        }
    }
}

void updatePhysics(PendulumState& state, float dt) {
    if (state.isPaused) return;

    float angularAcceleration = -(G / state.length) * std::sin(state.angle);
    state.angularVelocity += angularAcceleration * dt;
    state.angle += state.angularVelocity * dt;
}

void drawScene(sf::RenderWindow& window, const PendulumState& state, sf::Font& font) {
    window.clear(sf::Color(30, 30, 30));

    sf::Vector2f anchor(400.f, 100.f);
    sf::Vector2f bobPos(
        anchor.x + state.length * std::sin(state.angle),
        anchor.y + state.length * std::cos(state.angle)
    );

    sf::Vertex line[2];
    line[0].position = anchor;
    line[0].color = sf::Color::White;
    line[1].position = bobPos;
    line[1].color = sf::Color::White;

    sf::CircleShape bob(20.f);
    bob.setOrigin({ 20.f, 20.f });
    bob.setPosition(bobPos);
    bob.setFillColor(sf::Color::Cyan);
    window.draw(bob);

    // Новий синтаксис тексту SFML 3
    sf::Text infoText(font, "", 16);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({ 10.f, 10.f });

    float theoreticalPeriod = 2.0f * PI * std::sqrt(state.length / G);

    infoText.setString(
        "Length (Up/Down): " + std::to_string(state.length) + "\n" +
        "Init Angle (Left/Right): " + std::to_string(state.initialAngle) + "\n" +
        "Current Angle: " + std::to_string(state.angle) + "\n" +
        "Theoretical Period: " + std::to_string(theoreticalPeriod) + " s\n" +
        "State: " + (state.isPaused ? "PAUSED (Space to play, R to restart)" : "RUNNING")
    );
    window.draw(infoText);
    window.display();
}

int main() {
    // Фігурні дужки {800, 600} для SFML 3
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Swing Simulation - Variant 20");
    window.setFramerateLimit(60);

    sf::Font font;
    // openFromFile замість loadFromFile для SFML 3
    if (!font.openFromFile("../resources/fonts/arial.ttf")) {
        return -1;
    }

    PendulumState state = { PI / 4.f, 0.f, 250.f, PI / 4.f, true };
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        handleInput(window, state);
        updatePhysics(state, dt);
        drawScene(window, state, font);
    }
    return 0;
}