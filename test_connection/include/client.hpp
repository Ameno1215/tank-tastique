#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT_CONNEXION 3000

#include "bouton.hpp" 
#include "joueur.hpp"

class Client{
    public :
        Client();
        void sendMessageToServer(const std::string& message, int port);     // Fonction pour envoyer un message UDP au serveur
        void sendPosToServer(float position[2]); 
        void sendUDPdata(sf::Event event, Bouton button2);
        void initconnexion();
    private :
        int num_port;
        int sockfd;
        struct sockaddr_in servaddr, cliaddr;
};
#endif