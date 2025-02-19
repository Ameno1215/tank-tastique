#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception

#include "joueur.hpp"
#include "partie.hpp"

// Mutex pour synchroniser l'accès aux données partagées (position du tank)
std::mutex joueurMutex;

void udpCom(Partie& partie, int sockfd, struct sockaddr_in& clientaddr, joueur& Joueur) {
    while(true) {
        float posMouse[2];
        char buffer[BUFFER_SIZE];
        socklen_t len = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
        if (n < 0) {
            perror("Erreur lors de la réception");
            return;
        }

        if(buffer[0] == 'M') {
            // Traitement du message (si c'est un message classique et non une position)
            memmove(buffer, buffer + 2, n - 2);  // Enlève le "M "
            buffer[n] = '\0'; // Ajoute le terminateur de chaîne
            std::cout << "Message reçu : " << buffer << std::endl;
            std::string response = "Message bien reçu!";  // Réponse au client
            sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientaddr, len);
            std::cout << "Réponse envoyée au client." << std::endl;
        }
        if(buffer[0] == 'C'){
            partie.ajouteJoueur();
            float msg_port[1] = {partie.get_portactuel()};
            sendto(sockfd, msg_port, sizeof(float), 0, (struct sockaddr*)&clientaddr, len);
            std::cout << "Port envoyé au client" << std::endl;
        }
        else {
            // Si c'est une position de souris
            memcpy(posMouse, buffer, sizeof(posMouse));
            // Verrouiller l'accès au joueur avant de modifier sa position
            std::lock_guard<std::mutex> lock(joueurMutex);
            Joueur.recup_TankPos(posMouse[0], posMouse[1]);
            std::cout << "Position Souris client : x=" << posMouse[0] << " y=" << posMouse[1] << std::endl;
        }
    }
    close(sockfd);
}

void updateVisual(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Sprite& curseur, sf::FloatRect& taille_curseur, joueur& Joueur) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        window.clear();
        // Dessiner l'image de fond
        window.draw(backgroundSprite);

        // Mise à jour de l'affichage en fonction de la position du joueur
        {
            std::lock_guard<std::mutex> lock(joueurMutex);  // Protection des données partagées
            // Mettre à jour la position du curseur (tank)
            curseur.setPosition(Joueur.Tank.get_x()-(taille_curseur.width)/2, Joueur.Tank.get_y()-(taille_curseur.height)/2);  // Correction de l'accès
        }

        // Afficher le curseur (le tank)
        window.draw(curseur);
        window.display();
    }
}

int main() {
    joueur Joueur;
    Partie partie;
    partie.init();
    sf::RenderWindow window(sf::VideoMode(1800, 1600), "Test Server UDP");

    // Charger la texture du fond
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("fond.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
        return 1;
    }

    sf::Texture curseurTexture;
    if(!curseurTexture.loadFromFile("curseur_rouge.png")){
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
        return 1;
    }

    // Créer un sprite avec la texture
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    sf::Sprite curseurSprite;
    curseurSprite.setTexture(curseurTexture);
    sf::FloatRect taille_curseur = curseurSprite.getGlobalBounds();

    backgroundSprite.setScale(
        window.getSize().x / backgroundSprite.getGlobalBounds().width,
        window.getSize().y / backgroundSprite.getGlobalBounds().height
    );

    int sockfd;
    struct sockaddr_in servaddr, clientaddr;

    // Création du socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création du socket");
        return 1;
    }

    // Configuration de l'adresse du serveur
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; // Accepte toutes les connexions
    servaddr.sin_port = htons(SERVER_PORT);

    // Liaison du socket à l'adresse et au port
    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Échec du bind");
        close(sockfd);
        return 1;
    }

    std::cout << "Serveur UDP en attente sur le port " << SERVER_PORT << "..." << std::endl;

    // Lancer uniquement le thread pour la communication UDP
    std::thread comThread(udpCom, partie, sockfd, std::ref(clientaddr), std::ref(Joueur));

    // Boucle principale pour gérer l'affichage graphique
    updateVisual(window, backgroundSprite, curseurSprite, taille_curseur, Joueur);

    // Attendre que le thread de communication UDP se termine
    comThread.join();

    return 0;
}
