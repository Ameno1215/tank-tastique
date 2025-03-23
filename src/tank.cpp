#include "tank.hpp"
#include <iostream>

// Constructeur
tank::tank() {
    // Positionnement initial
    x = 300;
    y = 200;
    orientation = 0;

    set_vit(VITESSE_TANK);
    set_vit_can(VITESSE_CANON);
    set_vitesse_obus(VITESSE_OBUS);
    set_cadence_tir(CADENCE);
    set_porte(PORTE);
    set_degat(DEGAT);
    set_vie(VIE);

    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);

    // Redimensionnement
    spriteBase.setScale(0.1f, 0.1f);
    spriteTourelle.setScale(0.1f, 0.1f);
}

tank::tank(int x_init, int y_init, float orientation_init, float vitesse_tank, float vitesse_canon, float cadence_tir, float vitesse_obus, std::string chemin_img_tank, std::string chemin_img_tourelle) {
    // Chargement des textures
    if (!textureBase.loadFromFile(chemin_img_tank) || 
        !textureTourelle.loadFromFile(chemin_img_tourelle)) {
        std::cerr << "Erreur de chargement des textures !" << std::endl;
    }

    // Assignation des textures aux sprites
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    // Définir l'origine des sprites au centre
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

    orientation = orientation_init;
    vitesse = vitesse_tank;
    vitesse_canon = vitesse_canon;
    cadence_tir = cadence_tir;
    vitesse_obus = vitesse_obus;

    spriteBase.setPosition(x_init, y_init);
    spriteTourelle.setPosition(x_init, y_init);

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
int tank::get_vie() const { return vie; }
sf::Sprite& tank::getBaseSprite() { return spriteBase; }
sf::Sprite& tank::getTourelleSprite() { return spriteTourelle; }
float tank::get_cadence_tir() { return cadence_tir; }
ListeObus& tank::getListeObus() { return liste_obus; }
bool tank::isColliding() const { return collision; }     // Retourne l'état de collision
bool tank::isTouched(){ return touched;}// Retourne l'état de collision
int tank::get_porte() const { return porte; }
int tank::get_degat() const { return degat; }
std::vector<sf::Vector2f>  tank::get_tankHitbox(){ return tankHitbox; }


sf::Sprite& tank::getSpriteUltiPret() {
    return spriteUltiPret;
}

sf::Sprite& tank::getSpriteUlti() {
    return spriteUlti;
}

void tank::setSpriteUltiPret(const std::string& texturePath) {
    if (!textureUltiPret.loadFromFile(texturePath)) {
        std::cerr << "Erreur : Impossible de charger la texture " << texturePath << std::endl;
        return;
    }
    spriteUltiPret.setTexture(textureUltiPret);
}

void tank::setSpriteUlti(const std::string& texturePath) {
    if (!textureUlti.loadFromFile(texturePath)) {
        std::cerr << "Erreur : Impossible de charger la texture " << texturePath << std::endl;
        return;
    }
    spriteUlti.setTexture(textureUlti);
}


void tank::set_porte(int new_porte) {
    porte = new_porte;
}

void tank::set_degat(int new_degat) {
    degat = new_degat;
}

void tank::set_vie(int new_vie) {
    vie = new_vie;
}

// Mise à jour de la hitbox du tank
void tank::updateHitbox() { 
    tankHitbox = getTransformedPoints(getBaseSprite());
}

// Mise à jour de la collision avec un autre sprite
void tank::updateCollision(std::vector<std::vector<sf::Vector2f>> hitboxes, sf::FloatRect backgroundBounds, int id, std::vector<sf::Sprite> mursSprites){
    collision = false; // Réinitialisation

    for (const auto& point : tankHitbox) {
        if (point.x < backgroundBounds.left || 
            point.x > backgroundBounds.left + backgroundBounds.width || 
            point.y < backgroundBounds.top || 
            point.y > backgroundBounds.top + backgroundBounds.height) {
            collision = true; // Le tank est sorti du background
            return;
        }
        else{
            for (int i=0;i<12;i++){
                if (mursSprites[i].getGlobalBounds().contains(point)) {
                    collision = true;
                    return;
                }
            }
        }
    }
    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (i != static_cast<size_t>(id)) { // Conversion de `id` en `size_t`
            if (hitboxes[i].empty()){
                //sauter
            } 
            else{
                collisionTank(tankHitbox, hitboxes[i]);
                if (collision) { //collision à true/false
                    std::cout<<"touché"<<std::endl;
                    return;
                }
            }
        }
    }
}

int tank::updateRegenCollision(std::vector<sf::Sprite> regenSprite){
    for (const auto& point : tankHitbox) {
        for (int i=0;i<4;i++){
            if (regenSprite[i].getGlobalBounds().contains(point)) {
                return i;
            }
        }
    }
    return -1;
}

void tank::collisionTank(const std::vector<sf::Vector2f>& hitbox1, const std::vector<sf::Vector2f>& hitbox2){
    float hitbox1Left = hitbox1[0].x;
    float hitbox1Right = hitbox1[0].x;
    float hitbox1Top = hitbox1[0].y;
    float hitbox1Bottom = hitbox1[0].y;

    for (const auto& point : hitbox1) {
        hitbox1Left = std::min(hitbox1Left, point.x);
        hitbox1Right = std::max(hitbox1Right, point.x);
        hitbox1Top = std::min(hitbox1Top, point.y);
        hitbox1Bottom = std::max(hitbox1Bottom, point.y);
    }

    float hitbox2Left = hitbox2[0].x;
    float hitbox2Right = hitbox2[0].x;
    float hitbox2Top = hitbox2[0].y;
    float hitbox2Bottom = hitbox2[0].y;

    for (const auto& point : hitbox2) {
        hitbox2Left = std::min(hitbox2Left, point.x);
        hitbox2Right = std::max(hitbox2Right, point.x);
        hitbox2Top = std::min(hitbox2Top, point.y);
        hitbox2Bottom = std::max(hitbox2Bottom, point.y);
    }

    if (hitbox1Right >= hitbox2Left && 
        hitbox1Left <= hitbox2Right && 
        hitbox1Bottom >= hitbox2Top && 
        hitbox1Top <= hitbox2Bottom) {
        collision = true; // Les hitbox se touchent
    } else {
        collision = false; // Pas de collision
    }
}


void tank::updateTouched(const sf::Sprite& otherSprite) {
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

    sf::FloatRect localBounds = sprite.getLocalBounds();
    
    // Nombre de points d’échantillonnage
    int samplesX = 10;  // Plus = plus précis
    int samplesY = 10;

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



