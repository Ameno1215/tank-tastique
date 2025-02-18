#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

#include "joueur.hpp"

class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Vector2f originalSize;
    bool isHovered = false;

    Button(float x, float y, float width, float height, const std::string& label, sf::Font& font) {
        originalSize = sf::Vector2f(width, height);
        shape.setSize(originalSize);
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::White);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + 100, y + 60);
    }

    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void update(sf::Vector2f mousePos) {
        if (shape.getGlobalBounds().contains(mousePos)) {
            if (!isHovered) {
                shape.setSize(originalSize * 1.2f);
                shape.setPosition(shape.getPosition().x - (originalSize.x * 0.1f), shape.getPosition().y - (originalSize.y * 0.1f));
                isHovered = true;
            }
        } else {
            if (isHovered) {
                shape.setSize(originalSize);
                shape.setPosition(shape.getPosition().x + (originalSize.x * 0.1f), shape.getPosition().y + (originalSize.y * 0.1f));
                isHovered = false;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

class Souris{
    public :
        float x;
        float y;
};

// Fonction pour envoyer un message UDP au serveur
void sendMessageToServer(const std::string& message) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    sendto(sockfd, message.c_str(), message.length(), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    close(sockfd);
}

// Fonction pour envoyer la position de la souris en message UDP au serveur
void sendPosToServer(float position[2]) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création du socket");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (sendto(sockfd, position, sizeof(float) * 2, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors de l'envoi");
        close(sockfd);
        return;
    }
    
    close(sockfd);
}

void sendUDPdata(Souris& souris, sf::Event event, Button button2){
    while(true){
        float position[2] = { souris.x, souris.y};
        sf::Vector2f mousePos(souris.x, souris.y);
        sendPosToServer(position);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (button2.isClicked(mousePos)) {
                std::cout << "Bouton cliqué" << std::endl;
                sendMessageToServer("M Hello Server!");
            }
        }
    }
}

void initconnexion(joueur Joueur){
    int sockfd;
    struct sockaddr_in servaddr;
    sendMessageToServer("C");
    char buffer[1024];
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&servaddr, &len);
    if(n < 0){
        perror...;
    }
    Joueur.portJoueur = buffer[0];

}
int main() {
    sf::RenderWindow window(sf::VideoMode(1800, 1600), "Test Connexion UDP");

    // Charger la texture du fond
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("fond.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }

    // Créer un sprite avec la texture
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        window.getSize().x / backgroundSprite.getGlobalBounds().width,
        window.getSize().y / backgroundSprite.getGlobalBounds().height
    );

    // Charger la police
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }

    // Boutons
    Button button1(600, 600, 400, 150, "Bouton Inactif", font);
    Button button2(600, 1000, 400, 150, "Envoyer Message", font);

    //Souris
    Souris souris;
    // Texte de réponse du serveur
    sf::Text responseText;
    responseText.setFont(font);
    responseText.setCharacterSize(20);
    responseText.setFillColor(sf::Color::White);
    responseText.setPosition(100, 300);

    sf::Event event;

    initConnexion();


    std::thread sendUDP(sendUDPdata, std::ref(souris), event, std::ref(button2));

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            sf::Vector2f mousePos(sf::Mouse::getPosition(window));
            button1.update(mousePos);
            button2.update(mousePos);
            souris.x = mousePos.x;
            souris.y = mousePos.y;
        }

        // Affichage
        window.clear();
        window.draw(backgroundSprite);  // Dessiner l'image de fond
        button1.draw(window);
        button2.draw(window);
        window.draw(responseText);
        window.display();
    }

    sendUDP.join();

    return 0;
}
