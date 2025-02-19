#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "config/config.hpp"
#include "classe/tank.hpp"
#include "deplacement.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Lien entre objets");
    window.setMouseCursorVisible(false);

    tank mon_tank;

    sf::Texture textureCurseur;
    textureCurseur.loadFromFile("Image/curseur_rouge.png");
    sf::Sprite cursorSprite(textureCurseur);
    cursorSprite.setScale(0.08f, 0.08f);

    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
        sf::Vector2f dir = worldMousePos - mon_tank.getTourelleSprite().getPosition();
        float angle_voulu = atan2(dir.y, dir.x) * 180 / M_PI - 90;

        cursorSprite.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2f movement(0.f, 0.f);
        float speed = mon_tank.get_vit();
        float rotation = mon_tank.get_ori();
        float vit_canon = mon_tank.get_vit_canon();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            deplacement_verticale(mon_tank, movement, rotation, mon_tank.getBaseSprite(), speed);
            deplacement_verticale(mon_tank, movement, rotation, mon_tank.getTourelleSprite(), speed);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            deplacement_verticale(mon_tank, movement, rotation, mon_tank.getBaseSprite(), -speed);
            deplacement_verticale(mon_tank, movement, rotation, mon_tank.getTourelleSprite(), -speed);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) 
            deplacement_rotation(mon_tank, mon_tank.getBaseSprite(), &rotation, 0.05);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) 
            deplacement_rotation(mon_tank, mon_tank.getBaseSprite(), &rotation, -0.05);

        float angle_actu = mon_tank.getTourelleSprite().getRotation();
        float diff = angle_voulu - angle_actu;
        if (diff > 180) diff -= 360;
        if (diff < -180) diff += 360;

        mon_tank.getTourelleSprite().setRotation(angle_actu + diff * vit_canon);

        window.clear();
        window.draw(mon_tank.getBaseSprite());
        window.draw(mon_tank.getTourelleSprite());
        window.draw(cursorSprite);
        window.display();
    }

    return 0;
}
