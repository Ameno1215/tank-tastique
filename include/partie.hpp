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
#include <fstream>
#include <signal.h>

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
#define TEST 1      // 1 -> automatiquement en localHost, 0 -> choix ip/pseudo

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

        float stat[6][4]; //dans l'ordre des colonnes : PV, obus tiré, obus touchés, dégats infligés.
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int get_nbJoueur();
        std::string& get_buffer_missile();

        int get_go();
        void set_go(int val);
        
        int get_coeur_ajouter();
        void set_coeur_ajouter(int val);

        void setBufferMissile(const std::string& newBuffer);

        sf::Sprite& getpvSprite();
        bool partieComplete() { return nbJoueur >= NB_JOUEUR; }
        void afficheTableauScore(int fin);
        void afficherStatJoueur(int i, float startX, float columnSpacing, float &currentY, float rowSpacing);

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
        std::atomic<bool> partieFinie {false};

        sf::Sprite fondSprite;
        sf::Texture fondTexture;
        sf::FloatRect backgroundBounds;

        void afficherMinimap();

        int utltiActive[6] = {0, 0, 0, 0, 0, 0};

        std::vector<std::vector<sf::Vector2f>> hitboxes;

        std::vector<sf::Sprite> mursSprites;
        std::vector<sf::Texture> mursTextures;
        int laissePasserObus[20];              //mettre à un pour laisser passer les obus

        std::vector<sf::Sprite> regenSprites;
        sf::Texture regenTextures;

        sf::Sprite bonus;
        bool ultiClassicUse = false;
        void updateRegen();
        int regen[4][5];

        int finDePartie();

        // Liste de positions spécifiques
        std::vector<sf::Vector2f> positions = {
            {50, 50}, {200, 100}, {350, 150}, {500, 200}, {650, 250},
            {100, 300}, {250, 350}, {400, 400}, {550, 450}, {700, 500}, {150, 550}
        };

        void set_nbJoueur(int i);
        void recup_equip();
        int testEquipeGagnant();


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
        int coeurAjouter = 0;

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
