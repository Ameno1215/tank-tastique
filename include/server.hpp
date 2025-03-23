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
#include <algorithm>
#include <random>

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
        {120, 1000},  
        {120, 2450},  
        {1870, 2450}, 
        {1520, 120},   
        {2600, 120}, 
        {3580, 2500}
    };

    int spawnRegen[6][3] = {
        {1905, 1645, 0},  
        {800, 1400, 0}, 
        {1870, 2230, 0}, 
        {3320, 2100, 0}, 
        {1905, 860, 0}, 
        {3200, 120, 0}   
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
    void updateRegen();
    std::chrono::time_point<std::chrono::steady_clock> timerRegen;


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
