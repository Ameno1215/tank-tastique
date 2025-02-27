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

    //recupère n'importe quel message sur le port 3000
    int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);
    std::cout << "buffer recu " << buffer << " :\n";

    if (receivedBytes < 0) {   //verifie
        std::cerr << "❌ Erreur lors de la réception des données" << std::endl;
        return;
    }

    //recupère les données 
    int i, z, q, s, d, mouseX, mouseY, clicked; 
    int valuesRead = sscanf(buffer, "%d %d %d %d %d %d %d %d", &i, &z, &q, &s, &d, &mouseX, &mouseY, &clicked);

    if (valuesRead != 8) { 
        perror("Erreur de lecture avec sscanf, c'est la sauce !");
        exit(EXIT_FAILURE);
    }

    // Stockage des données dans le tableau de la partie
    partie.joueur[i].Zpressed = (z != 0);
    partie.joueur[i].Qpressed = (q != 0);
    partie.joueur[i].Spressed = (s != 0);
    partie.joueur[i].Dpressed = (d != 0);
    partie.joueur[i].worldMousePos = sf::Vector2f(mouseX, mouseY);
    partie.joueur[i].Clicked = (clicked != 0);

    // Affichage des données reçues pour débogage
    std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
    std::cout << "Touches : Z=" << partie.joueur[i].Zpressed << " Q=" << partie.joueur[i].Qpressed << " S=" << partie.joueur[i].Spressed << " D=" << partie.joueur[i].Dpressed << std::endl;
    std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x << " Y=" << partie.joueur[i].worldMousePos.y << "clicked : "<< partie.joueur[i].Clicked << std::endl; 
}

void Server::sendToClient(){
    char buffer_processed_data[100];
    char buffer_pV[100];
    sprintf(buffer_pV, "V %d %d %d %d %d %d %d", partie.joueur[0].pV, partie.joueur[1].pV, partie.joueur[2].pV, partie.joueur[3].pV, partie.joueur[4].pV, partie.joueur[5].pV, 1);

    for(int i = 0; i<NB_JOUEUR; i++){   

        for(int j = 0; j<NB_JOUEUR; j++){

            //recupère les tank/data processed de chaque joueur
            tank& tankjoueur = partie.joueur[j].Tank;
            sprintf(buffer_processed_data, "T %d %f %f %f %f %d", partie.joueur[j].id, tankjoueur.get_x(), tankjoueur.get_y(), tankjoueur.get_ori(), tankjoueur.getTourelleSprite().getRotation(), 1);

            //les envoies à chaque autre client
            int n = sendto(sockfd[i], buffer_processed_data, strlen(buffer_processed_data), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
            //verifiacation
            if (n < 0) {
                perror("❌ Erreur lors de l'envoi des données du tank");
                return;
            } else {
                //debugage
                //std::cout << "📨 Données processed envoyées au client : " << buffer_processed_data << std::endl;
                //std::cout << "Sur le port " << sockfd[0] << std::endl;
            }

            // ENVOI DU NOMBRE D'OBUS
            // sprintf(buffer_nb_obus, "N %d %d", partie.nb_obus(), 1);

            // //les envoies à chaque autre client
            // n = sendto(sockfd[i], buffer_nb_obus, strlen(buffer_nb_obus), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
            // //verifiacation
            // if (n < 0) {
            //     perror("❌ Erreur lors de l'envoi des données du nombre d'obus");
            //     return;
            // } else {
            //     //debugage
            //     //std::cout << "📨 Données processed envoyées au client : " << buffer_nb_obus << std::endl;
            //     //std::cout << "Sur le port " << sockfd[0] << std::endl;
            // }

            // ENVOIE DE LA LISTE D'OBUS
            std::string buffer_liste_obus;
            partie.string_obus(buffer_liste_obus);


            //les envoies à chaque autre client
            n = sendto(sockfd[i], buffer_liste_obus.c_str(), strlen(buffer_liste_obus.c_str()), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
            //verifiacation
            if (n < 0) {
                perror("❌ Erreur lors de l'envoi des données du nombre d'obus");
                return;
            } else {
                //debugage
                //std::cout << "📨 Données processed envoyées au client : " << buffer_nb_obus << std::endl;
                //std::cout << "Sur le port " << sockfd[0] << std::endl;
            }
        }
        int n = sendto(sockfd[i], buffer_pV, strlen(buffer_pV), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
        //verifiacation
        if (n < 0) {
            perror("❌ Erreur lors de l'envoi des données des pV");
            return;
        } else {
            //debugage
            //std::cout << "📨 Données processed envoyées au client : " << buffer_processed_data << std::endl;
            //std::cout << "Sur le port " << sockfd[0] << std::endl;
        }
    }
}

void Server::afficher_buffer(char tab[][5], int nb_lignes) {
    printf("Tableau buffer obus\n");
    for (int i = 0; i < nb_lignes; i++) {
        std::cout << "Ligne " << i << " : ";
        for (int j = 0; j < 5; j++) {
            std::cout << static_cast<int>(tab[i][j]) << " "; // Convertir en int pour affichage lisible
        }
        std::cout << std::endl;
    }
}

void Server::processEvent(){
    for(int i = 0; i<NB_JOUEUR; i++){
        partie.joueur_courant = i;
        partie.update();
    }
}

void Server::init_send_fd(){

    port_connexion = 3000;
    createBindedSocket();

    std::cout << "Initialisation des sockfd et clientaddr de chaque client\n";

    int recup = send_sockfd;              //pour ne pas tout casse
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

    init_send_fd();

    sf::Texture texturetest;
    texturetest.loadFromFile("Image/base1.png");
    partie.testSprite.setTexture(texturetest);
    partie.testSprite.setScale(0.08f, 0.08f);
    partie.testSprite.setPosition(300, 300);

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
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // Ajout d'un délai pour éviter une boucle trop rapide
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
