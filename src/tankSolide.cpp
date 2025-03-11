#include "tankSolide.hpp"
#include "partie.hpp"



Tank_solide::Tank_solide() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 1);
    set_cadence_tir(MULT_CADENCE_TIR / 1);
    set_degat(4);
    set_porte(MULT_PORTE * 2);
    set_vitesse_obus(MULT_VITESSE_OBUS * 2);
    set_vie(MULT_VIE * 4);
}

void Tank_solide::setTexture() {
    if (!textureBase.loadFromFile("Image/solide/base_solide.png") || !textureTourelle.loadFromFile("Image/solide/tourelle_solide.png")) {
        std::cerr << "Erreur chargement texture Tank solide\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_solide::getType() const {
    return "Tank solide";
}

int Tank_solide::get_type() const {
    return 5;
}