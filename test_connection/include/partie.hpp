#ifndef PARTIE_H
#define PARTIE_H

#include "joueur.hpp" 

class Partie {
    public:
        int nbJoueur;
        joueur Joueur[6];

        Partie(); // Constructeur pour initialiser les variables
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int multiJoueur();
        int Solo();
        
    private:
        int joueur_courant;
        int port_actuel;
};

#endif
