#include "lobby.hpp"
#include "partie.hpp"

int main(){

    Lobby lobby;
    Partie partie;
    int choix = lobby.choix();

    if(choix == 1){
        std::cout << "Partie Solo" << std::endl;
        return partie.Solo();
        //voir si on peut greffer le code de Joshua
    }

    if(choix == 2){
        std::cout << "Partie Multijoueur" << std::endl;
        return partie.multiJoueur();
        // donc ici ce qu'il faut faire : créer un objet client, joueur et faire tourné tout ca. merci au revoir.
    }

    return 0;

}