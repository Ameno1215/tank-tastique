#ifndef LOBBY_H
#define LOBBY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#include "bouton.hpp"

class Lobby{
    public :
        int choix(); 
        Lobby();
        void config();
        int nbJoueurChoisi;
        int mode;

    private:
        sf::RenderWindow window;
        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;
        sf::Font font;
    
        void loadResources(); // Fonction privée pour charger les ressources
};

#endif