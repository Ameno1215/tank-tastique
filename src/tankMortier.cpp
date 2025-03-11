#include "tankMortier.hpp"
#include "partie.hpp"



Tank_mortier::Tank_mortier() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 3);
    set_cadence_tir(MULT_CADENCE_TIR / 3);
    set_degat(3);
    set_porte(MULT_PORTE * 4);
    set_vitesse_obus(MULT_VITESSE_OBUS * 3);
    set_vie(MULT_VIE * 3);
}

void Tank_mortier::setTexture() {
    if (!textureBase.loadFromFile("Image/mortier/base_mortier.png") || !textureTourelle.loadFromFile("Image/mortier/tourelle_mortier.png")) {
        std::cerr << "Erreur chargement texture Tank mortier\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_mortier::getType() const {
    return "Tank mortier";
}

int Tank_mortier::get_type() const {
    return 4;
}