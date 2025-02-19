#ifndef PARTIE_H
#define PARTIE_H

#include "joueur.hpp" 
#include "client.hpp"
#define NB_JOUEUR 2

class Partie {
    public:
        joueur Joueur[6];

        Partie(); // Constructeur pour initialiser les variables
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        bool partieComplete() { return nbJoueur >= NB_JOUEUR; }
        int multiJoueur();
        int Solo();
        
    private:
        int nbJoueur;
        int port_actuel;
        int joueur_courant;
};

#endif
