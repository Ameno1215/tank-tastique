#include "server.hpp"

void udpCom(Joueur& joueur, Partie& partie) {
    int sockfd;
    struct sockaddr_in servaddr, clientaddr;

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

    const char* msg_pret = "P";  

    sleep(1);
    for (int i = 0; i < NB_JOUEUR; i++) {
        struct sockaddr_in sendaddr;
        memset(&sendaddr, 0, sizeof(sendaddr));
        sendaddr.sin_family = AF_INET;
        sendaddr.sin_addr.s_addr = inet_addr(SERVER_IP);  
        sendaddr.sin_port = htons(partie.joueur[i].port);

        int sent = sendto(sockfd, msg_pret, strlen(msg_pret), 0, (struct sockaddr*)&sendaddr, sizeof(sendaddr));

        if (sent < 0) {
            perror("Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "Message 'P' envoyé au joueur " << partie.joueur[i].id  << " sur le port " << partie.joueur[i].port << ".\n";
        }
    }
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

    connexion(sockfd, partie);  // Lancement de la gestion des connexions

    partie.joueur[0].id = 0;
    partie.joueur[0].port = 3001;
    partie.joueur[0].pseudo = "joueur1";

    partie.joueur[1].id = 1;
    partie.joueur[1].port = 3002;
    partie.joueur[1].pseudo = "joueur2";

    std::thread joueur1Thread(udpCom, std::ref(partie.joueur[0]), std::ref(partie));
    std::thread joueur2Thread(udpCom, std::ref(partie.joueur[1]), std::ref(partie));
    
    joueur1Thread.join();
    joueur2Thread.join();
    close(sockfd);
}

int main() {
    startServer();
    return 0;
}
