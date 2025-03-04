#include "tankBlanc.hpp"



Tank_blanc::Tank_blanc() : tank() {
    setTexture();

    int x = 500;
    int y = 200; 

    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);
    
    set_vit(0.02f);
}

void Tank_blanc::setTexture() {
    if (!textureBase.loadFromFile("Image/base_blanc.png")) {
        std::cerr << "Erreur chargement texture Tank Blanc\n";
    }
    spriteBase.setTexture(textureBase);
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_blanc::getType() const {
    return "Tank Blanc";
}

int Tank_blanc::get_type() const {
    return 0;
}
