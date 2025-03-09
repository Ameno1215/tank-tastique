#include "tankHealer.hpp"
#include "partie.hpp"



Tank_healer::Tank_healer() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 3);
    set_cadence_tir(MULT_CADENCE_TIR * 3);
    set_degat(2);
    set_porte(MULT_PORTE * 3);
    set_vitesse_obus(MULT_VITESSE_OBUS * 3);
    set_vie(MULT_VIE * 2);
}

void Tank_healer::setTexture() {
    if (!textureBase.loadFromFile("Image/healer/base_healer.png") || !textureTourelle.loadFromFile("Image/healer/tourelle_healer.png")) {
        std::cerr << "Erreur chargement texture Tank healer\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_healer::getType() const {
    return "Tank healer";
}

int Tank_healer::get_type() const {
    return 3;
}