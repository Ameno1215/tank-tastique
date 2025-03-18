#include <signal.h>
#include <fstream>
#include "lobby.hpp"
#include "partie.hpp"


// Récupérer le PID du serveur
int getServerPID() {
    std::ifstream pidFile("server.pid");
    if (!pidFile) {
        std::cerr << "Impossible de lire le fichier PID\n";
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
        std::cout << "\nArrêt du serveur...\n";
        kill(pid, SIGTERM);  // Envoie SIGTERM au serveur
    }
}

// Gestionnaire de signal pour Ctrl+C
void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << "\nCtrl+C détecté ! Fermeture propre...\n";
        stopServer();
        exit(0);  // Quitte le programme proprement
    }
}

int main(){

    Lobby lobby;
    Partie partie;
    int choix = lobby.choix();


    if(choix == 1){
        signal(SIGINT, signalHandler);
        std::cout << "On teste le lancement du server" << std::endl;
        std::string startServer = "./zbin/server " + std::to_string(lobby.nbJoueurChoisi) + "&";
        std::system(startServer.c_str());
        sleep(2);
        //voir si on peut greffer le code de Joshua
        partie.multiJoueur(); // boucle principale infinie
    }

    if(choix == 2){
        std::cout << "Partie Multijoueur" << std::endl;
        return partie.multiJoueur();
        // donc ici ce qu'il faut faire : créer un objet client, joueur et faire tourné tout ca. merci au revoir.
    }

    return 0;

}