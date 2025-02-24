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
    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }
    memset(&send_clientaddr, 0, sizeof(send_clientaddr));
    send_clientaddr.sin_family = AF_INET;
    send_clientaddr.sin_port = htons(port_connexion);
    send_clientaddr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Socket créée pret à l'envoi sur le port " << port_connexion << std::endl;
}

void Server::createBindedSocket(){
    recieve_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (recieve_sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }
    int opt = 1;
    setsockopt(recieve_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    memset(&recieve_clientaddr, 0, sizeof(recieve_clientaddr));
    recieve_clientaddr.sin_family = AF_INET;
    recieve_clientaddr.sin_port = htons(port_connexion);
    recieve_clientaddr.sin_addr.s_addr = INADDR_ANY; // Accepter les connexions de n'importe quelle adresse

    // Liaison du socket au port spécifié
    if (bind(recieve_sockfd, (struct sockaddr*)&recieve_clientaddr, sizeof(recieve_clientaddr)) < 0) {
        perror("Échec du bind du socket");
        close(recieve_sockfd);
        return;
    }
    std::cout << "Socket créée et bindée sur le port " << port_connexion << std::endl;
}

void Server::connexion() {
    socklen_t len = sizeof(recieve_clientaddr);
    socklen_t send_len = sizeof(send_clientaddr);
    char buffer[BUFFER_SIZE];

    while (NB_JOUEUR > partie.get_nbJoueur()) {
    
        port_connexion = SERVER_PORT;
        createBindedSocket(); // sur le port 3000
        int n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_clientaddr, &len); // Attente d'un message "C"
        close(recieve_sockfd);
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
            sendto(send_sockfd, &port_to_send, sizeof(port_to_send), 0, (struct sockaddr*)&send_clientaddr, send_len);
            close(send_sockfd);
            std::cout << "Port " << port_connexion << " envoyé au client\n";

            port_connexion = 3000 + partie.get_nbJoueur(); //ports 3001 à 3007 si 6 joueurs
            createBindedSocket();

            // Attente du message "T"
            n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_clientaddr, &len);
            if (n < 0) {
                perror("Erreur réception de 'T'");
                close(recieve_sockfd);
                continue;
            }
            close(close(recieve_sockfd));
            buffer[n] = '\0';

            if (buffer[0] == 'T') {
                std::cout << "Client validé sur le port " << port_connexion << ".\n";
                std::string confirmation = "Connexion réussie !";
                createSocketConnexion();
                sendto(send_sockfd, confirmation.c_str(), confirmation.length(), 0, (struct sockaddr*)&send_clientaddr, send_len);
            } else {
                std::cout << "Échec d'initialisation avec le client\n";
            }

            close(send_sockfd);
            std::cout<<"Nombre de joueur : "<<partie.get_nbJoueur()<<std::endl;
        } else {
            std::cout << "En attente du bon nombre de joueurs...\n";
        }
    }

    char msg_pret[32];
    sprintf(msg_pret,"P %d",partie.get_nbJoueur());
    sleep(1);

    for (int i = 0; i < NB_JOUEUR; i++) {
        port_connexion = partie.joueur[i].port;
        createSocketConnexion();
        int sent = sendto(send_sockfd, msg_pret, strlen(msg_pret), 0, (struct sockaddr*)&send_clientaddr, sizeof(send_clientaddr));
        if (sent < 0) {
            perror("Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "Message 'P' envoyé au joueur " << partie.joueur[i].id  << " sur le port " << port_connexion << ".\n";
        }
        close(send_sockfd);
    }
}

