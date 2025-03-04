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
#include <sstream>  
#include <atomic>

#include "joueur.hpp" 
#include "bouton.hpp"
#include "client.hpp"
#include "tankBleu.hpp"
#include "tankVert.hpp"
#include "tankBlanc.hpp"
#include "deplacement.hpp"
#define NB_JOUEUR 2

class Partie {
    public:
        int go = 0;
        Partie(); // Constructeur pour initialiser les variables
        ~Partie(); // Destructeur pour libérer la mémoire

        Joueur joueur[6];
        Client client;

        int stat[6][4]; //1ere colonne nb de dégats infligés, nb d'obus recu, nb d'obus tirés, nb de char détruit.
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int get_nbJoueur();
        std::string& get_buffer_missile();

        void setBufferMissile(const std::string& newBuffer);

        sf::Sprite& getpvSprite();
        bool partieComplete() { return nbJoueur >= NB_JOUEUR; }
        void afficheTableauScore();
        void renderExplosion(int x, int y);

        sf::Sprite testSprite;
        
        //méthode que pour solo
        int Solo();
        
        //methode pour Solo/Multi
        void getEvent(); 
        void update();
        void renderWindow(int multi);
        
        //methodes que pour multi
        int multiJoueur();
        void affichageConnexion();
        void sendData();
        void sendTank(int type);
        void recieveData();
        void recieveTank();
        void sendReceptionTank();
        void affichageAttenteTank();

        // retourne le nombre d'obus actif dans la partie
        int nb_obus();
        
        // rempli chaine (string) avec tous les obus (joueur, x, y, orientation) 
        void string_obus(std::string& chaine);

        int selectionTank();
        void affiche_type_tank();

        int joueur_courant;

        ListeExplosion listexplosion;

    private:
        sf::RenderWindow* window = nullptr;  // Pointeur pour gérer l'initialisation tardive
        sf::Vector2u windowSize;
        sf::Sprite cursorSprite;
        sf::Texture textureCurseur;
        std::string buffer_missile;
        sf::Sprite pvSprite;
        sf::Texture pvTexture;

        int nbJoueur;
        int port_actuel;
        bool serverPret;

        bool explosionActive = false;
        int currentFrameExplo = 0;
        sf::Clock explosionClock;
        sf::Texture explosionTexture;
        sf::Texture explosionTextureFrames[20];
        sf::Sprite explosionSprite;
};

#endif
