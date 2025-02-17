#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "config/config.hpp"
#include "classe/tank.hpp"


int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 1000), "Lien entre objets");

    tank mon_tank;
    // Charger les textures
    sf::Texture textureRobot, textureTable;
    if (!textureRobot.loadFromFile("base1.png") || !textureTable.loadFromFile("tourelle1.png"))
        return -1;

    sf::Sprite spriteRobot(textureRobot);
    spriteRobot.setPosition(300, 200);

    sf::Sprite spriteTable(textureTable);
    spriteTable.setPosition(500, 300);

    // Longueur fixe du lien
    mon_tank.set_vit(1.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Contrôle indépendant du robot
        sf::Vector2f movement(0.f, 0.f);
        movement.y=mon_tank.get_y();
        movement.x=mon_tank.get_x();
        float speed=mon_tank.get_vit();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) mon_tank.set_y(movement.y -= speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) mon_tank.set_x(movement.y += speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) mon_tank.set_x(movement.x -= speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mon_tank.set_x(movement.x += speed);
        spriteRobot.move(movement);

        // Vérifier la distance entre les objets
        sf::Vector2f diff = spriteTable.getPosition() - spriteRobot.getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // Si la distance dépasse la limite, ajuster la position de l'objet attaché
        if (distance > 1) {
            sf::Vector2f direction = diff / distance; // Normalisation du vecteur
            spriteTable.setPosition(spriteRobot.getPosition() + direction * speed);
        }

        // Affichage
        window.clear();
        window.draw(spriteRobot);
        window.draw(spriteTable);

        // Dessiner une ligne pour visualiser le lien
        sf::Vertex line[] = {
            sf::Vertex(spriteRobot.getPosition() + sf::Vector2f(25, 25), sf::Color::Red),
            sf::Vertex(spriteTable.getPosition() + sf::Vector2f(25, 25), sf::Color::Red)
        };
        window.draw(line, 2, sf::Lines);

        window.display();
    }

    return 0;
}
