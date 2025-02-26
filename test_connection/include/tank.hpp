#ifndef TANK_H
#define TANK_H

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tir.hpp"

class tank {
public:
    // Constructeur
    tank();

    // Accesseurs
    float get_x();
    float get_y();
    float get_ori();
    float get_vit();
    float get_vit_canon();
    float get_cadence_tir();
    float get_vitesse_obus();
    sf::Sprite& getBaseSprite();
    sf::Sprite& getTourelleSprite();
    ListeObus& getListeObus();

    // Setters
    void set_x(float new_x);
    void set_y(float new_y);
    void set_ori(float new_ori);
    void set_vit(float new_vit);
    void set_vit_can(float new_vit_canon);
    void set_cadence_tir(float new_cadence);
    void set_vitesse_obus(float new_vitesse_obus);

private:
    float x;
    float y;
    float orientation;
    float vitesse;
    float vitesse_canon;
    float cadence_tir;
    float vitesse_obus;
    ListeObus liste_obus;

    sf::Texture textureBase, textureTourelle;
    sf::Sprite spriteBase, spriteTourelle;
};

#endif
