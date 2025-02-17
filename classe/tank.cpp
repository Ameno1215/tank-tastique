#include "tank.hpp"

//accesseurs
int tank::get_x(){
    return x;
}

int tank::get_y(){
    return y;
}

float tank::get_ori(){
    return orientation;
}

float tank::get_vit(){
    return vitesse;
}

// Setters
void tank::set_x(int new_x) {
    x = new_x;  // Modifie la position X du tank
}

void tank::set_y(int new_y) {
    y = new_y;  // Modifie la position Y du tank
}

void tank::set_ori(float new_ori) {
    orientation = new_ori;  // Modifie l'orientation du tank
}

void tank::set_vit(float new_vit) {
    vitesse = new_vit;  // Modifie la vitesse du tank
}