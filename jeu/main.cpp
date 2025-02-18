#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "config/config.hpp"
#include "classe/tank.hpp"


int main() {
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Lien entre objets");
    window.setMouseCursorVisible(false);
    tank mon_tank;
    sf::Texture cursorTexture,textureBase, textureTourelle;
    cursorTexture.loadFromFile("Image/curseur_rouge.png"); // Remplace par ton image
    sf::Sprite cursorSprite(cursorTexture);
    // Charger les textures
    if (!textureBase.loadFromFile("Image/base1.png") || !textureTourelle.loadFromFile("Image/tourelle2.png"))
        return -1;
    
    sf::Vector2u taillebase = textureBase.getSize();
    sf::Vector2u tailletourelle = textureTourelle.getSize();
    sf::Vector2u taillecurseur = cursorTexture.getSize();
    sf::Sprite spriteBase(textureBase);    

    sf::Sprite spriteTourelle(textureTourelle);     
    // Définir l'origine des sprites au centre
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

    // Positionner la base au centre initial
    spriteBase.setPosition(300, 200);

    // Superposer la tourelle au centre de la base
    spriteTourelle.setPosition(spriteBase.getPosition());
    spriteBase.setScale(0.5,0.5);
    spriteTourelle.setScale(0.5,0.5);



    // Longueur fixe du lien
    mon_tank.set_vit(0.2f);
    mon_tank.set_ori(0);

    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
        sf::Vector2f dir = worldMousePos - spriteTourelle.getPosition();
        float angle = atan2(dir.y + taillecurseur.y/2, dir.x+ taillecurseur.x/2)* 180 / M_PI -90;

        cursorSprite.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
            mon_tank.set_y(movement.y += speed*cos(rotation*M_PI/180));
            mon_tank.set_x(movement.x -= speed*sin(rotation*M_PI/180) );
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            mon_tank.set_y(movement.y -= speed*cos(rotation*M_PI/180));
            mon_tank.set_x(movement.x += speed*sin(rotation*M_PI/180));
        }

        spriteBase.move(movement);
        spriteTourelle.move(movement);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) mon_tank.set_ori(rotation += 0.05);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) mon_tank.set_ori(rotation -= 0.05);

        spriteBase.setRotation(rotation);
    
        spriteTourelle.setRotation(angle);
        //sprite.setRotation(angle); 
        // Vérifier la distance entre les objets

        // Si la distance dépasse la limite, ajuster la position de l'objet attaché
        //if (distance > 1) {
        //    sf::Vector2f direction = diff / distance; // Normalisation du vecteur
        //    spriteTourelle.setPosition(spriteBase.getPosition() + direction * speed);
        //}

        // Affichage
        window.clear();
        window.draw(spriteBase);
        window.draw(spriteTourelle);
        window.draw(cursorSprite);

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
