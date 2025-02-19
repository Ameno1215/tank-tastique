#include "client.hpp"

Client::Client(){
    this->num_port = 0;
}

// Fonction pour envoyer un message UDP au serveur
void Client::sendMessageToServer(const std::string& message, int port) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    sendto(sockfd, message.c_str(), message.length(), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    close(sockfd);
}

// Fonction pour envoyer la position de la souris en message UDP au serveur
void Client::sendPosToServer(float position[2]) {
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

void Client::initconnexion(){
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(servaddr);
    int received_port;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    std::string message = "C";                                                                  //envoie du premier messsage sur le port 3000
    sendto(sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&servaddr, len);
    std::cout << "Message 'C' envoyé au serveur.\n";

    int n = recvfrom(sockfd, &received_port, sizeof(received_port), 0, (struct sockaddr*)&servaddr, &len); // Réception du nouveau port du serveur
    if (n < 0) {
        perror("Erreur lors de la réception du port");
        close(sockfd);
        return;
    }

    received_port = ntohl(received_port);
    std::cout << "Nouveau port reçu du serveur : " << received_port << "\n";

    close(sockfd); 

    int new_sockfd = socket(AF_INET, SOCK_DGRAM, 0);     // Création d'un nouveau socket pour communiquer sur le nouveau port
    if (new_sockfd < 0) {
        perror("Erreur lors de la création du nouveau socket");
        return;
    }

    struct sockaddr_in new_servaddr;
    memset(&new_servaddr, 0, sizeof(new_servaddr));
    new_servaddr.sin_family = AF_INET;
    new_servaddr.sin_port = htons(received_port);
    new_servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    message = "T";          // Envoie du message test sur le nouveau port
    sendto(new_sockfd, message.c_str(), message.length(), 0, (struct sockaddr*)&new_servaddr, len);
    std::cout << "Message 'T' envoyé au serveur sur le port " << received_port << ".\n";

    n = recvfrom(new_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&new_servaddr, &len); // Attente de la confirmation du serveur
    if (n < 0) { 
        perror("Erreur lors de la réception de la confirmation");
        close(new_sockfd);
        return;
    }

    buffer[n] = '\0';
    std::cout << "Confirmation du serveur : " << buffer << "\n";
    
    close(new_sockfd);
}

