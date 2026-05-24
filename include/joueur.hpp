#ifndef JOUEUR_H
#define JOUEUR_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "logguru.hpp"
#include "tank.hpp"
#include "tankClassique.hpp" 
#include "networkProtocol.hpp"

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception

class Joueur {
public :
    Joueur(); // Déclaration du constructeur

    // Méthode pour mettre à jour la position du tank
    void recup_TankPos(float x, float y);
    void afficherTypeTank() const;
    void setTank(std::unique_ptr<tank> newTank);
    void assignTankByType(int type);
    void resetInputs();
    void captureLocalInput(const sf::RenderWindow& window, bool alive);
    network::InputState toInputState() const;
    void applyInputState(const network::InputState& input);
    void applyTankState(const network::TankState& state);
    static std::unique_ptr<tank> makeTankByType(int type);

    // Membres de la classe
    std::unique_ptr<tank> Tank;
    int port;
    int id;
    int pV = 5;
    int pts = 0;
    std::string pseudo;
    bool Zpressed = false;
    bool Spressed = false;        
    bool Qpressed = false;
    bool Dpressed = false;
    bool Xpressed = false;
    bool Clicked = false;
    bool Tabpressed = false;
    sf::Vector2i mousePos;
    sf::Vector2f worldMousePos;
    bool vivant = true;
    int equipe = 0;
};

#endif
