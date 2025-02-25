#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "classe/tank.hpp"
#include "deplacement.hpp"
#include "classe/tir.hpp"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "Lien entre objets");
    window.setMouseCursorVisible(false);

    tank mon_tank;

    sf::Texture textureCurseur;
    textureCurseur.loadFromFile("Image/curseur_rouge.png");
    sf::Sprite cursorSprite(textureCurseur);
    cursorSprite.setScale(0.08f, 0.08f);


    while (window.isOpen()) {
        window.clear();
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


        
        // TIR
        Noeud* courant = mon_tank.getListeObus().get_tete();

        while (courant) {
            if (courant->obus.get_status()) {
                // Calcul du déplacement
                sf::Vector2f deltaMove;
                deltaMove.x = courant->obus.get_vitesse() * sin(courant->obus.get_Sprite().getRotation() * M_PI / 180);
                deltaMove.y = -courant->obus.get_vitesse() * cos(courant->obus.get_Sprite().getRotation() * M_PI / 180);
                
                // position = position + déplacement
                courant->obus.get_Sprite().setPosition(courant->obus.get_Sprite().getPosition().x + deltaMove.x, courant->obus.get_Sprite().getPosition().y + deltaMove.y);

                // test si depassement de la porté du tir
                int longueur = sqrt(pow(courant->obus.get_Sprite().getPosition().x - courant->obus.get_position_tir().x, 2) + pow(courant->obus.get_Sprite().getPosition().y - courant->obus.get_position_tir().y, 2));
                if (longueur > courant->obus.get_porte()) {
                    courant->obus.set_status(0); // destruction obus
                    Noeud * temp = courant->suivant;
                    mon_tank.getListeObus().supprimer(courant->index);
                    courant = temp;
                }
                else {
                    window.draw(courant->obus.get_Sprite());
                    courant = courant->suivant;
                }
            }
        }

        

        // si clique souris tir
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (get_time_seconds() - mon_tank.getListeObus().get_time_dernier_tir()) > mon_tank.get_cadence_tir()) {
            // ajout création du nouvel obus et l'ajouter à la liste d'obus du tank
            int index = mon_tank.getListeObus().ajouterFin(mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getRotation(), 0.2, 500, "Image/obus.png");
            mon_tank.getListeObus().set_time_dernier_tir(get_time_seconds());
            mon_tank.getListeObus().trouverNoeud(index)->obus.initTir(mon_tank.getTourelleSprite().getRotation(), mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y);
            window.draw(mon_tank.getListeObus().trouverNoeud(index)->obus.get_Sprite());

            // mon_tank.getListeObus().afficher();
            }


        
        
        window.draw(mon_tank.getBaseSprite());
        window.draw(mon_tank.getTourelleSprite());
        window.draw(cursorSprite);
        window.display();
    }

    return 0;
}
