#include "tank.hpp"

//accesseurs
int tank::get_x(){
    return x;
}

int tank::get_y(){
    return y;
}

double tank::get_ori(){
    return orientation;
}

double tank::get_vit(){
    return vitesse;
}

// Setters
void tank::set_x(int new_x) {
    x = new_x;  // Modifie la position X du tank
}

void tank::set_y(int new_y) {
    y = new_y;  // Modifie la position Y du tank
}

void tank::set_ori(double new_ori) {
    orientation = new_ori;  // Modifie l'orientation du tank
}

void tank::set_vit(double new_vit) {
    vitesse = new_vit;  // Modifie la vitesse du tank
}