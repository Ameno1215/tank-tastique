#include "partie.hpp"
#include "client.hpp"

Partie::Partie() {
    this->nbJoueur = 0;
    this->joueur_courant = 0;
    this->port_actuel = 0;
}

// Fonction pour ajouter un joueur jusqu'à un maximum de 6
bool Partie::ajouteJoueur() {
    if (nbJoueur < 6) {
        port_actuel = 3001 + nbJoueur;
        Joueur[nbJoueur].portJoueur = port_actuel;
        nbJoueur++;
        return true;
    } else {
        std::cout << "Nombre maximal de joueurs atteint !" << std::endl;
        return false;
    }
}


int Partie::get_portactuel(){
    return port_actuel;
}

int Partie::multiJoueur(){
    Client client;
    client.initconnexion();
    return 2;
}

int Partie::Solo(){
    return 1;
}
