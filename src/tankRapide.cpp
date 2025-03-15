#include "tankRapide.hpp"
#include "partie.hpp"



Tank_rapide::Tank_rapide() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 5);
    set_cadence_tir(MULT_CADENCE_TIR / 5);
    set_degat(1);
    set_porte(MULT_PORTE * 2);
    set_vitesse_obus(MULT_VITESSE_OBUS * 5);
    set_vie(MULT_VIE * 1);

    spriteTourelle.setScale(0.11f, 0.11f);

    setSpriteUlti("Image/petit/ulti.png");
    setSpriteUltiPret("Image/petit/ulti_pret.png");

    getSpriteUlti().setScale(0.05f, 0.05f);
    getSpriteUltiPret().setScale(0.05f, 0.05f);
}

void Tank_rapide::setTexture() {
    if (!textureBase.loadFromFile("Image/petit/base_petit.png") || !textureTourelle.loadFromFile("Image/petit/tourelle_petit.png")) {
        std::cerr << "Erreur chargement texture Tank rapide\n";
    }
    spriteBase.setTexture(textureBase);
    spriteTourelle.setTexture(textureTourelle);

    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_rapide::getType() const {
    return "Tank rapide";
}

int Tank_rapide::get_type() const {
    return 2;
}