#include "tank.hpp"
#include <iostream>

// Constructeur
tank::tank() {
    // Chargement des textures
    if (!textureBase.loadFromFile("../../jeu/Image/base1.png") || 
        !textureTourelle.loadFromFile("../../jeu/Image/tourelle2.png")) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
    }

    // Assignation des textures aux sprites
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    // Définir l'origine des sprites au centre
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

    // Positionnement initial
    x = 300;
    y = 200;
    orientation = 0;
    vitesse = 0.2f;
    vitesse_canon = 0.1f; // Valeur par défaut

    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);

    // Redimensionnement
    spriteBase.setScale(0.1f, 0.1f);
    spriteTourelle.setScale(0.1f, 0.1f);
}

// Accesseurs
float tank::get_x() { return x; }
float tank::get_y() { return y; }
float tank::get_ori() { return orientation; }
float tank::get_vit() { return vitesse; }
float tank::get_vit_canon() { return vitesse_canon; }
sf::Sprite& tank::getBaseSprite() { return spriteBase; }
sf::Sprite& tank::getTourelleSprite() { return spriteTourelle; }

// Setters
void tank::set_x(float new_x) { 
    x = new_x; 
    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);
}

void tank::set_y(float new_y) { 
    y = new_y; 
    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);
}

void tank::set_ori(float new_ori) { 
    orientation = new_ori; 
    spriteBase.setRotation(orientation);
}

void tank::set_vit(float new_vit) { vitesse = new_vit; }

void tank::set_vit_can(float new_vit_canon) { vitesse_canon = new_vit_canon; }
