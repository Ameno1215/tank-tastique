#include "tir.hpp"


Obus::Obus(int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture) 
    : x(x), y(y), orientation(orientation), vitesse(vitesse), porte(porte) 
{
    set_texture(nomTexture);
    sf::Sprite spriteObus(texture);
    spriteObus.setPosition(x, y);
}

//accesseurs
int Obus::get_x(){
    return x;
}

int Obus::get_y(){
    return y;
}

float Obus::get_ori(){
    return orientation;
}

float Obus::get_vit(){
    return vitesse;
}

int Obus::get_porte(){
    return porte;
}

const sf::Texture& Obus::get_texture() {
    return texture;
}

sf::Sprite& Obus::get_Sprite() {
    return spriteObus;
}





// Setters
void Obus::set_x(int new_x) {
    x = new_x;  // Modifie la position X du tank
}

void Obus::set_y(int new_y) {
    y = new_y;  // Modifie la position Y du tank
}

void Obus::set_ori(float new_ori) {
    orientation = new_ori;  // Modifie l'orientation du tank
}

void Obus::set_vit(float new_vit) {
    vitesse = new_vit;  // Modifie la vitesse du tank
}

void Obus::set_porte(int new_porte) {
    porte = new_porte;
}

void Obus::set_texture(const std::string& nom) {
    if (!texture.loadFromFile(nom)) {
        std::cerr << "Erreur : Impossible de charger la texture " << nom << std::endl;
    }
}




