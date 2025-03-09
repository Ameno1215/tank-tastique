#include "joueur.hpp"

// Définition du constructeur
Joueur::Joueur() { // Initialisation des membres
    // Initialisation explicite de chaque membre à 0 ou à des valeurs par défaut
    this->id = 0;        // Identifiant du joueur
    this->pseudo = "Anto";   // Pseudo vide
    this->port = SERVER_PORT;      // Port à 0 par défaut
    setTank(std::make_unique<Tank_classique>());
}

// Définition de la méthode recup_TankPos
void Joueur::recup_TankPos(float x, float y) {
    Tank->set_x(x);
    Tank->set_y(y);
}

void Joueur::setTank(std::unique_ptr<tank> newTank) {
    Tank = std::move(newTank); 
}

void Joueur::afficherTypeTank() const {
    if (Tank) {
        std::cout << "Type de tank : " << Tank->getType() << std::endl;
    } else {
        std::cout << "Aucun tank assigné." << std::endl;
    }
}