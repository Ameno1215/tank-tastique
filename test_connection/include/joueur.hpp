#ifndef JOUEUR_H
#define JOUEUR_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception

#include "tank.hpp"

class Joueur {
public :
    Joueur(); // Déclaration du constructeur

    // Méthode pour mettre à jour la position du tank
    void recup_TankPos(float x, float y);

    // Membres de la classe
    tank Tank;
    int port;
    int id;
    int pV = 5;
    int pts = 0;
    std::string pseudo;
    bool Zpressed = false;
    bool Spressed = false;        
    bool Qpressed = false;
    bool Dpressed = false;
    bool Tabpressed = false;
    sf::Vector2i mousePos;
    sf::Vector2f worldMousePos;
};

#endif
