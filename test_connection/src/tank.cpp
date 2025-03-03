#include "tank.hpp"
#include <iostream>

// Constructeur
tank::tank() {

    // Chargement des textures
    if (!textureBase.loadFromFile("Image/base1.png") || 
        !textureTourelle.loadFromFile("Image/tourelle2.png")) {
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
    vitesse = 1.2f;
    vitesse_canon = 1.1f; // Valeur par défaut
    cadence_tir = 0.8;
    vitesse_obus = 1;

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
float tank::get_vitesse_obus() { return vitesse_obus; }
sf::Sprite& tank::getBaseSprite() { return spriteBase; }
sf::Sprite& tank::getTourelleSprite() { return spriteTourelle; }
float tank::get_cadence_tir() { return cadence_tir; }
ListeObus& tank::getListeObus() { return liste_obus; }
bool tank::isColliding() const { return collision; }     // Retourne l'état de collision
bool tank::isTouched(){ return touched;}// Retourne l'état de collision

// Mise à jour de la hitbox du tank
void tank::updateHitbox() { 
    tankHitbox = getTransformedPoints(getBaseSprite());

    /* pour afficher les points
    std::cout<<"debut hitbox\n";
    for (const auto& p : tankHitbox) {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }
    std::cout<<"fin hitbox\n";
    */
    // Trouver les bornes min et max
    int initBornes = 0;       //pour initialiser les bornes
    for (const auto& point : tankHitbox) {
        if(initBornes == 0){
            bornesHitBox[0] = point.x;
            bornesHitBox[1] = point.x;
            bornesHitBox[2] = point.y;
            bornesHitBox[3] = point.y;
            initBornes = 1;
        }
        if (point.x < bornesHitBox[0]) bornesHitBox[0] = point.x;
        if (point.x > bornesHitBox[1]) bornesHitBox[1] = point.x;
        if (point.y < bornesHitBox[2]) bornesHitBox[2] = point.y;
        if (point.y > bornesHitBox[3]) bornesHitBox[3] = point.y;
    }
}

// Mise à jour de la collision avec un autre sprite
void tank::updateCollision(const sf::Sprite& otherSprite) {
    collision = false; // Réinitialisation

    for (const auto& point : tankHitbox) {
        if (otherSprite.getGlobalBounds().contains(point)) {
            collision = true;
            break;
        }
    }
}


void tank::updateTouched(const sf::Sprite& otherSprite) {      // ca marche bof detecte mal
    touched = false; 

    for (const auto& point : tankHitbox) {
        if (otherSprite.getGlobalBounds().contains(point)) {
            std::cout<<"touché";
            touched = true;
            return;
        }
    }
}

std::vector<sf::Vector2f> tank::getTransformedPoints(const sf::Sprite& sprite) {
    std::vector<sf::Vector2f> points;

    // Récupérer les dimensions locales du sprite
    sf::FloatRect localBounds = sprite.getLocalBounds();
    
    // Nombre de points d’échantillonnage
    int samplesX = 10;  // Plus = plus précis
    int samplesY = 10;

    // Calcul du pas entre les points
    float stepX = localBounds.width / (samplesX - 1);
    float stepY = localBounds.height / (samplesY - 1);

    // Générer une grille de points transformés
    for (int i = 0; i < samplesX; i++) {
        for (int j = 0; j < samplesY; j++) {
            sf::Vector2f localPoint(localBounds.left + i * stepX, localBounds.top + j * stepY);
            sf::Vector2f globalPoint = sprite.getTransform().transformPoint(localPoint);
            points.push_back(globalPoint);
        }
    }

    return points;
}

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

void tank::set_cadence_tir(float new_cadence) { cadence_tir = new_cadence; }

void tank::set_vitesse_obus(float new_vitesse_obus) { vitesse_obus = new_vitesse_obus; }