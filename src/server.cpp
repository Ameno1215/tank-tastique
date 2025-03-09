#include "server.hpp"

Server::Server() {
    std::memset(sockfd, 0, sizeof(sockfd));
    std::cout << "Serveur initialisé." << std::endl;

    for (int i = 0; i < 6; i++) {
        tank_recu[i] = 0;
    }
}

Server::~Server() {
    for (int i = 0; i < 6; ++i) {
        if (sockfd[i] > 0) {
            close(sockfd[i]);
        }
    }
    std::cout << "Serveur arrêté." << std::endl;
}

void Server::createSocketConnexion(const std::string& ip){
    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }
    memset(&send_clientaddr, 0, sizeof(send_clientaddr));
    send_clientaddr.sin_family = AF_INET;
    send_clientaddr.sin_port = htons(port_connexion);
    if (inet_pton(AF_INET, ip.c_str(), &send_clientaddr.sin_addr) <= 0) {
        perror("Échec de la conversion de l'IP");
        close(send_sockfd);
        return;
    }
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
    recieve_clientaddr.sin_addr.s_addr = INADDR_ANY;  // Accepte les connexions de n'importe quelle adresse

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

            std::string ipJoueur = extractIP(buffer);
            
            std::cout << "Message 'C' reçu. Recuperation de l'IP et attribution d'un port...\n";

            if (!partie.ajouteJoueur()) { // ajoute les joueurs
                continue;
            }

            int port_to_send = htons(3000 + partie.get_nbJoueur()); //ports 3001 à 3007 si 6 joueurs
            createSocketConnexion(ipJoueur); //creation de la socket sur la bonne adresse
            ip[partie.get_nbJoueur() - 1] = ipJoueur; //ajoute l'adresse IP au tableau des IP
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
                createSocketConnexion(ipJoueur);
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
        createSocketConnexion(ip[i]);
        int sent = sendto(send_sockfd, msg_pret, strlen(msg_pret), 0, (struct sockaddr*)&send_clientaddr, sizeof(send_clientaddr));
        if (sent < 0) {
            perror("Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "Message 'P' envoyé au joueur " << partie.joueur[i].id  << " sur le port " << port_connexion << ".\n";
        }
        close(send_sockfd);
    }
}

std::string Server::extractIP(const std::string& message) {
    std::istringstream iss(message);
    std::string type, ip;

    iss >> type >> ip; // Sépare "C" et l'adresse IP

    if (!ip.empty()) {
        std::cout << "IP extraite du message : " << ip << std::endl;
        return ip;
    } else {
        std::cout << "Erreur : IP non trouvée dans le message !" << std::endl;
        return ip;
    }
}

