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
#include <iomanip> 

#include "joueur.hpp" 
#include "bouton.hpp"
#include "client.hpp"
#include "tankBleu.hpp"
#include "tankClassique.hpp"
#include "tankBlanc.hpp"
#include "tankSolide.hpp"
#include "tankHealer.hpp"
#include "tankRapide.hpp"
#include "tankSniper.hpp"
#include "tankMortier.hpp"
#include "deplacement.hpp"


#define NB_JOUEUR 2       //defini le nb de joueur
#define TEST 1          // 1 -> automatiquement en localHost, 0 -> choix ip/pseudo

#define MULT_VITESSE_TANK 0.3
#define MULT_CADENCE_TIR 1.5
#define MULT_PORTE 80
#define MULT_VITESSE_OBUS 0.4
#define MULT_VIE 2



class Partie {
    public:
        Partie(); // Constructeur pour initialiser les variables
        ~Partie(); // Destructeur pour libérer la mémoire

        Joueur joueur[6];
        Client client;

        float stat[6][4]; //dans l'ordre des colonnes : PV, obus tiré, obus tocuhés, dégats infligés.
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int get_nbJoueur();
        std::string& get_buffer_missile();

        int get_go();
        void set_go(int val);

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
        void initialiserpseudo();

        int waitOthertank();

        // retourne le nombre d'obus actif dans la partie
        int nb_obus();
        
        // rempli chaine (string) avec tous les obus (joueur, x, y, orientation) 
        void string_obus(std::string& chaine);

        int selectionTank();
        void affiche_type_tank();

        int joueur_courant;

        ListeExplosion listexplosion;

        void initialiserGameOverUI();

        int testGagnant();
        std::atomic<bool> partieFinie {false};  // ✅ Initialisation correcte dans la classe

        sf::Sprite fondSprite;
        sf::Texture fondTexture;
        sf::FloatRect backgroundBounds;

        void afficherMinimap();
        
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

        int go = 0;
        int nbchoix;

        bool explosionActive = false;
        int currentFrameExplo = 0;
        sf::Clock explosionClock;
        sf::Texture explosionTexture;
        sf::Texture explosionTextureFrames[20];
        sf::Sprite explosionSprite;

        sf::View view;
        sf::Font font;
        sf::Text gameOverText;
        Bouton boutonScore;
        Bouton boutonReplay;
        bool visionnage = false;

        sf::RectangleShape minimapBackground;
        sf::CircleShape tankPoint;
};

#endif
