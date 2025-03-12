#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

#define SERVER_IP "192.168.1.48"
#define SERVER_PORT_CONNEXION 3000

#include "bouton.hpp" 
#include "joueur.hpp"

class Client{

    public :

        Joueur joueur;
        int num_port; //cosntruire setter et accesseurs pour tout ceux la
        int sockfd, recieve_sockfd;
        struct sockaddr_in servaddr, recieve_servaddr;
        float new_ori, new_x, new_y, new_angle = 0;
        int nbJoueur;

        Client();
        void sendMessageToServer(const std::string& message);     // Fonction pour envoyer un message UDP au serveur
        void initconnexion();
        void sendData();
        void udpdateData(Joueur& joueur);
        void createSocket();
        void createBindedSocket();
        int get_etatConnexion();
        std::string getLocalIPAddress();
        bool ipValide = false;

        std::string server_ip;
        std::string pseudos[6];

        int test;

    private :
        int etatConnexion = -1;
};
#endif