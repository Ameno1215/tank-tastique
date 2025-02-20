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

        Joueur joueur;
        int num_port;

        Client();
        void sendMessageToServer(const std::string& message);     // Fonction pour envoyer un message UDP au serveur
        void initconnexion();
        void sendData();
        void udpdateData(Joueur& joueur);
        void createSocket();
        void createBindedSocket();
        void attendServerPret();

    private :
        int sockfd;
        struct sockaddr_in servaddr, cliaddr;
        bool serverPret = false;
};
#endif