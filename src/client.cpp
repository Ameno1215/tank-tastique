#include "client.hpp"

Client::Client() : num_port(SERVER_PORT_CONNEXION), sockfd(-1), nbJoueur(0), mode(1), test(0) {
    std::memset(&servaddr, 0, sizeof(servaddr));
    std::memset(&recieve_servaddr, 0, sizeof(recieve_servaddr));
    LOG_F(INFO, "[Client] Client initialise. Port de connexion par defaut: %d", num_port);
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
        PLOG_F(ERROR, "[Client] Echec de la creation du socket d'envoi");
        return;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&recieve_servaddr, 0, sizeof(recieve_servaddr));
    recieve_servaddr.sin_family = AF_INET;
    recieve_servaddr.sin_port = htons(0);
    recieve_servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, reinterpret_cast<struct sockaddr*>(&recieve_servaddr), sizeof(recieve_servaddr)) < 0) {
        PLOG_F(ERROR, "[Client] Echec du bind du socket client");
        close(sockfd);
        sockfd = -1;
        return;
    }

    socklen_t localAddrLen = sizeof(recieve_servaddr);
    if (getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&recieve_servaddr), &localAddrLen) < 0) {
        PLOG_F(ERROR, "[Client] Echec de la recuperation du port local");
        close(sockfd);
        sockfd = -1;
        return;
    }

    receive_port = ntohs(recieve_servaddr.sin_port);

    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(num_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr) <= 0) {
        PLOG_F(ERROR, "[Client] Echec de la conversion de l'IP du serveur");
        close(sockfd);
        sockfd = -1;
        return;
    }

    LOG_F(INFO, "[Client] Socket d'envoi configure vers %s:%d", server_ip.c_str(), num_port);
    LOG_F(DEBUG, "[Client] Socket de reception binde sur le port local %d", receive_port);
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
        PLOG_F(ERROR, "[Client] Erreur lors de l'envoi au serveur");
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
        PLOG_F(ERROR, "[Client] Erreur lors de la reception");
        return false;
    }

    LOG_F(DEBUG, "[Client] Paquet recu (%zd octets)", receivedBytes);
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
        LOG_F(DEBUG, "[Client] En attente de la configuration de l'IP du serveur");
        connectionCv.wait(lock, [this]() { return connectionConfigured; });
        configuredIp = server_ip;
        configuredPseudo = joueur.pseudo;
    }

    LOG_F(INFO, "[Client] Configuration IP detectee. Sortie de l'attente");

    etatConnexion.store(-1);
    num_port = SERVER_PORT_CONNEXION;
    LOG_F(INFO, "[Client] IP configuree: %s. Demarrage de la connexion", configuredIp.c_str());
    createSocket();

    if (sockfd < 0) {
        return;
    }

    const network::ConnectionRequest request{getLocalIPAddress(), configuredPseudo};
    LOG_F(INFO, "[Client] Envoi de la demande de connexion pour le pseudo: %s", configuredPseudo.c_str());
    sendMessageToServer(network::makeConnectionRequest(request));

    std::string packet;
    if (!receivePacket(packet)) {
        closeSockets();
        return;
    }

    const auto config = network::parseConnectionConfig(packet);
    if (!config.has_value()) {
        LOG_F(ERROR, "[Client] Configuration serveur invalide : %s", packet.c_str());
        closeSockets();
        return;
    }

    num_port = config->port;
    mode = config->mode;
    joueur.id = config->playerId;
    LOG_F(INFO, "[Client] Configuration recue. Port attribue: %d, mode: %d, joueur id: %d", num_port, mode, joueur.id);

    etatConnexion.store(0);
    LOG_F(INFO, "[Client] Connexion validee. En attente du lobby");

    if (!receivePacket(packet)) {
        closeSockets();
        return;
    }

    const auto lobbyState = network::parseLobbyState(packet, mode);
    if (!lobbyState.has_value()) {
        LOG_F(ERROR, "[Client] Etat du lobby invalide : %s", packet.c_str());
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
    LOG_F(INFO, "[Client] Lobby recu. Nombre de joueurs: %d. Connexion terminee", nbJoueur);
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
