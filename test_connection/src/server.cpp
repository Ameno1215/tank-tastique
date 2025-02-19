#include "server.hpp"

// Mutex global
std::mutex joueurMutex;

void udpCom(int sockfd, struct sockaddr_in& clientaddr, joueur& Joueur) {
    while (true) {
        float posMouse[2];
        char buffer[BUFFER_SIZE];
        socklen_t len = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);


        if (n < 0) {
            perror("Erreur lors de la réception");
            return;
        }

        if (buffer[0] == 'M') {
            // Gestion des messages
            memmove(buffer, buffer + 2, n - 2);  
            buffer[n] = '\0'; 
            std::cout << "Message reçu : " << buffer << std::endl;
            std::string response = "Message bien reçu!";
            sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientaddr, len);
            std::cout << "Réponse envoyée au client." << std::endl;
        } 
        else {
            // Gestion des positions
            memcpy(posMouse, buffer, sizeof(posMouse));
            std::lock_guard<std::mutex> lock(joueurMutex);
            Joueur.recup_TankPos(posMouse[0], posMouse[1]);
            std::cout << "Position Souris client : x=" << posMouse[0] << " y=" << posMouse[1] << std::endl;
        }
    }
    close(sockfd);
}

void connexion(int sockfd, Partie& partie) {
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    char buffer[BUFFER_SIZE];

    while (!partie.partieComplete()) {
        // Attente d'un message "C"
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
        if (n < 0) {
            perror("Erreur lors de la réception");
            continue;
        }

        buffer[n] = '\0';
        if (buffer[0] == 'C') {
            std::cout << "Message 'C' reçu. Attribution d'un port...\n";
            if (!partie.ajouteJoueur()) {
                continue;
            }

            int new_port = partie.get_portactuel();
            int port_to_send = htonl(new_port);
            sendto(sockfd, &port_to_send, sizeof(port_to_send), 0, (struct sockaddr*)&clientaddr, len);
            std::cout << "Port " << new_port << " attribué au client.\n";

            // Création d'un socket pour le client
            int client_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (client_sockfd < 0) {
                perror("Erreur socket client");
                continue;
            }

            struct sockaddr_in client_servaddr;
            memset(&client_servaddr, 0, sizeof(client_servaddr));
            client_servaddr.sin_family = AF_INET;
            client_servaddr.sin_addr.s_addr = INADDR_ANY;
            client_servaddr.sin_port = htons(new_port);

            if (bind(client_sockfd, (struct sockaddr*)&client_servaddr, sizeof(client_servaddr)) < 0) {
                perror("Erreur lors du bind client");
                close(client_sockfd);
                continue;
            }

            // Attente du message "T"
            n = recvfrom(client_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
            if (n < 0) {
                perror("Erreur réception de 'T'");
                close(client_sockfd);
                continue;
            }

            buffer[n] = '\0';
            if (buffer[0] == 'T') {
                std::cout << "Client validé sur le port " << new_port << ".\n";
                std::string confirmation = "Connexion réussie !";
                sendto(client_sockfd, confirmation.c_str(), confirmation.length(), 0, (struct sockaddr*)&clientaddr, len);
            } else {
                std::cout << "Échec d'initialisation avec le client\n";
            }

            close(client_sockfd);
        } else {
            std::cout << "En attente du bon nombre de joueurs...\n";
        }
    }
    std::cout << "Tous les joueurs sont connectés. Début de la partie !\n";
}

void startServer() {
    Partie partie;
    int sockfd;
    struct sockaddr_in servaddr;

    // Création du socket UDP principal
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors du bind");
        close(sockfd);
        return;
    }

    std::cout << "Serveur en attente sur le port " << SERVER_PORT << "...\n";

    // Lancement de la gestion des connexions
    connexion(sockfd, partie);

    close(sockfd);
}

int main() {
    startServer();
    return 0;
}