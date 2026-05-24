#include "server.hpp"

Server::Server() {
    std::memset(sockfd, 0, sizeof(sockfd));
    std::cout << "[Server] Serveur initialise." << std::endl;

    for (int i = 0; i < 6; i++) {
        tank_recu[i] = 0;
    }
}

Server::~Server() {
    if (recieve_sockfd > 0) {
        close(recieve_sockfd);
    }
    if (send_sockfd > 0) {
        close(send_sockfd);
    }
    for (int i = 0; i < 6; ++i) {
        if (sockfd[i] > 0) {
            close(sockfd[i]);
        }
    }
    std::cout << "[Server] Serveur arrete." << std::endl;
}

void Server::createSocketConnexion(const std::string& ip, int port){
    send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_sockfd < 0) {
        perror("[Server] Echec de la creation du socket d'envoi");
        return;
    }
    memset(&send_clientaddr, 0, sizeof(send_clientaddr));
    send_clientaddr.sin_family = AF_INET;
    send_clientaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &send_clientaddr.sin_addr) <= 0) {
        perror("[Server] Echec de la conversion de l'IP");
        close(send_sockfd);
        return;
    }
    std::cout << "[Server] Socket d'envoi pret vers " << ip
              << ":" << port << std::endl;
}


void Server::createBindedSocket(int port){
    if (recieve_sockfd > 0) {
        close(recieve_sockfd);
        recieve_sockfd = -1;
    }

    recieve_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (recieve_sockfd < 0) {
        perror("[Server] Echec de la creation du socket de reception");
        return;
    }
    int opt = 1;
    setsockopt(recieve_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    memset(&recieve_clientaddr, 0, sizeof(recieve_clientaddr));
    recieve_clientaddr.sin_family = AF_INET;
    recieve_clientaddr.sin_port = htons(port);
    recieve_clientaddr.sin_addr.s_addr = INADDR_ANY;  // Accepte les connexions de n'importe quelle adresse

    // Liaison du socket au port spécifié
    if (bind(recieve_sockfd, (struct sockaddr*)&recieve_clientaddr, sizeof(recieve_clientaddr)) < 0) {
        perror("[Server] Echec du bind du socket de reception");
        close(recieve_sockfd);
        recieve_sockfd = -1;
        return;
    }
    char bindIpBuffer[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &recieve_clientaddr.sin_addr, bindIpBuffer, sizeof(bindIpBuffer));
    std::cout << "[Server] Socket de reception binde sur le port "
              << port << " (IP d'ecoute: " << bindIpBuffer << ")" << std::endl;
}

void Server::connexion() {
    socklen_t len = sizeof(recieve_clientaddr);
    char buffer[BUFFER_SIZE];

    createBindedSocket(SERVER_PORT);

    while (nb_joueur > partie.get_nbJoueur()) {
        std::cout << "[Server] En attente d'une demande de connexion sur le port "
                  << SERVER_PORT << std::endl;
        int n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_clientaddr, &len);

        if (n < 0) {
            perror("[Server] Erreur lors de la reception");
            continue;
        }
        buffer[n] = '\0'; // Ajoute un terminateur de chaîne
    
        const std::string message(buffer);
        const auto request = network::parseConnectionRequest(message);

        if (request.has_value()) {
            const std::string& pseudo = request->pseudo;
            char sourceIpBuffer[INET_ADDRSTRLEN] = {0};
            inet_ntop(AF_INET, &recieve_clientaddr.sin_addr, sourceIpBuffer, sizeof(sourceIpBuffer));
            const std::string ipJoueur(sourceIpBuffer);
            std::cout << "[Server] Demande de connexion recue. Pseudo: " << pseudo
                      << ", IP source: " << ipJoueur << std::endl;

            const int playerIndex = partie.get_nbJoueur();
            if (!partie.ajouteJoueur()) {
                std::cout << "[Server] Impossible d'ajouter un joueur supplementaire." << std::endl;
                continue;
            }

            ip[playerIndex] = ipJoueur;
            pseudos[playerIndex] = pseudo;
            partie.joueur[playerIndex].port = ntohs(recieve_clientaddr.sin_port);
            client[playerIndex] = recieve_clientaddr;

            const int gameplayPort = SERVER_PORT;
            const std::string buffer_config = network::makeConnectionConfig({playerIndex, gameplayPort, mode});
            createSocketConnexion(ipJoueur, partie.joueur[playerIndex].port);

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            sendto(send_sockfd, buffer_config.c_str(), buffer_config.size(), 0, (struct sockaddr*)&send_clientaddr, sizeof(send_clientaddr));
            close(send_sockfd);
            send_sockfd = -1;

            std::cout << "[Server] Joueur " << pseudo
                      << " enregistre avec l'id " << playerIndex
                      << " et le port client " << partie.joueur[playerIndex].port << std::endl;
            std::cout << "[Server] Nombre de joueurs connectes : "
                      << partie.get_nbJoueur() << std::endl;
        } else {
            std::cout << "[Server] Message de connexion invalide recu: "
                      << message << std::endl;
        }
    }
    
    std::cout << "[Server] Tous les joueurs sont connectes." << std::endl;
    // Préparation du message "P" contenant le nombre de joueurs et leurs pseudos
    network::LobbyState lobbyState;
    lobbyState.nbJoueurs = partie.get_nbJoueur();

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
                lobbyState.pseudos[i] = pseudos[i];
                lobbyState.equipes[i] = equipe_assignment[i];
                partie.joueur[i].equipe = equipe_assignment[i];
            }
        }
    } else {
        for (int i = 0; i < nb_joueur; ++i) {
            if (!pseudos[i].empty()) {
                lobbyState.pseudos[i] = pseudos[i];
            }
        }
    }

    const std::string msg_pret = network::makeLobbyState(lobbyState, mode);
    
    // Affichage du message final pour vérification
    std::cout << "[Server] Etat final du lobby prepare pour " << lobbyState.nbJoueurs
              << " joueurs." << std::endl;
    
    // Envoi du message "P" à tous les joueurs
    for (int i = 0; i < nb_joueur; i++) {
        createSocketConnexion(ip[i], partie.joueur[i].port);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        int sent = sendto(send_sockfd, msg_pret.c_str(), msg_pret.size(), 0, (struct sockaddr*)&send_clientaddr, sizeof(send_clientaddr));
        if (sent < 0) {
            perror("[Server] Erreur lors de l'envoi du message au joueur");
        } else {
            std::cout << "[Server] Message de lobby envoye au joueur "
                      << partie.joueur[i].id << " sur le port " << partie.joueur[i].port << ".\n";
        }
        close(send_sockfd);
        send_sockfd = -1;
    }
}