void Server::recevoirEvent() {
    socklen_t len = sizeof(recieve_clientaddr);

    // Initialiser le buffer pour éviter des problèmes de lecture
    memset(buffer, 0, sizeof(buffer));

    int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);
    std::cout << "buffer recu " << buffer << " :\n";
    if (receivedBytes < 0) {
        std::cerr << "❌ Erreur lors de la réception des données" << std::endl;
        return;
    }

    int i, z, q, s, d, mouseX, mouseY; // Variables temporaires
    int valuesRead = sscanf(buffer, "%d %d %d %d %d %d %d", &i, &z, &q, &s, &d, &mouseX, &mouseY);

    if (valuesRead != 7) {
        std::cerr << "❌ Erreur de parsing des données reçues (sscanf a lu " << valuesRead << " valeurs)" << std::endl;
        std::cerr << "Contenu du buffer : " << buffer << std::endl;
        return;
    }

    // Mise à jour des touches et de la position de la souris
    partie.joueur[i].Zpressed = (z != 0);
    partie.joueur[i].Qpressed = (q != 0);
    partie.joueur[i].Spressed = (s != 0);
    partie.joueur[i].Dpressed = (d != 0);
    partie.joueur[i].worldMousePos = sf::Vector2f(mouseX, mouseY);

    // Affichage des données reçues pour débogage
    std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
    std::cout << "Touches : Z=" << partie.joueur[i].Zpressed 
              << " Q=" << partie.joueur[i].Qpressed 
              << " S=" << partie.joueur[i].Spressed 
              << " D=" << partie.joueur[i].Dpressed << std::endl;
    std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x 
              << " Y=" << partie.joueur[i].worldMousePos.y << std::endl;
}

void Server::sendToClient(){
    char buffer_processed_data[100];  // Par exemple, assez grand pour 4 entiers et quelques espaces
    for(int i = 0; i<NB_JOUEUR; i++){
        // Récupère les données du tank du joueur 0
        for(int j = 0; j<NB_JOUEUR; j++){
            tank& tankjoueur = partie.joueur[j].Tank;

            // Formate les données dans le buffer
            sprintf(buffer_processed_data, "%d %f %f %f %f %d", partie.joueur[j].id, tankjoueur.get_x(), tankjoueur.get_y(), tankjoueur.get_ori(), tankjoueur.getTourelleSprite().getRotation(), 1);

            // Envoi des données
            int n = sendto(sockfd[i], buffer_processed_data, strlen(buffer_processed_data), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
                        
            if (n < 0) {
                perror("❌ Erreur lors de l'envoi des données");
                return;
            } else {
                //std::cout << "📨 Données processed envoyées au client : " << buffer_processed_data << std::endl;
                //std::cout << "Sur le port " << sockfd[0] << std::endl;
            }
        }
    }
}

void Server::processEvent(){
    for(int i = 0; i<NB_JOUEUR; i++){
        partie.joueur_courant = i;
        partie.update();
    }
}

void Server::init_send_fd(){
    std::cout << "Initialisation des sockfd et clientaddr de chaque client\n";
    int recup = send_sockfd;
    int port = port_connexion;
    for(int i=0; i<NB_JOUEUR; i++){
        port_connexion = 3001 + i;
        createSocketConnexion();
        sockfd[i] = send_sockfd;
        client[i] = send_clientaddr;
    }
    send_sockfd = recup;
    port_connexion = port; 
    std::cout << "Fin de l'initialisation\n";
}

void Server::startServer() {
    
    connexion();  // Lancement de la gestion des connexions

    partie.joueur[0].id = 0;
    partie.joueur[0].port = 3001;
    partie.joueur[0].pseudo = "joueur1";

    partie.joueur[1].id = 1;
    partie.joueur[1].port = 3002;
    partie.joueur[1].pseudo = "joueur2";
    port_connexion = 3000;
    createBindedSocket();
    port_connexion = 3001;
    createSocketConnexion();

    init_send_fd();

    // Thread dédié pour recevoir les événements des clients
    std::thread receptionThread([this]() {
        while (running) {
            recevoirEvent();
        }
    });

    // Boucle principale du serveur
    while (running) {
        //recevoirEvent();
        processEvent();  
        sendToClient();
        std::this_thread::sleep_for(std::chrono::milliseconds(4));  // Ajout d'un délai pour éviter une boucle trop rapide
    }

    // Fermeture propre du serveur
    if (receptionThread.joinable()) {
        receptionThread.join();
    }

    close(recieve_sockfd);  //pas toucher
}

int main() {
    Server server;
    server.startServer();
    return 0;
}
