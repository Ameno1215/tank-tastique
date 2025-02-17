#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "config/config.hpp"
#include "classe/tank.hpp"


int main() {
    sf::RenderWindow window(sf::VideoMode(2400, 2000), "Lien entre objets");

    tank mon_tank;
    // Charger les textures
    sf::Texture textureBase, textureTourelle;
    if (!textureBase.loadFromFile("Image/base1.png") || !textureTourelle.loadFromFile("Image/tourelle2.png"))
        return -1;
    
    sf::Vector2u taillebase = textureBase.getSize();
    sf::Vector2u tailletourelle = textureTourelle.getSize();
    sf::Sprite spriteBase(textureBase);
    spriteBase.setPosition(300, 200);
    

    sf::Sprite spriteTourelle(textureTourelle);     
    spriteTourelle.setPosition(spriteBase.getPosition().x+taillebase.x/2-tailletourelle.x/2,spriteBase.getPosition().y+taillebase.y/2-tailletourelle.y/1.8);


    // Longueur fixe du lien
    mon_tank.set_vit(0.5f);
    mon_tank.set_ori(0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Contrôle indépendant du robots
        sf::Vector2f movement(0.f, 0.f);
        movement.y=mon_tank.get_y();
        movement.x=mon_tank.get_x();
        float speed=mon_tank.get_vit();
        float rotation=mon_tank.get_ori();

        spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) mon_tank.set_y(movement.y -= speed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) mon_tank.set_x(movement.y += speed);
        spriteBase.move(movement);
        spriteTourelle.move(movement);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) mon_tank.set_ori(rotation += 0.1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mon_tank.set_ori(rotation -= 0.1);
        spriteBase.setRotation(rotation);
        //sprite.setRotation(angle); 
        // Vérifier la distance entre les objets
        sf::Vector2f diff = spriteTourelle.getPosition() - spriteBase.getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // Si la distance dépasse la limite, ajuster la position de l'objet attaché
        //if (distance > 1) {
        //    sf::Vector2f direction = diff / distance; // Normalisation du vecteur
        //    spriteTourelle.setPosition(spriteBase.getPosition() + direction * speed);
        //}

        // Affichage
        window.clear();
        window.draw(spriteBase);
        window.draw(spriteTourelle);

        // Dessiner une ligne pour visualiser le lien
        //sf::Vertex line[] = {
        //    sf::Vertex(spriteBase.getPosition() + sf::Vector2f(25, 25), sf::Color::Red),
        //    sf::Vertex(spriteTourelle.getPosition() + sf::Vector2f(25, 25), sf::Color::Red)
        //};
        //window.draw(line, 2, sf::Lines);

        window.display();
    }

    return 0;
}