void Server::recevoirEvent() {
    socklen_t len = sizeof(recieve_clientaddr);

    memset(buffer, 0, sizeof(buffer));

    int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);

    if (receivedBytes < 0) {   //verifie
        std::cerr << "[Server] Erreur lors de la reception des donnees" << std::endl;
        return;
    }

    const auto input = network::parseInputState(std::string(buffer, receivedBytes));
    if (input.has_value() && input->playerId >= 0 && input->playerId < nb_joueur) {
        partie.joueur[input->playerId].applyInputState(input.value());
    }
}

void Server::sendToClient(){
    char buffer_explo[100];
    const std::string buffer_stat = network::makeStatsPacket(partie.stat);
    std::array<int, network::kMaxPlayers> hp{};

    partie.listexplosion.toCharArray(buffer_explo);
    for (int i = 0; i < network::kMaxPlayers; ++i) {
        hp[i] = partie.joueur[i].pV;
    }
    const std::string buffer_pV = network::makeHealthState(hp);

    for(int i = 0; i<nb_joueur; i++){   

        for(int j = 0; j<nb_joueur; j++){

            //recupère les tank/data processed de chaque joueur
            tank& tankjoueur = *(partie.joueur[j].Tank);
            const std::string buffer_processed_data = network::makeTankState({
                partie.joueur[j].id,
                tankjoueur.get_x(),
                tankjoueur.get_y(),
                tankjoueur.get_ori(),
                tankjoueur.getTourelleSprite().getRotation(),
                partie.utltiActive[j]
            });

            //les envoies à chaque autre client
            int n = sendto(sockfd[i], buffer_processed_data.c_str(), buffer_processed_data.size(), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
            //verifiacation
            if (n < 0) {
                perror(" Erreur lors de l'envoi des données du tank");
                return;
            } else {
                //debugage
                //std::cout << " Données processed envoyées au client : " << buffer_processed_data << std::endl;
                //std::cout << "Sur le port " << sockfd[0] << std::endl;
            }

            // ENVOIE DE LA LISTE D'OBUS
            std::string buffer_liste_obus;
            partie.string_obus(buffer_liste_obus);

            //les envoies à chaque autre client
            n = sendto(sockfd[i], buffer_liste_obus.c_str(), buffer_liste_obus.size(), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            
            //verifiacation
            if (n < 0) {
                perror(" Erreur lors de l'envoi des données du nombre d'obus");
                return;
            } else {
                //debugage
                //std::cout << "Données processed envoyées au client : " << buffer_nb_obus << std::endl;
                //std::cout << "Sur le port " << sockfd[0] << std::endl;
            }
        }

        //envoie des pV
        int n = sendto(sockfd[i], buffer_pV.c_str(), buffer_pV.size(), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
        //verifiacation
        if (n < 0) {
            perror(" Erreur lors de l'envoi des données des pV");
            return;
        } else {
            //debugage
            //std::cout << "Données processed envoyées au client : " << buffer_processed_data << std::endl;
            //std::cout << "Sur le port " << sockfd[0] << std::endl;
        }
        if(partie.listexplosion.nouveau){
            //std::cout << "buffer envoyé au client : " << buffer_explo << " (taille: " << strlen(buffer_explo) << ")\n";
            n = sendto(sockfd[i], buffer_explo, strlen(buffer_explo), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
            if (n < 0) {
                perror(" Erreur lors de l'envoi des données des explosions");
            } else {
                std::cout << " Explosion envoyée avec succès (" << n << " octets)\n";
            }

        }

        // Envoi du tableau stat
        n = sendto(sockfd[i], buffer_stat.data(), buffer_stat.size(), 0, 
                  (const struct sockaddr*)&client[i], sizeof(client[i]));
        if (n < 0) {
            perror(" Erreur lors de l'envoi des données du tableau stat");
        } else {
            //std::cout << " Tableau stat envoyé avec succès (" << n << " octets)\n";
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
            //std::cout << " Données processed envoyées au client : " << buffer_processed_data << std::endl;
            //std::cout << "Sur le port " << sockfd[0] << std::endl;
        }
    }     
}

void Server::sendTankRecu() {
    const std::string buffer = network::makeReadySignal();

    //les envoies à chaque autre client
    for (int i = 0; i < partie.get_nbJoueur(); i++) {
        int n = sendto(sockfd[i], buffer.c_str(), buffer.size(), 0, (const struct sockaddr*)&client[i], sizeof(client[i]));
        
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
    std::vector<std::pair<int, int>> tankTypes;

    for (int i = 0; i < partie.get_nbJoueur(); i++) {
        tankTypes.emplace_back(i, partie.joueur[i].Tank->get_type());
    }
    chaine = network::makeTankList(tankTypes);
}

void Server::init_choix_tank(){

    int nb_choix_recu = 0;
    const int totalPlayers = partie.get_nbJoueur();
    std::vector<int> joueursAyantChoisi;
    
    while(nb_choix_recu < totalPlayers){ // tant que tous les joueurs n'ont pas envoyé leur choix de tank
        socklen_t len = sizeof(recieve_clientaddr);

        // Initialiser le buffer pour éviter des problèmes de lecture
        memset(buffer, 0, sizeof(buffer));

        //recupère n'importe quel message sur le port 3000
        int receivedBytes = recvfrom(recieve_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&recieve_clientaddr, &len);

        if (receivedBytes < 0) {   //verifie
            std::cerr << " Erreur lors de la réception des données" << std::endl;
            return;
        }

        const auto choice = network::parseTankChoice(std::string(buffer, receivedBytes));
        if (choice.has_value()) {
            const int type_tank = choice->tankType;
            const int id = choice->playerId;
            // Stockage des données dans la partie
            printf("[Server] joueur %d à selectionner le tank : %d \n", id, type_tank);
            partie.joueur[id].assignTankByType(type_tank);
            joueursAyantChoisi.push_back(id);
            nb_choix_recu ++;

            const std::string progressMessage = network::makeTankChoiceProgress(nb_choix_recu);
            printf("[Server] buffer de confirmation %s", progressMessage.c_str());

            //envoie du nb de tank reçu à tous les joueurs ayant choisi (dont le joueur qui a envoyé son choix)
            for (const int indice : joueursAyantChoisi) {
                sendto(sockfd[indice], progressMessage.c_str(), progressMessage.size(), 0, (const struct sockaddr*)&client[indice], sizeof(client[indice]));
            }
            // partie.affiche_type_tank();
        } 
        else{
            std::cout<<"[Server]  message bizarre recu pour le choix des tank"<<buffer<<std::endl;
        }
        
    }

    std::string listeTank;
    string_tank(listeTank);
    std::cout<<listeTank<<std::endl;
    //envoi de tous les tanks (au complet) aux joueurs
    for (int i = 0; i < totalPlayers; i++) {

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
    std::cout << "Initialisation des sockfd et clientaddr de chaque client\n";

    for(int i=0; i<nb_joueur; i++){
        createSocketConnexion(ip[i], partie.joueur[i].port);
        sockfd[i] = send_sockfd;
        client[i] = send_clientaddr;
        std::cout << "[Server] Canal de jeu configure pour le joueur " << i
                  << " vers le port client " << partie.joueur[i].port << std::endl;
    }
    send_sockfd = -1;
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
