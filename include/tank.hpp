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
    tank(int x_init, int y_init, float orientation_init, float vitesse_tank, float vitesse_canon, float cadence_tir, float vitesse_obus, std::string chemin_img_tank, std::string chemin_img_tourelle);
    virtual ~tank() = default;
    
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
    virtual std::string getType() const = 0;
    virtual int get_type() const = 0;

    void updateHitbox(); // Met à jour la hitbox
    void updateCollision(const sf::Sprite& otherSprite, sf::FloatRect backgroundBounds); // Met à jour l'état de collision
    bool isColliding() const; // Retourne l'état de collision
    bool isTouched(); // Retourne l'état de collision
    std::vector<sf::Vector2f> getTransformedPoints(const sf::Sprite& sprite);
    void updateTouched(const sf::Sprite& otherSprite);

    // Setters
    void set_x(float new_x);
    void set_y(float new_y);
    void set_ori(float new_ori);
    void set_vit(float new_vit);
    void set_vit_can(float new_vit_canon);
    void set_cadence_tir(float new_cadence);
    void set_vitesse_obus(float new_vitesse_obus);
    virtual void setTexture() = 0;

protected:
    float x;
    float y;
    float orientation;
    float vitesse;
    float vitesse_canon;
    float cadence_tir;
    float vitesse_obus;
    ListeObus liste_obus;
    int type;

    bool collision = false;
    bool touched;
    std::vector<sf::Vector2f> tankHitbox;
    int bornesHitBox[4];
    sf::ConvexShape rectangleHitBox;


    sf::Texture textureBase, textureTourelle;
    sf::Sprite spriteBase, spriteTourelle;
};



#endif
