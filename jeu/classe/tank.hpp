#ifndef TANK_H
#define TANK_H

#include <SFML/Graphics.hpp>
#include "../config/config.hpp"

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
    sf::Sprite& getBaseSprite();
    sf::Sprite& getTourelleSprite();

    // Setters
    void set_x(float new_x);
    void set_y(float new_y);
    void set_ori(float new_ori);
    void set_vit(float new_vit);
    void set_vit_can(float new_vit_canon);

private:
    float x;
    float y;
    float orientation;
    float vitesse;
    float vitesse_canon;

    sf::Texture textureBase, textureTourelle;
    sf::Sprite spriteBase, spriteTourelle;
};

#endif