void Server::recevoirEvent() {
    socklen_t len = sizeof(recieve_clientaddr);

    // Initialiser le buffer pour éviter des problèmes de lecture
    memset(buffer, 0, sizeof(buffer));

    //recupère n'importe quel message sur le port 3000
    int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);

    if (receivedBytes < 0) {   //verifie
        std::cerr << "❌ Erreur lors de la réception des données" << std::endl;
        return;
    }

    //recupère les données 
    if (buffer[0] == 'A') {
        int i, z, q, s, d, mouseX, mouseY, clicked; 
        int valuesRead = sscanf(buffer, "A %d %d %d %d %d %d %d %d", &i, &z, &q, &s, &d, &mouseX, &mouseY, &clicked);

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
        // std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
        // std::cout << "Touches : Z=" << partie.joueur[i].Zpressed << " Q=" << partie.joueur[i].Qpressed << " S=" << partie.joueur[i].Spressed << " D=" << partie.joueur[i].Dpressed << std::endl;
        // std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x << " Y=" << partie.joueur[i].worldMousePos.y << "clicked : "<< partie.joueur[i].Clicked << std::endl; 
    }

    if (buffer[0] == 'K') {
        int type_tank = -1;
        int id;
        int valuesRead = sscanf(buffer, "K %d %d", &id, &type_tank);

        if (valuesRead != 2) { 
            perror("Erreur de lecture avec sscanf, c'est la sauce !");
            exit(EXIT_FAILURE);
        }

        // Stockage des données dans la partie
        printf("joueur %d à selectionner le tank : %d \n", id, type_tank);
        if (type_tank == 1) {
            partie.joueur[id].setTank(std::make_unique<Tank_vert>());
        }
        else if (type_tank == 2) {
            partie.joueur[id].setTank(std::make_unique<Tank_bleu>());
        }

        // partie.affiche_type_tank();
    } 

    if (buffer[0] == 'M') {
        int id;
        int valuesRead = sscanf(buffer, "M %d", &id);

        if (valuesRead != 1) { 
            perror("Erreur de lecture avec sscanf, c'est la sauce !");
            exit(EXIT_FAILURE);
        }

        setTankRecu(id, 1);
        std::cout << "Joueur " << id << " a recu tous les tank\n";

        if (getNbTanksRecus() == partie.get_nbJoueur()) {
            sendTankRecu();
        }
        
    } 
    // Affichage des données reçues pour débogage
    /*std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
    std::cout << "Touches : Z=" << partie.joueur[i].Zpressed << " Q=" << partie.joueur[i].Qpressed << " S=" << partie.joueur[i].Spressed << " D=" << partie.joueur[i].Dpressed << std::endl;
    std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x << " Y=" << partie.joueur[i].worldMousePos.y << "clicked : "<< partie.joueur[i].Clicked << std::endl;
    */
}

void Server::setTankRecu(int index, int value) {
    if (index >= 0 && index < 6) {
        tank_recu[index] = value;
    }
}

int Server::getTankRecu(int index) {
    if (index >= 0 && index < 6) {
        return tank_recu[index];
    }
    return -1;
}

int Server::getNbTanksRecus() {
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if (tank_recu[i] == 1) count++;
    }
    return count;
}


