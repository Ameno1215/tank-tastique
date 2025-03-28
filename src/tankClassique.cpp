#include "tankClassique.hpp"
#include "partie.hpp"



Tank_classique::Tank_classique() : tank() {
    setTexture();

    set_vit(MULT_VITESSE_TANK * 3);
    set_cadence_tir(MULT_CADENCE_TIR / 3);
    set_degat(2);
    set_porte(MULT_PORTE * 4);
    set_vitesse_obus(MULT_VITESSE_OBUS * 3);
    set_vie(MULT_VIE * 3);

    setSpriteUlti("Image/classique/ulti.png");
    setSpriteUltiPret("Image/classique/ulti_pret.png");

    getSpriteUlti().setScale(0.5f, 0.5f);
    getSpriteUltiPret().setScale(0.5f, 0.5f);
}

void Tank_classique::setTexture() {
    if (!textureBase.loadFromFile("Image/classique/base_classique.png") || !textureTourelle.loadFromFile("Image/classique/tourelle_classique.png")) {
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