#include "client.hpp"

Client::Client(){
    this->num_port = 3000;
    sockfd = -1;  // Initialisation du socket dans le constructeur
}

void Client::createSocket(){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(num_port);
    servaddr.sin_addr.s_addr = INADDR_ANY;
}

void Client::createBindedSocket(){
    //std::cout << "TEEEEEEEEEEEEEEEESSSSTTTT" << std::endl;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(num_port);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP); // Accepter les connexions de n'importe quelle adresse

    // Liaison du socket au port spécifié
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Échec du bind du socket");
        close(sockfd);
        return;
    }

    std::cout << "Socket créée et bindée sur le port " << num_port << std::endl;
}



// Fonction pour envoyer un message UDP au serveur
void Client::sendMessageToServer(const std::string& message) {
    createSocket();
    sendto(sockfd, message.c_str(), message.length(), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    close(sockfd);
}

void Client::initconnexion(){
    int received_port;
    std::string message = "C";
    socklen_t len = sizeof(servaddr);
    sendMessageToServer(message);                                                              //envoie du premier messsage sur le port 3000
    std::cout << "ICI Message 'C' envoyé au serveur.\n";

    createBindedSocket();
    int n = recvfrom(sockfd, &received_port, sizeof(received_port), 0, (struct sockaddr*)&servaddr, &len); // Réception du nouveau port du serveur
    if (n < 0) {
        perror("Erreur lors de la réception du port");
        close(sockfd);
        return;
    }
    close(sockfd); 

    received_port = ntohl(received_port);
    num_port = received_port;
    std::cout << "Nouveau port reçu du serveur : " << received_port << "\n";
    message = "T";          
    sleep(0.001);
    sendMessageToServer(message);// Envoie du message test sur le nouveau port                                                            
    std::cout << "Message 'T' envoyé au serveur sur le port " << received_port << ".\n";

    createBindedSocket();
    char buffer[1024];
    n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&servaddr, &len); // Attente de la confirmation du serveur
    if (n < 0) { 
        perror("Erreur lors de la réception de la confirmation");
        close(sockfd);
        return;
    }

    buffer[n] = '\0';
    std::cout << "Confirmation du serveur : " << buffer << "\n";
    
    close(sockfd);
} 

void Client::attendServerPret() {
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(serverAddr);
    char msg_pret[1024] = {0};  

    // Si le socket n'est pas déjà créé, le créer
    if (sockfd <= 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "❌ Erreur: Impossible de créer le socket UDP. Code erreur: " << errno << " (" << strerror(errno) << ")\n";
            return;
        }
        std::cout << "✅ Socket UDP créé avec succès: " << sockfd << std::endl;

        // Configuration de l'adresse du client
        memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_addr.s_addr = INADDR_ANY;  
        clientAddr.sin_port = htons(num_port);

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Liaison du socket au port
        if (bind(sockfd, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0) {
            std::cerr << "❌ Erreur: Impossible de lier le socket. Code erreur: " << errno << " (" << strerror(errno) << ")\n";
            close(sockfd);
            sockfd = -1;  
            return;
        }
        std::cout << "✅ Socket lié au port " << num_port << std::endl;
    }

    std::cout << "🟡 En attente du serveur... sur le port" << num_port<<std::endl;

    while (!serverPret) {
        if (sockfd < 0) {
            std::cerr << "❌ ERREUR: `sockfd` est invalide avant `recvfrom()`\n";
            return;
        }

        int n = recvfrom(sockfd, msg_pret, sizeof(msg_pret) - 1, 0, (struct sockaddr*)&serverAddr, &addrLen);
        if (n > 0) {
            msg_pret[n] = '\0';  
            if (strcmp(msg_pret, "P") == 0) {
                serverPret = true;
                std::cout << "✅ Serveur prêt !\n";
            }
        } else {
            std::cerr << "❌ Erreur lors de la réception du message du serveur\n";
            std::cerr << "Code erreur: " << errno << " (" << strerror(errno) << ")\n";
        }
    }

    close(sockfd);
    sockfd = -1;  
}


void Client::sendData(){
    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "T %.2f %.2f %.2f", joueur.Tank.get_x(), joueur.Tank.get_y(), joueur.Tank.get_ori());

    sendMessageToServer(buffer);
}

void Client::udpdateData(Joueur& joueur){
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);

    if (sockfd <= 0) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "❌ Erreur: Impossible de créer le socket UDP. Code erreur: " << errno << " (" << strerror(errno) << ")\n";
            return;
        }
        std::cout << "✅ Socket UDP créé avec succès: " << sockfd << std::endl;

        // Configuration de l'adresse du client
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;  
        serverAddr.sin_port = htons(num_port);

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Liaison du socket au port
        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "❌ Erreur: Impossible de lier le socket. Code erreur: " << errno << " (" << strerror(errno) << ")\n";
            close(sockfd);
            sockfd = -1;  
            return;
        }
        std::cout << "✅ Socket lié au port " << num_port << std::endl;
    }
    
    int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&serverAddr, &addrLen);
    
    if (n < 0) {
        perror("Erreur lors de la réception");
        return;
    }

    buffer[n] = '\0'; // Assurer une terminaison correcte de la chaîne

    // Vérifier que le message commence bien par 'T'
    if (buffer[0] == 'T') {
        float x, y, ori;
        if (sscanf(buffer, "T %f %f %f", &x, &y, &ori) == 3) {
            joueur.Tank.set_x(x);
            joueur.Tank.set_y(y);
            joueur.Tank.set_ori(ori);
        } else {
            std::cerr << "Format de message invalide !" << std::endl;
        }
    } else {
        std::cerr << "Message non valide reçu : " << buffer << std::endl;
    }

}
