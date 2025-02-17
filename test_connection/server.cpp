#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 3000  // Port du serveur
#define BUFFER_SIZE 1024  // Taille du buffer de réception

int main() {
    int sockfd;
    struct sockaddr_in servaddr, clientaddr;
    char buffer[BUFFER_SIZE];

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

    while (true) {
        socklen_t len = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
        if (n < 0) {
            perror("Erreur lors de la réception");
            continue;
        }
        buffer[n] = '\0'; // Ajout du terminateur de chaîne

        // Afficher le message reçu
        std::cout << "Message reçu : " << buffer << std::endl;

        // Répondre au client
        std::string response = "Message bien reçu!";
        sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientaddr, len);
        std::cout << "Réponse envoyée au client." << std::endl;
    }

    close(sockfd);
    return 0;
}
