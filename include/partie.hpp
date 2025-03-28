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
#define TEST 0    // 1 -> automatiquement en localHost, 0 -> choix ip/pseudo

#define MULT_VITESSE_TANK 0.3
#define MULT_CADENCE_TIR 2.3
#define MULT_PORTE 150
#define MULT_VITESSE_OBUS 0.6
#define MULT_VIE 2



class Partie {
    public:

        //constructeur
        Partie(); 
        ~Partie(); 

        //Fonctions Majeures
        int multiJoueur();
        void getEvent(); 
        void update();
        void renderWindow(int multi);
        int finDePartie();
        
        //init
        Joueur joueur[6];
        Client client;
        void initialiserGameOverUI();
        void set_nbJoueur(int i);
        void initialiserpseudo();
        void set_coeur_ajouter(int val);
        void set_go(int val);
        void setBufferMissile(const std::string& newBuffer);
        
        //affichage
        void affichageAttenteTank();
        void affichageConnexion();
        void renderExplosion(int x, int y);
        void afficheTableauScore(int fin);
        void afficherStatJoueur(int i, float startX, float columnSpacing, float &currentY, float rowSpacing);
        void affiche_type_tank();
        void afficherMinimap();
        sf::Sprite& getpvSprite();
        sf::Sprite fondSprite;
        sf::Texture fondTexture;
        sf::Texture regenTextures;
        sf::View defaultView;

        //Calcul
        void string_obus(std::string& chaine); // rempli chaine (string) avec tous les obus (joueur, x, y, orientation) 
        int selectionTank();
        int nb_obus();  // retourne le nombre d'obus actif dans la partie
        void updateRegen();
        bool ajouteJoueur(); // Fonction pour ajouter un joueur
        int get_portactuel();
        int get_nbJoueur();
        void recup_equip();
        std::string& get_buffer_missile();
        int get_go();
        int get_coeur_ajouter();
        float stat[6][4]; //dans l'ordre des colonnes : PV, obus tiré, obus touchés, dégats infligés.
        int utltiActive[6] = {0, 0, 0, 0, 0, 0};
        int joueur_courant;
        ListeExplosion listexplosion;
        sf::FloatRect backgroundBounds;
        std::vector<std::vector<sf::Vector2f>> hitboxes;
        std::vector<sf::Sprite> mursSprites;
        int laissePasserObus[20];              //mettre à un pour laisser passer les obus
        std::vector<sf::Sprite> regenSprites;
        sf::Sprite bonus;
        std::vector<sf::Vector2f> positions = {
            {245, 320}, {240, 170}, {710, 660}, {605, 1240}, {2, 1215},
            {1460,2370}, {2135, 2360}, {1780, 2}, {2810, 250}, {2805, 620}, {3162, 620},{3480, 620},
            {2665, 775}, {2535, 1230}, {2665, 1665}, {1815, 1880},

            {1310, 900}, {2355, 900}, {1570, 670}, {1570, 1720}, {2260, 1039},
            {2260,1336}, {1483, 1039}, {1483, 1336}, {1700, 1625},{1995, 1625},{1700, 820}, {1997, 820},{1768, 1120}, {335, 1835}
        };
        std::vector<sf::Vector2f> scale = {
            {2.2f, 2.2f}, {2.2f, 2.2f}, {2.05f, 2.05f}, {2.f, 2.f}, {2.f,2.f},
            {2.f, 2.f}, {2.f, 2.f}, {2.f, 2.f}, {2.f, 2.f}, {2.f, 2.f}, {2.f, 2.f},{2.f, 2.f},
            {2.2f, 2.1f}, {2.2, 2.1f}, {2.05f, 2.05f}, {2.05f, 2.05f},

            {2.2f, 2.1f}, {2.2, 2.1f}, {2.05f, 2.05f}, {2.05f, 2.05f}, {2.05f,2.05f},
            {2.05f,2.05f},{2.05f,2.05f},{2.05f,2.05f},{2.1f,2.3f},{2.1f,2.3f},{2.1f,2.1f},{2.1f,2.1f},{2.1f,2.1f},{2.1f,2.1f}
        };
        bool ultiClassicUse = false;
        int regen[4][5];

        //Test
        int testGagnant();
        std::atomic<bool> partieFinie {false};
        int testEquipeGagnant();
        bool partieComplete() { return nbJoueur >= NB_JOUEUR; }

        //Reseau
        void sendData();
        void sendTank(int type);
        void recieveData();
        void recieveTank();
        int waitOthertank();

    private:
        std::vector<sf::Texture> mursTextures;
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
        int nbchoix = -1;
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
