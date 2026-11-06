#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <string>
#include <optional>

const float G = 980.f;
const float L = 250.f;
const float PI = 3.14159265f;

struct Pendulum {
    float angle;
    float omega;
};

float getAcceleration(float angle) {
    return -(G / L) * std::sin(angle);
}

// 1. Метод Ейлера
void updateEuler(Pendulum& p, float dt) {
    float currentAcc = getAcceleration(p.angle);
    float currentOmega = p.omega;
    p.angle += currentOmega * dt;
    p.omega += currentAcc * dt;
}

// 2. Метод Рунге-Кутти 4
void updateRK4(Pendulum& p, float dt) {
    float k1_omega = getAcceleration(p.angle);
    float k1_theta = p.omega;

    float k2_omega = getAcceleration(p.angle + 0.5f * dt * k1_theta);
    float k2_theta = p.omega + 0.5f * dt * k1_omega;

    float k3_omega = getAcceleration(p.angle + 0.5f * dt * k2_theta);
    float k3_theta = p.omega + 0.5f * dt * k2_omega;

    float k4_omega = getAcceleration(p.angle + dt * k3_theta);
    float k4_theta = p.omega + dt * k3_omega;

    p.omega += (dt / 6.0f) * (k1_omega + 2.0f * k2_omega + 2.0f * k3_omega + k4_omega);
    p.angle += (dt / 6.0f) * (k1_theta + 2.0f * k2_theta + 2.0f * k3_theta + k4_theta);
}

float calculateEnergy(const Pendulum& p) {
    float kinetic = 0.5f * (L * L) * (p.omega * p.omega);
    float potential = G * L * (1.0f - std::cos(p.angle));
    return kinetic + potential;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 900, 600 }), "Euler vs RK4 - Variant 34");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Users/Kompik.Store/source/repos/SFML_Lab 34/resources/fonts/arial.ttf")) return -1;

    Pendulum pEuler = { PI / 3.f, 0.f };
    Pendulum pRK4 = { PI / 3.f, 0.f };
    float initialEnergy = calculateEnergy(pRK4);

    sf::Clock clock;
    bool isPaused = true;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Space) {
                    isPaused = !isPaused;
                }
                if (key->code == sf::Keyboard::Key::R) {
                    pEuler = { PI / 3.f, 0.f };
                    pRK4 = { PI / 3.f, 0.f };
                    isPaused = true;
                }
            }
        }

        if (!isPaused) {
            updateEuler(pEuler, dt);
            updateRK4(pRK4, dt);
        }

        window.clear(sf::Color(20, 20, 20));

        sf::Vector2f anchorEuler(250.f, 150.f);
        sf::Vector2f posEuler(
            anchorEuler.x + L * std::sin(pEuler.angle),
            anchorEuler.y + L * std::cos(pEuler.angle)
        );

        sf::Vector2f anchorRK4(650.f, 150.f);
        sf::Vector2f posRK4(
            anchorRK4.x + L * std::sin(pRK4.angle),
            anchorRK4.y + L * std::cos(pRK4.angle)
        );

        sf::Vertex lineEuler[2];
        lineEuler[0].position = anchorEuler; lineEuler[0].color = sf::Color::White;
        lineEuler[1].position = posEuler;    lineEuler[1].color = sf::Color::White;
        window.draw(lineEuler, 2, sf::PrimitiveType::Lines);

        sf::CircleShape bobEuler(15.f);
        bobEuler.setOrigin({ 15.f, 15.f });
        bobEuler.setPosition(posEuler);
        bobEuler.setFillColor(sf::Color::Red);
        window.draw(bobEuler);

        sf::Vertex lineRK4[2];
        lineRK4[0].position = anchorRK4; lineRK4[0].color = sf::Color::White;
        lineRK4[1].position = posRK4;    lineRK4[1].color = sf::Color::White;
        window.draw(lineRK4, 2, sf::PrimitiveType::Lines);

        sf::CircleShape bobRK4(15.f);
        bobRK4.setOrigin({ 15.f, 15.f });
        bobRK4.setPosition(posRK4);
        bobRK4.setFillColor(sf::Color::Green);
        window.draw(bobRK4);

        sf::Text info(font, "", 16);
        info.setFillColor(sf::Color::White);
        info.setString(
            "SPACE to Pause/Play, R to Reset\n\n"
            "EXPLICIT EULER (Red):\n"
            "Current Energy: " + std::to_string(calculateEnergy(pEuler)) + "\n"
            "Energy Error: " + std::to_string(std::abs(calculateEnergy(pEuler) - initialEnergy)) + "\n\n"
            "RUNGE-KUTTA 4 (Green):\n"
            "Current Energy: " + std::to_string(calculateEnergy(pRK4)) + "\n"
            "Energy Error: " + std::to_string(std::abs(calculateEnergy(pRK4) - initialEnergy))
        );
        info.setPosition({ 20.f, 20.f });
        window.draw(info);

        window.display();
    }
    return 0;
}