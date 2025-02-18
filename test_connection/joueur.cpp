#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception

#include "joueur.hpp"

void joueur::recup_TankPos(float x, float y) {
    this->Tank.set_x(x);  
    this->Tank.set_y(y);
}
