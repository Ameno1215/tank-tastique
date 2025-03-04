#include "tankBleu.hpp"



Tank_bleu::Tank_bleu() : tank() {
    setTexture();

    int x = 500;
    int y = 200; 

    spriteBase.setPosition(x, y);
    spriteTourelle.setPosition(x, y);
    
    set_vit(0.02f);
}

void Tank_bleu::setTexture() {
    if (!textureBase.loadFromFile("Image/base_bleu.png")) {
        std::cerr << "Erreur chargement texture Tank Bleu\n";
    }
    spriteBase.setTexture(textureBase);
    spriteBase.setOrigin(spriteBase.getLocalBounds().width / 2, spriteBase.getLocalBounds().height / 2);
    spriteTourelle.setOrigin(spriteTourelle.getLocalBounds().width / 2, spriteTourelle.getLocalBounds().height / 2);

}

std::string Tank_bleu::getType() const {
    return "Tank Bleu";
}

int Tank_bleu::get_type() const {
    return 2;
}
