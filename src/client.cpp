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
    if (inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr) <= 0) {
        perror("Échec de la conversion de l'IP");
        close(sockfd);
        return;
    }
}

void Client::createBindedSocket(){

    recieve_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (recieve_sockfd < 0) {
        perror("Échec de la création du socket");
        return;
    }

    int opt = 1;
    setsockopt(recieve_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configuration de l'adresse du serveur
    memset(&recieve_servaddr, 0, sizeof(recieve_servaddr));
    recieve_servaddr.sin_family = AF_INET;
    recieve_servaddr.sin_port = htons(num_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr) <= 0) {
        perror("Échec de la conversion de l'IP");
        close(sockfd);
        return;
    }
    // Liaison du socket au port spécifié
    if (bind(recieve_sockfd, (struct sockaddr*)&recieve_servaddr, sizeof(recieve_servaddr)) < 0) {
        perror("Échec du bind du socket");
        close(recieve_sockfd);
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

void Client::initconnexion() {

    while(!ipValide){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printf("la fete commence");
    etatConnexion = -1; //connexion avec le server
    int received_port;

    std::string message = "C " + getLocalIPAddress(); //recupération de l'adresse IP
    printf("%s", message.c_str());
    socklen_t recieve_len = sizeof(recieve_servaddr);

    sendMessageToServer(message);  // Envoi du premier message sur le port 3000
    std::cout << "Requete et IP envoyee au serveur.\n";

    num_port = SERVER_PORT; //3000
    createBindedSocket();

    int n = recvfrom(recieve_sockfd, &received_port, sizeof(received_port), 0, (struct sockaddr*)&recieve_servaddr, &recieve_len); // Réception du nouveau port du serveur
    if (n < 0) {
        perror("Erreur lors de la réception du port");
        close(recieve_sockfd);
        return;
    }
    close(recieve_sockfd);

    num_port = ntohs(received_port);
    std::cout << "Nouveau port reçu du serveur : " << num_port << "\n";
    joueur.id = num_port - 3001;
    sleep(0.001);
    sendMessageToServer("T");  // Envoi du message test sur le nouveau port
    std::cout << "Message 'T' envoyé au serveur sur le port " << num_port << ".\n";

    createBindedSocket();
    char buffer[1024];

    // Attente de la confirmation du serveur
    n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_servaddr, &recieve_len);
    if (n < 0) {
        perror("Erreur lors de la réception de la confirmation");
        close(recieve_sockfd);
        return;
    }
    buffer[n] = '\0';
    std::cout << "Confirmation du serveur : " << buffer << "\n";
    etatConnexion = 0; //attente des joueurs

    // Attente du message "P" du serveur
    n = recvfrom(recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recieve_servaddr, &recieve_len);
    if (n < 0) {
        perror("Erreur lors de la réception de la confirmation");
        close(recieve_sockfd);
        return;
    }

    if (buffer[0] == 'P') {
        int parsed = sscanf(buffer, "P %d", &nbJoueur);
        
        if (parsed == 1) {  // Vérifie que sscanf a bien trouvé un nombre
            std::cout << "CLIENT nb joueurs : " << nbJoueur << std::endl;
            std::cout << "✅ Serveur prêt !\n";
            etatConnexion = 1;
        } else {
            std::cerr << "❌ Erreur : format du message incorrect (" << buffer << ")" << std::endl;
        }
    }
    else{
        std::cout << "Mauvais message du serveur !\n";
    }
    
    close(recieve_sockfd);
}

void Client::sendData(){
    char buffer[1024];

    snprintf(buffer, sizeof(buffer), "T %.2f %.2f %.2f", joueur.Tank->get_x(), joueur.Tank->get_y(), joueur.Tank->get_ori());

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
            joueur.Tank->set_x(x);
            joueur.Tank->set_y(y);
            joueur.Tank->set_ori(ori);
        } else {
            std::cerr << "Format de message invalide !" << std::endl;
        }
    } else {
        std::cerr << "Message non valide reçu : " << buffer << std::endl;
    }

}

int Client::get_etatConnexion(){
    return etatConnexion;
}

std::string Client::getLocalIPAddress() {
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    return ip.toString();
}
