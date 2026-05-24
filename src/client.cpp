#include "client.hpp"

Client::Client() : num_port(SERVER_PORT_CONNEXION), sockfd(-1), nbJoueur(0), mode(1), test(0) {
    std::memset(&servaddr, 0, sizeof(servaddr));
    std::memset(&recieve_servaddr, 0, sizeof(recieve_servaddr));
    std::cout << "[Client] Client initialise. Port de connexion par defaut: " << num_port << std::endl;
}

Client::~Client() {
    closeSockets();
}

void Client::closeSockets() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }

    receive_port = 0;
}

void Client::createSocket() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[Client] Echec de la creation du socket d'envoi");
        return;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&recieve_servaddr, 0, sizeof(recieve_servaddr));
    recieve_servaddr.sin_family = AF_INET;
    recieve_servaddr.sin_port = htons(0);
    recieve_servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, reinterpret_cast<struct sockaddr*>(&recieve_servaddr), sizeof(recieve_servaddr)) < 0) {
        perror("[Client] Echec du bind du socket client");
        close(sockfd);
        sockfd = -1;
        return;
    }

    socklen_t localAddrLen = sizeof(recieve_servaddr);
    if (getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&recieve_servaddr), &localAddrLen) < 0) {
        perror("[Client] Echec de la recuperation du port local");
        close(sockfd);
        sockfd = -1;
        return;
    }

    receive_port = ntohs(recieve_servaddr.sin_port);

    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(num_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr) <= 0) {
        perror("[Client] Echec de la conversion de l'IP du serveur");
        close(sockfd);
        sockfd = -1;
        return;
    }

    std::cout << "[Client] Socket d'envoi configure vers " << server_ip
              << ":" << num_port << std::endl;
    std::cout << "[Client] Socket de reception binde sur le port local "
              << receive_port << std::endl;
}

void Client::sendMessageToServer(const std::string& message) {
    if (sockfd < 0) {
        createSocket();
    }

    if (sockfd < 0) {
        return;
    }

    const ssize_t sent = sendto(sockfd, message.c_str(), message.size(), 0, reinterpret_cast<const struct sockaddr*>(&servaddr), sizeof(servaddr));
    if (sent < 0) {
        perror("[Client] Erreur lors de l'envoi au serveur");
    }
}

bool Client::sendInput(const Joueur& joueurLocal) {
    sendMessageToServer(network::makeInputState(joueurLocal.toInputState()));
    return sockfd >= 0;
}

bool Client::sendTankChoice(int playerId, int tankType) {
    sendMessageToServer(network::makeTankChoice({playerId, tankType}));
    return sockfd >= 0;
}

bool Client::receivePacket(std::string& packet) {
    char buffer[BUFFER_SIZE];
    ssize_t receivedBytes = 0;

    if (!receiveRawPacket(buffer, sizeof(buffer), receivedBytes)) {
        return false;
    }

    packet.assign(buffer, static_cast<std::size_t>(receivedBytes));
    return true;
}

bool Client::receiveRawPacket(char* buffer, std::size_t bufferSize, ssize_t& receivedBytes) {
    if (sockfd < 0) {
        return false;
    }

    socklen_t addrLen = sizeof(recieve_servaddr);
    receivedBytes = recvfrom(sockfd, buffer, bufferSize, 0, reinterpret_cast<struct sockaddr*>(&recieve_servaddr), &addrLen);

    if (receivedBytes < 0) {
        perror("[Client] Erreur lors de la reception");
        return false;
    }

    std::cout << "[Client] Paquet recu (" << receivedBytes << " octets)" << std::endl;
    return true;
}

void Client::configureConnection(const std::string& ip, const std::string& pseudo) {
    {
        std::lock_guard<std::mutex> lock(connectionMutex);
        server_ip = ip;
        joueur.pseudo = pseudo;
        connectionConfigured = true;
        ipValide.store(true);
    }

    connectionCv.notify_one();
}

void Client::initconnexion() {
    std::string configuredIp;
    std::string configuredPseudo;

    {
        std::unique_lock<std::mutex> lock(connectionMutex);
        std::cout << "[Client] En attente de la configuration de l'IP du serveur..." << std::endl;
        connectionCv.wait(lock, [this]() { return connectionConfigured; });
        configuredIp = server_ip;
        configuredPseudo = joueur.pseudo;
    }

    std::cout << "[Client] Configuration IP detectee. Sortie de l'attente." << std::endl;

    etatConnexion.store(-1);
    num_port = SERVER_PORT_CONNEXION;
    std::cout << "[Client] IP configuree: " << configuredIp << ". Demarrage de la connexion." << std::endl;
    createSocket();

    if (sockfd < 0) {
        return;
    }

    const network::ConnectionRequest request{getLocalIPAddress(), configuredPseudo};
    std::cout << "[Client] Envoi de la demande de connexion pour le pseudo: "
              << configuredPseudo << std::endl;
    sendMessageToServer(network::makeConnectionRequest(request));

    std::string packet;
    if (!receivePacket(packet)) {
        closeSockets();
        return;
    }

    const auto config = network::parseConnectionConfig(packet);
    if (!config.has_value()) {
        std::cerr << "[Client] Configuration serveur invalide : " << packet << std::endl;
        closeSockets();
        return;
    }

    num_port = config->port;
    mode = config->mode;
    joueur.id = config->playerId;
    std::cout << "[Client] Configuration recue. Port attribue: " << num_port
              << ", mode: " << mode << ", joueur id: " << joueur.id << std::endl;

    etatConnexion.store(0);
    std::cout << "[Client] Connexion validee. En attente du lobby..." << std::endl;

    if (!receivePacket(packet)) {
        closeSockets();
        return;
    }

    const auto lobbyState = network::parseLobbyState(packet, mode);
    if (!lobbyState.has_value()) {
        std::cerr << "[Client] Etat du lobby invalide : " << packet << std::endl;
        closeSockets();
        return;
    }

    nbJoueur = lobbyState->nbJoueurs;
    pseudos = lobbyState->pseudos;
    equipe = lobbyState->equipes;

    if (mode == 2 && joueur.id >= 0 && joueur.id < nbJoueur) {
        joueur.equipe = equipe[joueur.id];
    }

    servaddr.sin_port = htons(SERVER_PORT_CONNEXION);
    etatConnexion.store(1);
    std::cout << "[Client] Lobby recu. Nombre de joueurs: " << nbJoueur
              << ". Connexion terminee." << std::endl;
}

int Client::get_etatConnexion() {
    return etatConnexion.load();
}

int Client::getReceivePort() const {
    return receive_port;
}

const std::array<std::string, network::kMaxPlayers>& Client::getPseudos() const {
    return pseudos;
}

const std::array<int, network::kMaxPlayers>& Client::getEquipes() const {
    return equipe;
}

std::string Client::getLocalIPAddress() {
    if (test) {
        return "127.0.0.1";
    }

    return sf::IpAddress::getLocalAddress().toString();
}
