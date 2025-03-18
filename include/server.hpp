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
#include <chrono> 
#include <array> 
#include <cstdlib> 
#include <ctime> 
#include <fstream>

#include "joueur.hpp"
#include "partie.hpp"

#define SERVER_IP "192.168.1.48"
#define SERVER_PORT 3000

#define BUFFER_SIZE 1024  

std::atomic<bool> running(true);  // Permet d'arrêter le serveur proprement

class Server {
private:

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
    void init_choix_tank();
    void init_Spawn();
    std::string ip[6]; 
    std::string pseudos[6];

    int spawn[6][2] = {
        {350, 1800},  // Position en haut à gauche
        {2100, 400},  // Position en haut à droite
        {1200, 1200}, // Position centrale
        {400, 900},   // Position en bas à gauche
        {1800, 1700}, // Position en bas à droite
        {1000, 300}   // Position aléatoire en haut
    };
    
    void majDead(char* buffer);

    void createSocketConnexion(const std::string& ip);
    void createBindedSocket();
    void connexion();
    void afficher_buffer(char tab[][5], int nb_lignes);
    void sendTankToClient();
    void string_tank(std::string& chaine);
    void sendTankRecu();

    std::string extractIP(const std::string& message);
    std::string extractPseudo(const std::string& message);
    std::chrono::time_point<std::chrono::steady_clock> timer;
    std::array<std::array<std::chrono::time_point<std::chrono::steady_clock>, 2>, 6> chronoUlti;        

public:
    Partie partie;
    Server();
    ~Server();
    
    void startServer();
    void setTankRecu(int index, int value);
    int getTankRecu(int index);
    int getNbTanksRecus();
    int nb_joueur = 0;
    int mode = 1;  //par defaut en MG

};

#endif
