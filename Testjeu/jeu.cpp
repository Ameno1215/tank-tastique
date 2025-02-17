#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Deplacement Robot avec Pause");

    // Charger la texture du fond
    sf::Texture textureFond;
    if (!textureFond.loadFromFile("fond.png")) {
        return -1;
    }
    sf::Sprite spriteFond(textureFond);
    spriteFond.setScale(800.0f / textureFond.getSize().x, 600.0f / textureFond.getSize().y);

    // Charger la texture du robot
    sf::Texture textureRobot;
    if (!textureRobot.loadFromFile("robot.png")) {
        return -1;
    }
    sf::Sprite spriteRobot(textureRobot);
    spriteRobot.setPosition(400 - textureRobot.getSize().x / 2, 300 - textureRobot.getSize().y / 2);

    float speed = 1.0f; // Vitesse du robot
    bool isPaused = false; // État de la pause

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Pause / Reprise avec la touche ESPACE
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isPaused = !isPaused;
            }
        }

        if (!isPaused) { // Si ce n'est pas en pause, on bouge
            sf::Vector2f movement(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                movement.y -= speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                movement.y += speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                movement.x -= speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                movement.x += speed;

            spriteRobot.move(movement);

            // Empêcher le sprite de sortir des limites
            sf::Vector2f pos = spriteRobot.getPosition();
            float spriteWidth = textureRobot.getSize().x;
            float spriteHeight = textureRobot.getSize().y;
            pos.x = std::max(0.f, std::min(800.f - spriteWidth, pos.x));
            pos.y = std::max(0.f, std::min(600.f - spriteHeight, pos.y));
            spriteRobot.setPosition(pos);
        }

        // Affichage
        window.clear();
        window.draw(spriteFond);
        window.draw(spriteRobot);
        window.display();
    }

    return 0;
}
