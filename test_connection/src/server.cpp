#include "server.hpp"

Server::Server() {
    std::memset(sockfd, 0, sizeof(sockfd));
    std::cout << "Serveur initialisé." << std::endl;
}

Server::~Server() {
    for (int i = 0; i < 6; ++i) {
        if (sockfd[i] > 0) {
            close(sockfd[i]);
        }
    }
    std::cout << "Serveur arrêté." << std::endl;
}

void Server::createSocketConnexion(){
    sockfdconnexion = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfdconnexion < 0) {
        perror("Échec de la création du socket");
        return;
    }
    memset(&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(port_connexion);
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Socket créée pret à l'envoi sur le port " << port_connexion << std::endl;
}

void Server::createBindedSocket(){
    sockfdconnexion = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfdconnexion < 0) {
        perror("Échec de la création du socket");
        return;
    }
    int opt = 1;
    setsockopt(sockfdconnexion, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    memset(&servaddr, 0, sizeof(servaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(port_connexion);
    clientaddr.sin_addr.s_addr = INADDR_ANY; // Accepter les connexions de n'importe quelle adresse

    // Liaison du socket au port spécifié
    if (bind(sockfdconnexion, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("Échec du bind du socket");
        close(sockfdconnexion);
        return;
    }
    std::cout << "Socket créée et bindée sur le port " << port_connexion << std::endl;
}

void Server::udpCom(Joueur& joueur) {
    int sockfd;
    struct sockaddr_in clientaddr;

    // Création du socket UDP principal
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(joueur.port);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors du bind");
        close(sockfd);
        return;
    }

    std::cout << "thread en écoute sur le port" << joueur.port <<std::endl;

    while (true) {
        char buffer[BUFFER_SIZE];
        socklen_t len = sizeof(clientaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);

        if (n < 0) {
            perror("Erreur lors de la réception");
            return;
        }
        buffer[n] = '\0'; // Assurer une bonne lecture

        // Vérification du message
        if (buffer[0] == 'T' && n >= 4) {  
            float x, y, ori;
            if (sscanf(buffer + 1, "%f %f %f", &x, &y, &ori) == 3) {  
                joueur.Tank.set_x(x);  
                joueur.Tank.set_y(y);  
                joueur.Tank.set_ori(ori);  
            } else {
                std::cerr << "Erreur dans la forme du msg" << std::endl;
            }

            std::cout << " Position Tank mise à jour: x=" << joueur.Tank.get_x() << ", y=" << joueur.Tank.get_y() << ", ori=" << joueur.Tank.get_ori() << "\n";
            
            for (int i = 0; i < NB_JOUEUR; i++) {
                if (partie.joueur[i].id != joueur.id) {  // pas enoyer au meme joueur
                    struct sockaddr_in sendaddr;
                    memset(&sendaddr, 0, sizeof(sendaddr));
                    sendaddr.sin_family = AF_INET;
                    sendaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
                    sendaddr.sin_port = htons(partie.joueur[i].port);

                    sendto(sockfd, buffer, n, 0, (struct sockaddr*)&sendaddr, sizeof(sendaddr));

                    std::cout << " Données envoyées à Joueur " << partie.joueur[i].id << " sur le port " << partie.joueur[i].port << ".\n";
                }
            }
        } else {
            std::cout << " Message non valide reçu.\n";
        }

    }
    close(sockfd);
}


void Server::connexion() {
    socklen_t len = sizeof(clientaddr);
    char buffer[BUFFER_SIZE];

    while (!partie.partieComplete()) {
        // Attente d'un message "C"
        port_connexion = SERVER_PORT;
        createBindedSocket(); // sur le port 3000
        int n = recvfrom(sockfdconnexion, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
        close(sockfdconnexion);
        if (n < 0) {
            perror("Erreur lors de la réception");
            continue;
        }
        buffer[n] = '\0';
        if (buffer[0] == 'C') {
            std::cout << "Message 'C' reçu. Attribution d'un port...\n";
            if (!partie.ajouteJoueur()) { // ajoute les joueurs
                continue;
            }

            int port_to_send = htons(3000 + partie.get_nbJoueur()); //ports 3001 à 3007 si 6 joueurs
            createSocketConnexion();
            sendto(sockfdconnexion, &port_to_send, sizeof(port_to_send), 0, (struct sockaddr*)&clientaddr, len);
            close(sockfdconnexion);
            std::cout << "Port " << port_connexion << " envoyé au client\n";

            port_connexion = 3000 + partie.get_nbJoueur(); 
            createBindedSocket();

            // Attente du message "T"
            n = recvfrom(sockfdconnexion, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientaddr, &len);
            if (n < 0) {
                perror("Erreur réception de 'T'");
                close(sockfdconnexion);
                continue;
            }
            close(close(sockfdconnexion));
            buffer[n] = '\0';

            if (buffer[0] == 'T') {
                std::cout << "Client validé sur le port " << port_connexion << ".\n";
                std::string confirmation = "Connexion réussie !";
                createSocketConnexion();
                sendto(sockfdconnexion, confirmation.c_str(), confirmation.length(), 0, (struct sockaddr*)&clientaddr, len);
            } else {
                std::cout << "Échec d'initialisation avec le client\n";
            }

            close(sockfdconnexion);
            std::cout<<"Nombre de joueur : "<<partie.get_nbJoueur()<<std::endl;
        } else {
            std::cout << "En attente du bon nombre de joueurs...\n";
        }
    }

    const char* msg_pret = "P";  

    sleep(1);
    for (int i = 0; i < NB_JOUEUR; i++) {
        port_connexion = partie.joueur[i].port;
        createSocketConnexion();
        int sent = sendto(sockfdconnexion, msg_pret, strlen(msg_pret), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
        if (sent < 0) {
            perror("Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "Message 'P' envoyé au joueur " << partie.joueur[i].id  << " sur le port " << port_connexion << ".\n";
        }
        close(sockfdconnexion);
    }
}

void Server::startServer() {
    
    connexion();  // Lancement de la gestion des connexions

    partie.joueur[0].id = 0;
    partie.joueur[0].port = 3001;
    partie.joueur[0].pseudo = "joueur1";

    partie.joueur[1].id = 1;
    partie.joueur[1].port = 3002;
    partie.joueur[1].pseudo = "joueur2";

    //std::thread joueur1Thread(udpCom, std::ref(partie.joueur[0]));
    //std::thread joueur2Thread(udpCom, std::ref(partie.joueur[1]));
    
    //joueur1Thread.join();
    //joueur2Thread.join();
    close(sockfdconnexion);
}

int main() {
    Server server;
    server.startServer();
    return 0;
}