void Server::sendToClient(){
    char buffer_processed_data[100];
    char buffer_pV[100];
    char buffer_explo[100];

    char buffer_stat[1 + 6 * 4 * sizeof(float)];
    buffer_stat[0] = 'Z';
    // Sérialisation du tableau stat après le marqueur
    std::memcpy(buffer_stat + 1, partie.stat, sizeof(partie.stat));

    partie.listexplosion.toCharArray(buffer_explo);
    
    sprintf(buffer_pV, "V %d %d %d %d %d %d %d", partie.joueur[0].pV, partie.joueur[1].pV, partie.joueur[2].pV, partie.joueur[3].pV, partie.joueur[4].pV, partie.joueur[5].pV, 1);

    for(int i = 0; i<NB_JOUEUR; i++){   

        for(int j = 0; j<NB_JOUEUR; j++){

            //recupère les tank/data processed de chaque joueur
            tank& tankjoueur = *(partie.joueur[j].Tank);
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

        //envoie des pV
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
        if(partie.listexplosion.nouveau){
            std::cout << "buffer envoyé au client : " << buffer_explo << " (taille: " << strlen(buffer_explo) << ")\n";
            n = sendto(sockfd[i], buffer_explo, strlen(buffer_explo), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            if (n < 0) {
                perror("❌ Erreur lors de l'envoi des données des explosions");
            } else {
                std::cout << "📨 Explosion envoyée avec succès (" << n << " octets)\n";
            }

        }

        // Envoi du tableau stat
        n = sendto(sockfd[i], buffer_stat, sizeof(buffer_stat), 0, 
                  (const struct sockaddr*)&client[i], sizeof(client[i]));
        if (n < 0) {
            perror("❌ Erreur lors de l'envoi des données du tableau stat");
        } else {
            //std::cout << "📨 Tableau stat envoyé avec succès (" << n << " octets)\n";
        }
    }
    partie.listexplosion.maj();
}

void Server::sendTankToClient(){
    std::string buffer;
    string_tank(buffer);


    //les envoies à chaque autre client
    for (int i = 0; i < partie.get_nbJoueur(); i++) {
         int n = sendto(sockfd[i], buffer.c_str(), strlen(buffer.c_str()), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
        
        //verifiacation
        if (n < 0) {
            perror("❌ Erreur lors de l'envoi des données du tank");
            return;
        } else {
            //debugage
            //std::cout << "📨 Données processed envoyées au client : " << buffer_processed_data << std::endl;
            //std::cout << "Sur le port " << sockfd[0] << std::endl;
        }
    }     
}

void Server::sendTankRecu() {
    char buffer[10];
    sprintf(buffer, "W 1 1");
    printf("%c\n", buffer[0]);

    //les envoies à chaque autre client
    for (int i = 0; i < partie.get_nbJoueur(); i++) {
         int n = sendto(sockfd[i], buffer, strlen(buffer), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
        
        //verifiacation
        if (n < 0) {
            perror("❌ Erreur lors de l'envoi des données du tank");
            return;
        } else {
            //debugage
            std::cout << i << " envoie\n";
        }
    }     
}

void Server::string_tank(std::string& chaine) {
    chaine = "B";

    chaine += " \n";

    for (int i = 0; i < partie.get_nbJoueur(); i++) {  
        chaine += std::to_string(i);
        chaine += " " + std::to_string(partie.joueur[i].Tank->get_type());
        chaine += "\n";
    }
    
    chaine += " 1";
    // std::cout << chaine << "\n\n";
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
    int compt = 0;
    for(int i = 0; i<NB_JOUEUR; i++){
        if(partie.joueur[i].pV>0){
            compt++;
            partie.joueur_courant = i;
            partie.update();
        }
    }
}

void Server::majDead(char* buffer) {
    int offset = snprintf(buffer, 100, "D "); // Commence par "D "

    for (int i = 0; i < NB_JOUEUR && offset < 100; i++) {
        offset += snprintf(buffer + offset, 100 - offset, "%d ", partie.joueur[i].vivant ? 1 : 0);
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
        createSocketConnexion(ip[i]);
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

    partie.fondTexture.loadFromFile("Image/Keep_Off_The_Grass.png");
    partie.fondSprite.setTexture(partie.fondTexture);
    partie.fondSprite.setScale(2, 2);

    // TYPE DE TANK BLANC POUR TOUS LES JOUEURS AU DEBUT
    for (int i = 0; i < NB_JOUEUR; i ++) {
        partie.joueur[i].setTank(std::make_unique<Tank_blanc>());
    }
    std::cout << "tank en début de partie sur le server\n";
    partie.affiche_type_tank();

    // Thread dédié pour recevoir les événements des clients
    std::thread receptionThread([this]() {
        while (running && !partie.partieFinie.load()) {
            recevoirEvent();
        }
    });
 
    std::thread tankThread([this]() {       // dire à chauvet de mettre une condition pour arreter ce thread ca reduit la perf
        while (running && !partie.partieFinie.load()) {
            sendTankToClient();
        }
    });

    std::chrono::time_point<std::chrono::steady_clock> finPartieTime; // Stocker le moment de fin

    // Boucle principale du serveur
    while (running) {
        //recevoirEvent();
        processEvent();  
        sendToClient();
        // partie.affiche_type_tank();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // Ajout d'un délai pour éviter une boucle trop rapide

        if (partie.partieFinie.load()) {
            // Démarrer le timer dès que la partie est terminée
            if (finPartieTime.time_since_epoch().count() == 0) {
                finPartieTime = std::chrono::steady_clock::now();
                std::cout << "Fin de la partie signalée, arrêt dans 3 secondes..." << std::endl;
            }

            // Vérifier si 3 secondes se sont écoulées
            auto elapsed = std::chrono::steady_clock::now() - finPartieTime;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 5) {
                break; // Sortir de la boucle de jeu après 3 secondes
            }
        }
    }

    if (receptionThread.joinable()) {
        receptionThread.join();
    }

    if (tankThread.joinable()) {
        tankThread.join();
    }

    close(recieve_sockfd);  //pas toucher
}

int main() {
    Server server;
    server.startServer();
    return 0;
}
