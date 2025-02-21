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

class Server {
private:
    
    Partie partie;
    int sockfdconnexion = SERVER_PORT;
    int port_connexion;
    int sockfd[6];
    struct sockaddr_in clientaddr, servaddr;
    std::mutex joueurMutex; // Mutex pour synchroniser les accès

    void createSocketConnexion();
    void createBindedSocket();
    void udpCom(Joueur& joueur);
    void connexion();
    
public:
    Server();
    ~Server();

    void startServer();
};

#endif
