#include "joueur.hpp"

// Définition du constructeur
Joueur::Joueur() { // Initialisation des membres
    // Initialisation explicite de chaque membre à 0 ou à des valeurs par défaut
    this->id = 0;        // Identifiant du joueur
    this->pseudo = "Anto";   // Pseudo vide
    this->port = SERVER_PORT;      // Port à 0 par défaut
    this->Tank = tank();
    std::cout << "tank crée (normalement)";
    Tank.set_x(0.f);     // Position du tank à (0, 0)
    Tank.set_y(0.f);     // Position du tank à (0, 0)
}

// Définition de la méthode recup_TankPos
void Joueur::recup_TankPos(float x, float y) {
    Tank.set_x(x);
    Tank.set_y(y);
}