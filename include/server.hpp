#ifndef SERVER_HPP
#define SERVER_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>

#include "joueur.hpp"
#include "partie.hpp"

#define SERVER_IP "192.168.1.48"
#define SERVER_PORT 3000

#define BUFFER_SIZE 1024  

std::atomic<bool> running(true);  // Permet d'arrêter le serveur proprement

class Server {
private:
    
    Partie partie;
    char buffer[100];
    int recieve_sockfd, send_sockfd = SERVER_PORT;
    int port_connexion;
    int sockfd[6];
    struct sockaddr_in recieve_clientaddr, send_clientaddr; //connexion
    struct sockaddr_in client[6];
    int tank_recu[6];
    void processEvent();
    void recevoirEvent();
    void sendToClient();
    void init_send_fd();

    void majDead(char* buffer);

    void createSocketConnexion();
    void createBindedSocket();
    void connexion();
    void afficher_buffer(char tab[][5], int nb_lignes);
    void sendTankToClient();
    void string_tank(std::string& chaine);
    void sendTankRecu();
    
    
public:
    Server();
    ~Server();
    
    void startServer();
    void setTankRecu(int index, int value);
    int getTankRecu(int index);
    int getNbTanksRecus();

};

#endif
