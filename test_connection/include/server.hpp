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

#define SERVER_PORT 3000  
#define BUFFER_SIZE 1024  
#define NEW_PORT 3001

std::atomic<bool> running(true);  // Permet d'arrêter le serveur proprement

class Server {
private:
    
    Partie partie;
    char buffer[100];
    int recieve_sockfd, send_sockfd = SERVER_PORT;
    int port_connexion;
    int sockfd[6];
    struct sockaddr_in recieve_clientaddr, send_clientaddr;
    std::mutex joueurMutex; // Mutex pour synchroniser les accès
    void processEvent();
    void recevoirEvent();
    void sendToClient();

    void createSocketConnexion();
    void createBindedSocket();
    void connexion();
    
public:
    Server();
    ~Server();

    void startServer();
};

#endif
