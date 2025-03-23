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
    
    while (nb_joueur > partie.get_nbJoueur()) {
        port_connexion = SERVER_PORT; // Port par défaut : 3000
        createBindedSocket(); // Crée un socket lié au port 3000
    
        // Attente d'un message "C" du client
        int n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_clientaddr, &len);
        close(recieve_sockfd); // Ferme le socket après réception
    
        if (n < 0) {
            perror("Erreur lors de la réception");
            continue;
        }
        buffer[n] = '\0'; // Ajoute un terminateur de chaîne
    
        if (buffer[0] == 'C') {
            // Extraction de l'IP et du pseudo du message
            std::string ipJoueur = extractIP(buffer);
            std::string pseudo = extractPseudo(buffer);
            printf("Voici le pseudo : %s\n", pseudo.c_str());
            std::cout << "Message 'C' reçu. Récupération de l'IP et attribution d'un port...\n";
    
            // Ajoute un nouveau joueur à la partie
            if (!partie.ajouteJoueur()) {
                continue; // Si l'ajout échoue, on passe au prochain client
            }
    
            // Attribution d'un nouveau port au client
            int port_to_send = 3000 + partie.get_nbJoueur(); // Ports 3001 à 3007 pour 6 joueurs
            char buffer_config[100];
            sprintf(buffer_config,"C %d %d %d", port_to_send, mode, 1);
            createSocketConnexion(ipJoueur); // Crée un socket pour communiquer avec le client
    
            // Ajoute l'IP et le pseudo du joueur aux tableaux
            ip[partie.get_nbJoueur() - 1] = ipJoueur;
            pseudos[partie.get_nbJoueur() - 1] = pseudo;
            
            // Envoi du nouveau port au client
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // On attend que le client soit pret
            sendto(send_sockfd, &buffer_config, sizeof(buffer_config), 0, (struct sockaddr*)&send_clientaddr, send_len);
            close(send_sockfd); // Ferme le socket après envoi
            std::cout << "Port " << port_to_send << " envoyé au client\n";
    
            // Passage au nouveau port pour la suite de la communication
            port_connexion = 3000 + partie.get_nbJoueur();
            createBindedSocket(); // Crée un socket lié au nouveau port
    
            // Attente du message "T" du client
            n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_clientaddr, &len);
            if (n < 0) {
                perror("Erreur lors de la réception de 'T'");
                close(recieve_sockfd);
                continue;
            }
            buffer[n] = '\0';
    
            if (buffer[0] == 'T') {
                std::cout << "Client validé sur le port " << port_connexion << ".\n";
                // Envoi d'une confirmation au client
                std::string confirmation = "Connexion réussie !";
                createSocketConnexion(ipJoueur);
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // On attend que le client soit pret
                sendto(send_sockfd, confirmation.c_str(), confirmation.length(), 0, (struct sockaddr*)&send_clientaddr, send_len);
                close(send_sockfd); // Ferme le socket après envoi
            } else {
                std::cout << "Échec d'initialisation avec le client\n";
            }
    
            close(recieve_sockfd); // Ferme le socket de réception
            std::cout << "Nombre de joueurs : " << partie.get_nbJoueur() << std::endl;
        } else {
            std::cout << "En attente du bon nombre de joueurs...\n";
        }
    }
    
    std::cout<<"tout le monde est la"<<std::endl;
    // Préparation du message "P" contenant le nombre de joueurs et leurs pseudos
    char msg_pret[256];
    sprintf(msg_pret, "P %d", partie.get_nbJoueur());
    
    // Ajout des pseudos (et equipe) au message
    if (mode == 2) { // Mode par équipe

        std::vector<int> equipe_assignment(nb_joueur); //vecteur dans lequel vont etre tirés les joueurs

        // Remplir le vecteur avec le bon nombre de 1 et 2
        int nb_equipe1 = (nb_joueur + 1) / 2; // Plus de joueurs dans équipe 1 si impair
        for (int i = 0; i < nb_equipe1; ++i) equipe_assignment[i] = 1;
        for (int i = nb_equipe1; i < nb_joueur; ++i) equipe_assignment[i] = 2;
        
        // Mélanger aléatoirement les affectations d'équipe
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(equipe_assignment.begin(), equipe_assignment.end(), g);
        
        for (int i = 0; i < nb_joueur; ++i) {
            if (!pseudos[i].empty()) {
                strcat(msg_pret, " ");
                strcat(msg_pret, pseudos[i].c_str());
                strcat(msg_pret, " ");
                
                char equipe_str[2];
                sprintf(equipe_str, "%d", equipe_assignment[i]);
                strcat(msg_pret, equipe_str);
                partie.joueur[i].equipe = equipe_assignment[i];
            }
        }
    } else {
        for (int i = 0; i < nb_joueur; ++i) {
            if (!pseudos[i].empty()) {
                strcat(msg_pret, " ");
                strcat(msg_pret, pseudos[i].c_str());
            }
        }
    }
    
    // Affichage du message final pour vérification
    printf("Message final : %s\n", msg_pret);
    
    // Envoi du message "P" à tous les joueurs
    for (int i = 0; i < nb_joueur; i++) {
        port_connexion = partie.joueur[i].port;
        createSocketConnexion(ip[i]); // Crée un socket pour communiquer avec le joueur

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // On attend que le client soit pret
        int sent = sendto(send_sockfd, msg_pret, strlen(msg_pret), 0, (struct sockaddr*)&send_clientaddr, sizeof(send_clientaddr));
        if (sent < 0) {
            perror("Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "Message 'P' envoyé au joueur " << partie.joueur[i].id << " sur le port " << port_connexion << ".\n";
        }
        close(send_sockfd); // Ferme le socket après envoi
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
        int i, z, q, s, d, x, mouseX, mouseY, clicked; 
        int valuesRead = sscanf(buffer, "A %d %d %d %d %d %d %d %d %d", &i, &z, &q, &s, &d, &x, &mouseX, &mouseY, &clicked);

        if (valuesRead != 9) { 
            perror("Erreur de lecture avec sscanf, c'est la sauce !");
            exit(EXIT_FAILURE);
        }

        // Stockage des données dans le tableau de la partie
        partie.joueur[i].Zpressed = (z != 0);
        partie.joueur[i].Qpressed = (q != 0);
        partie.joueur[i].Spressed = (s != 0);
        partie.joueur[i].Dpressed = (d != 0);
        partie.joueur[i].Xpressed = (x != 0);
        partie.joueur[i].worldMousePos = sf::Vector2f(mouseX, mouseY);
        partie.joueur[i].Clicked = (clicked != 0);

        // Affichage des données reçues pour débogage
        // std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
        // std::cout << "Touches : Z=" << partie.joueur[i].Zpressed << " Q=" << partie.joueur[i].Qpressed << " S=" << partie.joueur[i].Spressed << " D=" << partie.joueur[i].Dpressed << std::endl;
        // std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x << " Y=" << partie.joueur[i].worldMousePos.y << "clicked : "<< partie.joueur[i].Clicked << std::endl; 
    }
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

    for(int i = 0; i<nb_joueur; i++){   

        for(int j = 0; j<nb_joueur; j++){

            //recupère les tank/data processed de chaque joueur
            tank& tankjoueur = *(partie.joueur[j].Tank);
            sprintf(buffer_processed_data, "T %d %f %f %f %f %d %d", partie.joueur[j].id, tankjoueur.get_x(), tankjoueur.get_y(), tankjoueur.get_ori(), tankjoueur.getTourelleSprite().getRotation(), partie.utltiActive[j], 1);

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
            //std::cout << "buffer envoyé au client : " << buffer_explo << " (taille: " << strlen(buffer_explo) << ")\n";
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

        std::ostringstream oss;
        oss << "R";
        
        for (int ligne = 0; ligne < 4; ++ligne) {
            for (int col = 0; col < 3; ++col) {
                oss << " " << partie.regen[ligne][col];
            }
        }

        std::string buffer_regen = oss.str();
        //std::cout << buffer_regen << std::endl;

        n = sendto(sockfd[i], buffer_regen.c_str(), buffer_regen.size(), 0, 
                            (const struct sockaddr*)&client[i], sizeof(client[i]));

        if (n < 0) {
            perror("Erreur lors de l'envoi des données des regens");
        } else {
            //std::cout << " Données des regens envoyées avec succès (" << n << " octets)\n";
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

    //les envoies à chaque autre client
    for (int i = 0; i < partie.get_nbJoueur(); i++) {
        int n = sendto(sockfd[i], buffer, strlen(buffer), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
        
        //verifiacation
        if (n < 0) {
            perror("❌ Erreur lors de l'envoi des données du tank");
            return;
        } else {
            //debugage
            std::cout <<"le server envoie le feu vert à"<< i<<std::endl;
        }
    }     
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

void Server::string_tank(std::string& chaine) {
    chaine = "B"; // Préfixe

    for (int i = 0; i < partie.get_nbJoueur(); i++) {
        chaine += " " + std::to_string(i); // ID du joueur
        chaine += " " + std::to_string(partie.joueur[i].Tank->get_type()); // Type de tank
    }

    // Exemple de sortie : "B 0 1 1 2 2 3"
}

void Server::init_choix_tank(){

    int nb_choix_recu = 0;
    int nb_joueur = partie.get_nbJoueur();

    int tabChoixRecu[nb_joueur]; //tableau qui va contenir les id des joueurs ayant envoyé leur choix
    
    while(nb_choix_recu < nb_joueur){ // tant que tous les joueurs n'ont pas envoyé leur choix de tank
        socklen_t len = sizeof(recieve_clientaddr);

        // Initialiser le buffer pour éviter des problèmes de lecture
        memset(buffer, 0, sizeof(buffer));

        //recupère n'importe quel message sur le port 3000
        int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);

        if (receivedBytes < 0) {   //verifie
            std::cerr << "❌ Erreur lors de la réception des données" << std::endl;
            return;
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
                partie.joueur[id].setTank(std::make_unique<Tank_classique>());
            }
            else if (type_tank == 2) {
                partie.joueur[id].setTank(std::make_unique<Tank_rapide>());
            }
            else if (type_tank == 3) {
                partie.joueur[id].setTank(std::make_unique<Tank_healer>());
            }
            else if (type_tank == 4) {
                partie.joueur[id].setTank(std::make_unique<Tank_mortier>());
            }
            else if (type_tank == 5) {
                partie.joueur[id].setTank(std::make_unique<Tank_solide>());
            }
            else if (type_tank == 6) {
                partie.joueur[id].setTank(std::make_unique<Tank_sniper>());
            }

            tabChoixRecu[nb_choix_recu] = id;  //stockage de l'id du joueur qui a envoyé son choix

            partie.joueur[id].pV = partie.joueur[id].Tank->get_vie();
            nb_choix_recu ++;

            char buffer[256]; 
            snprintf(buffer, sizeof(buffer), "N %d %d\n", nb_choix_recu, 1);
            printf("buffer de confirmation %s", buffer);

            //envoie du nb de tank reçu à tous les joueurs ayant choisi (dont le joueur qui a envoyé son choix)
            for(int i = 0; i < nb_choix_recu; i++){
                int indice = tabChoixRecu[i];
                sendto(sockfd[indice], buffer, strlen(buffer), 0, (const struct sockaddr*)&client[indice], sizeof(client[indice]));
            }
            // partie.affiche_type_tank();
        } 
        else{
            std::cout<<"message bizarre recu pour le choix des tank"<<buffer<<std::endl;
        }
        // Affichage des données reçues pour débogage
        /*std::cout << "✅ Données reçues pour le joueur " << i << " :\n";
        std::cout << "Touches : Z=" << partie.joueur[i].Zpressed << " Q=" << partie.joueur[i].Qpressed << " S=" << partie.joueur[i].Spressed << " D=" << partie.joueur[i].Dpressed << std::endl;
        std::cout << "Souris : X=" << partie.joueur[i].worldMousePos.x << " Y=" << partie.joueur[i].worldMousePos.y << "clicked : "<< partie.joueur[i].Clicked << std::endl;
        */
    }

    std::string listeTank;
    string_tank(listeTank);
    std::cout<<listeTank<<std::endl;
    //envoi de tous les tanks (au complet) aux joueurs
    for (int i = 0; i < nb_joueur; i++) {

        const char* message = listeTank.c_str();
        size_t message_size = listeTank.size();

        int n = sendto(sockfd[i], message, message_size, 0, (const struct sockaddr*)&client[i], sizeof(client[i]));

        if (n < 0) {
            perror("Erreur lors de l'envoi des données du tank");
            return;
        }else {
            std::cout <<"le server envoie le feu vert à joueur "<<i<<std::endl;
        }
    }     
}

void Server::init_send_fd(){

    port_connexion = 3000;
    createBindedSocket();

    std::cout << "Initialisation des sockfd et clientaddr de chaque client\n";

    int recup = send_sockfd;              //pour ne pas tout casse
    int port = port_connexion;

    for(int i=0; i<nb_joueur; i++){
        port_connexion = 3001 + i;
        createSocketConnexion(ip[i]);
        sockfd[i] = send_sockfd;
        client[i] = send_clientaddr;
    }

    send_sockfd = recup;
    port_connexion = port; 
    std::cout << "Fin de l'initialisation\n";
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

void Server::updateRegen(){

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - timerRegen) > std::chrono::seconds(10)) {
        timerRegen = now; // Reset du timer de régénération

        //mise à jour des spawn pris ou pas pris sinon peut spawner 2 fois au meme endroit
        for(int i = 0; i<4; i++){ 
            for(int j = 0; j<6; j++){
                if((partie.regen[i][0] == 0) && (partie.regen[i][1] == spawnRegen[j][0]) && (partie.regen[i][2] == spawnRegen[j][1])){
                    spawnRegen[j][2] = 0;
                }
            }
        }

        for(int i = 0; i<4; i++){

            if(partie.regen[i][0] == 0){
                bool dejapris = true;
                int randomSpawn = 0;

                while(dejapris){
                    randomSpawn = rand() % 5; // Génère un nombre entre 1 et 6
                    if(spawnRegen[randomSpawn][2] == 0){
                        spawnRegen[randomSpawn][2] = 1;
                        std::cout<<"nouveau spawn"<<std::endl;
                        dejapris = false;
                    }
                }
                std::cout << "10 secondes écoulées, régénération déclenchée en x: "<<spawnRegen[randomSpawn][0]<<" y: "<< spawnRegen[randomSpawn][1]<<std::endl;
                partie.regen[i][0] = 1;
                partie.regen[i][1] = spawnRegen[randomSpawn][0]; //nouveau x
                partie.regen[i][2] = spawnRegen[randomSpawn][1]; // nouveau y
                sf::Vector2f pos(partie.regen[i][1], partie.regen[i][2]);
                partie.regenSprites[i].setPosition(pos);
                break;
            }
        }

    } 
}

void Server::processEvent(){

    updateRegen();

    int compt = 0;
    for(int i = 0; i<nb_joueur; i++){

        if(partie.joueur[i].pV>0){

            compt++;
            partie.joueur_courant = i;

            if(partie.joueur[i].Xpressed && partie.utltiActive[i] != 1 && partie.utltiActive[i] == 0){ //cas où X pressé et l'ulti n'est pas encore activé 
                
                chronoUlti[i][0] = timer;
                chronoUlti[i][1] = timer + std::chrono::seconds(3);
                partie.utltiActive[i] = 1;
                
                if(partie.joueur[i].Tank->get_type() == 1){
                    partie.joueur[i].Tank->ultiClassicUse = false;
                }

                std::cout<<"activation de l'utli du joueur "<<i<<std::endl;
            }

            if(partie.utltiActive[i] == 1){ //cas où l'ulti actif
                
                if(timer > chronoUlti[i][1]){

                    partie.utltiActive[i] = -1;
                    std::cout<<"Desactivation de l'utli du joueur "<<i<<std::endl;

                }
                
            }

            partie.update();
        }
    }
}

void Server::majDead(char* buffer) {
    int offset = snprintf(buffer, 100, "D "); // Commence par "D "

    for (int i = 0; i < nb_joueur && offset < 100; i++) {
        offset += snprintf(buffer + offset, 100 - offset, "%d ", partie.joueur[i].vivant ? 1 : 0);
    }
}

void Server::init_Spawn(){
    int tab_deja[6] = {0,0,0,0,0,0};
    for(int i = 0; i < partie.get_nbJoueur(); i++){
        int dejapris = true;
        int random_spawn;
        while(dejapris){
            random_spawn = rand() % 6; 
            if(tab_deja[random_spawn]==0){
                tab_deja[random_spawn] = 1;
                dejapris = false;
            }
            std::cout<<random_spawn<<std::endl;
        }
        partie.joueur[i].Tank->set_x(spawn[random_spawn][0]);
        partie.joueur[i].Tank->set_y(spawn[random_spawn][1]);
        partie.joueur[i].Tank->updateHitbox(); //on met à jour la hitBox
        partie.hitboxes.push_back(partie.joueur[i].Tank->get_tankHitbox());
    }
}

void Server::startServer() {

    partie.client.mode = mode; 

    connexion();  // Lancement de la gestion des connexions
    init_send_fd();
    srand(time(0)); 
    sf::Texture texturetest;
    texturetest.loadFromFile("Image/classique/base_classique.png");
    partie.testSprite.setTexture(texturetest);
    partie.testSprite.setScale(0.08f, 0.08f);
    partie.testSprite.setPosition(300, 300);

    partie.fondTexture.loadFromFile("Image/cartef.png");
    partie.fondSprite.setTexture(partie.fondTexture);
    partie.fondSprite.setScale(2, 2);

    // TYPE DE TANK BLANC POUR TOUS LES JOUEURS AU DEBUT
    for (int i = 0; i < nb_joueur; i ++) {
        partie.joueur[i].setTank(std::make_unique<Tank_blanc>());
    }
    
    partie.affiche_type_tank();

    init_choix_tank();
    
    init_Spawn();
    // Thread dédié pour recevoir les événements des clients
    std::thread receptionThread([this]() {
        while (running && !partie.partieFinie.load()) {
            recevoirEvent();
        }
    });

    std::chrono::time_point<std::chrono::steady_clock> finPartieTime; // Stocker le moment de fin
    //initialisation des chronos Ulti
     for (int i = 0; i < 6; ++i) {
        chronoUlti[i][0] = timer;
        chronoUlti[i][1] = timer;
    }

    std::cout << "tank en début de partie sur le server\n";

    // Boucle principale du serveur
    while (running) {
        //recevoirEvent();
        timer = std::chrono::steady_clock::now();
        processEvent();  
        sendToClient();
        // partie.affiche_type_tank();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // Ajout d'un délai pour éviter une boucle trop rapide

        if (partie.partieFinie.load()) {
            // Démarrer le timerr dès que la partie est terminée
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

    close(recieve_sockfd);  //pas toucher
}

std::string Server::extractPseudo(const std::string& message) {
    size_t pos = message.find("N: ");
    if (pos != std::string::npos) {
        return message.substr(pos + 3); // 4 pour sauter "N:  "
    }
    return "Batar"; // Retourne une chaîne vide si "N:  " n'est pas trouvé
}

int main(int argc, char* argv[]) {
    std::ofstream pidFile("server.pid");
    pidFile << getpid();  // Stocke son propre PID
    pidFile.close();

    Server server;

    // Vérifie s'il y a un argument et le convertit en entier
    int nbJoueur = NB_JOUEUR; // Valeur par défaut
    int m = -1;

    if (argc > 2) {
        nbJoueur = std::atoi(argv[1]);
        m = std::atoi(argv[2]);
        // Vérifie que nbJoueur est bien entre 0 et 6
        if (nbJoueur < 0 || nbJoueur > 6) {
            std::cerr << "Erreur: nbJoueur doit être compris entre 0 et 6." << std::endl;
            return 1;  // Quitte le programme avec un code d'erreur
        }
        std::cout<<"le server est lancé pour "<<nbJoueur<<" joueurs"<<std::endl;

        if(m != 1 && m != 2){
            std::cerr << "Erreur: MODE doit être compris entre 1 et 2." << std::endl;
            return 1;  // Quitte le programme avec un code d'erreur
        }
        std::cout<<"le server est lancé mode "<<m<<" (1 MG 2 MME)"<<std::endl;
    }

    server.mode = m;
    server.nb_joueur = nbJoueur;
    server.startServer();
    
    return 0;
}
