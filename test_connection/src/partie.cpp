#include "partie.hpp"

Partie::Partie() {
    this->nbJoueur = 0;
    this->joueur_courant = 0;
    this->port_actuel = 0;
}

// Fonction pour ajouter un joueur jusqu'à un maximum de 6
bool Partie::ajouteJoueur() {
    if (nbJoueur < 6) {
        Joueur[nbJoueur].portJoueur = 3001 + nbJoueur; // Attribution du port
        port_actuel = 3001 + nbJoueur;
        nbJoueur++;
        return true; // Ajout réussi
    } else {
        std::cout << "Nombre maximal de joueurs atteint !" << std::endl;
        return false; // Échec de l'ajout
    }
}

int Partie::get_portactuel(){
    return port_actuel;
}

int Partie::multiJoueur(){
    return 2;
}

int Partie::Solo(){
    return 1;
}
