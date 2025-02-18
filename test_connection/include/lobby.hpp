#ifndef LOBBY_H
#define LOBBY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Lobby{
    public :
        int choix(); 
};

#endif