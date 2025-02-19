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

#include "partie.hpp"

extern std::mutex joueurMutex; // Mutex pour synchroniser les accès

// Fonction de communication UDP
void udpCom(int sockfd, struct sockaddr_in& clientaddr, joueur& Joueur);

// Fonction d'initialisation et de gestion du serveur
void startServer();

void connexion(Partie& partie, int sockfd, struct sockaddr_in& clientaddr);

#endif
