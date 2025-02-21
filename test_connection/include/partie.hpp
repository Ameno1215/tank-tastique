#ifndef PARTIE_H
#define PARTIE_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <atomic>


#include "joueur.hpp" 
#include "client.hpp"
#define NB_JOUEUR 2

class Partie {
    public:
        Partie(); // Constructeur pour initialiser les variables
        ~Partie(); // Destructeur pour libérer la mémoire

        Joueur joueur[6];
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int get_nbJoueur();

        bool partieComplete() { return nbJoueur >= NB_JOUEUR; }
        int multiJoueur();
        int Solo();
        void getEvent(); 
        void update();
        void renderWindow();

    private:
        bool Zpressed = false;
        bool Spressed = false;        
        bool Qpressed = false;
        bool Dpressed = false;
        sf::Vector2i mousePos;

        sf::RenderWindow* window = nullptr;  // Pointeur pour gérer l'initialisation tardive
        sf::Vector2u windowSize;
        sf::Sprite cursorSprite;
        sf::Texture textureCurseur;

        int nbJoueur;
        int port_actuel;
        int joueur_courant;
};

#endif
