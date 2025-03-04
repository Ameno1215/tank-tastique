#include "tankVert.hpp"



Tank_vert::Tank_vert() : tank() {
    setTexture();

    set_vit(2.f);
}

void Tank_vert::setTexture() {
    if (!textureBase.loadFromFile("Image/base_vert.png")) {
        std::cerr << "Erreur chargement texture Tank Vert\n";
    }
    spriteBase.setTexture(textureBase);
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_vert::getType() const {
    return "Tank Vert";
}

int Tank_vert::get_type() const {
    return 1;
}