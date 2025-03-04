#include "deplacement.hpp"

void deplacement_verticale(tank& mon_tank, float rotation, float speed) {
    // Calcul du déplacement en utilisant directement un vecteur
    sf::Vector2f deltaMove;
    deltaMove.x = -speed * sin(rotation * M_PI / 180);
    deltaMove.y = speed * cos(rotation * M_PI / 180);

    // Mise à jour des coordonnées du tank
    float new_x = mon_tank.get_x() + deltaMove.x;
    float new_y = mon_tank.get_y() + deltaMove.y;
    mon_tank.set_x(new_x);
    mon_tank.set_y(new_y);

}

void deplacement_rotation(tank& mon_tank, float* rotation, float indentation) {
    *rotation += indentation;
    mon_tank.set_ori(*rotation);
}
