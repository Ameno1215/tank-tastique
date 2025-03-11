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



#define VITESSE_TANK 1.2f
#define VITESSE_CANON 1.1f
#define VITESSE_OBUS 1
#define DEGAT 1
#define PORTE 500
#define CADENCE 1
#define VIE 5


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
    int get_porte() const;
    int get_degat() const;
    int get_vie() const;

    void updateHitbox(); // Met à jour la hitbox
    void updateCollision(const sf::Sprite& otherSprite); // Met à jour l'état de collision
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
    void set_porte(int new_porte);
    void set_degat(int new_degat);
    void set_vie(int new_vie);

protected:
    float x;
    float y;
    float orientation;
    float vitesse;
    float vitesse_canon;
    float cadence_tir;
    float vitesse_obus;
    int porte;
    ListeObus liste_obus;
    int type;
    int degat;
    int vie;

    bool collision = false;
    bool touched;
    std::vector<sf::Vector2f> tankHitbox;
    int bornesHitBox[4];
    sf::ConvexShape rectangleHitBox;


    sf::Texture textureBase, textureTourelle;
    sf::Sprite spriteBase, spriteTourelle;
};



#endif
