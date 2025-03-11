#include "tankSniper.hpp"
#include "partie.hpp"



Tank_sniper::Tank_sniper() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 2);
    set_cadence_tir(MULT_CADENCE_TIR / 1);
    set_degat(3);
    set_porte(MULT_PORTE * 5);
    set_vitesse_obus(MULT_VITESSE_OBUS * 4);
    set_vie(MULT_VIE * 3);
}

void Tank_sniper::setTexture() {
    if (!textureBase.loadFromFile("Image/sniper/base_sniper.png") || !textureTourelle.loadFromFile("Image/sniper/tourelle_sniper.png")) {
        std::cerr << "Erreur chargement texture Tank sniper\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_sniper::getType() const {
    return "Tank sniper";
}

int Tank_sniper::get_type() const {
    return 6;
}