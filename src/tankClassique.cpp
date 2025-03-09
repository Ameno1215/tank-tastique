#include "tankClassique.hpp"
#include "partie.hpp"



Tank_classique::Tank_classique() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 3);
    set_cadence_tir(MULT_CADENCE_TIR * 3);
    set_degat(3);
    set_porte(MULT_PORTE * 4);
    set_vitesse_obus(MULT_VITESSE_OBUS * 3);
    set_vie(MULT_VIE * 3);
}

void Tank_classique::setTexture() {
    if (!textureBase.loadFromFile("Image/base_classique.png") || !textureTourelle.loadFromFile("Image/tourelle_classique.png")) {
        std::cerr << "Erreur chargement texture Tank Classique\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_classique::getType() const {
    return "Tank classique";
}

int Tank_classique::get_type() const {
    return 1;
}