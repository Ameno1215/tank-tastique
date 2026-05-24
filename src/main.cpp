#include <signal.h>
#include <fstream>
#include "lobby.hpp"
#include "partie.hpp"


// Récupérer le PID du serveur
int getServerPID() {
    std::ifstream pidFile("server.pid");
    if (!pidFile) {
        LOG_F(WARNING, "Impossible de lire le fichier PID");
        return -1;
    }
    
    int pid;
    pidFile >> pid;
    return pid;
}

// Fonction pour arrêter le serveur proprement
void stopServer() {
    int pid = getServerPID();
    if (pid > 0) {
        LOG_F(INFO, "Arret du serveur");
        kill(pid, SIGTERM);  // Envoie SIGTERM au serveur
    }
}

// Gestionnaire de signal pour Ctrl+C
void signalHandler(int signum) {
    if (signum == SIGINT) {
        LOG_F(INFO, "Ctrl+C detecte, fermeture propre");
        stopServer();
        exit(0);  // Quitte le programme proprement
    }
}

int main(){
    int argc = 0;
    char** argv = nullptr;
    logguru::init(argc, argv);

    int choix = 0;
    int retourLobby = 1;

    while(retourLobby){

        Partie partie;
        sleep(0.2);
        Lobby lobby;
        choix =  lobby.choix();
        
        //créer partie
        if(choix == 1){
            signal(SIGINT, signalHandler);
            std::string startServer = "./zbin/server " + std::to_string(lobby.nbJoueurChoisi) + " " + std::to_string(lobby.mode) + "&";
            std::system(startServer.c_str());
            sleep(1);

            retourLobby = partie.multiJoueur(true); // boucle principale infinie*
            stopServer();
            retourLobby = 0;
        }
        
        // rejoindre partie
        if(choix == 2){
            LOG_F(INFO, "Ouverture d'une partie multijoueur");
            retourLobby = partie.multiJoueur(false);
        }

    }
    return 0;

}
