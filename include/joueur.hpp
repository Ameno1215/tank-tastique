#ifndef JOUEUR_H
#define JOUEUR_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tank.hpp"
#include "tank.hpp"
#include "tankVert.hpp" 

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception


class Joueur {
public :
    Joueur(); // Déclaration du constructeur

    // Méthode pour mettre à jour la position du tank
    void recup_TankPos(float x, float y);
    void afficherTypeTank() const;


    void setTank(std::unique_ptr<tank> newTank);

    // Membres de la classe
    std::unique_ptr<tank> Tank;
    int port;
    int id;
    int pV = 5;
    int pts = 0;
    std::string pseudo;
    bool Zpressed = false;
    bool Spressed = false;        
    bool Qpressed = false;
    bool Dpressed = false;
    bool Clicked = false;
    bool Tabpressed = false;
    sf::Vector2i mousePos;
    sf::Vector2f worldMousePos;
    bool vivant = true;
};

#endif
