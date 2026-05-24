#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>
#include <thread> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <optional>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "logguru.hpp"

#define SERVER_IP "192.168.1.48"
#define SERVER_PORT_CONNEXION 3000

#include "bouton.hpp" 
#include "joueur.hpp"
#include "networkProtocol.hpp"

class Client{

    public :
        Joueur joueur;
        int num_port;
        int sockfd;
        struct sockaddr_in servaddr, recieve_servaddr;
        int nbJoueur;
        int mode;
        Client();
        ~Client();

        void sendMessageToServer(const std::string& message);
        void initconnexion();
        void createSocket();
        void closeSockets();
        bool sendInput(const Joueur& joueur);
        bool sendTankChoice(int playerId, int tankType);
        bool receivePacket(std::string& packet);
        bool receiveRawPacket(char* buffer, std::size_t bufferSize, ssize_t& receivedBytes);
        int get_etatConnexion();
        std::string getLocalIPAddress();
        int getReceivePort() const;
        const std::array<std::string, network::kMaxPlayers>& getPseudos() const;
        const std::array<int, network::kMaxPlayers>& getEquipes() const;
        void configureConnection(const std::string& ip, const std::string& pseudo);
        std::atomic<bool> ipValide {false};

        std::string server_ip;
        std::array<std::string, network::kMaxPlayers> pseudos{};
        std::array<int, network::kMaxPlayers> equipe{};

        int test;

    private :
        std::atomic<int> etatConnexion {-1};
        int receive_port = 0;
        std::mutex connectionMutex;
        std::condition_variable connectionCv;
        bool connectionConfigured = false;

};
#endif
