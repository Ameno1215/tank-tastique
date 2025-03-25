Tanktastique - Règles du jeu et Notice


PRESENTATION

Tanktastique est un jeu de combat de tanks en 2D multijoueur. C'est un battle royale où de 2 à 6 joueurs s'affrontent sur une carte, et le dernier tank ou la dernière équipe en vie remporte la partie.

CONDITION

Le jeu tourne uniquement sous LINUX UBUNTU et avec une version SMFL 2.6.1 ou plus


LANCEMENT DU JEU

	1. Compilation du jeu : place-toi dans le dossier tank-tastique dans un terminal, réalise la commande make all.

	2. Lancement du jeu : Chaque joueur lance le jeu avec la commande ./zbin/client.

	3. Création de la partie : Un et un seul joueur clique sur le bouton "Créer partie", sélectionne le nombre de joueurs et choisit soit "mêlée générale" ou "équipe" puis "valider".

	4. Récupération de l'adresse IP : Le joueur qui a créé la partie doit récupérer son adresse IP avec la commande: hostname -I | awk '{print $1}'

	5. Connexion des joueurs : une fois la partie créée par un joueur, les autres joueurs cliquent sur "rejoindre partie".

	6. Connexion au serveur : tous les joueurs doivent renseigner leur pseudo "valider" puis renseigner l'adresse IP issue de létape 4 et enfin "valider".


Il existe aussi un mode de test permettant de jouer en localhost sans demander les pseudos pour gagner du temps. Pour activer ce mode, il faut modifier la constante TEST à la ligne 32.

	- Si TEST est à 1, le mode est activé.

	- Si TEST est à 0, le jeu fonctionne en mode réseau classique.

Après avoir modifié cette constante, il est nécessaire d'exécuter la commande : make clean avant de recommencer la compilation du jeu (voir étape 1 du lancement du jeu).


DEROULEMENT DU JEU

	1. Choix du tank : Chaque joueur choisit son tank parmi 6 types ayant chacun des caractéristiques uniques et un ultime (pouvoir) propre à chaque type. Une fois l'ultime actif, il dure 3 secondes.

	2. Combat : Les joueurs s'affrontent en utilisant les tirs et les ultimes de leur tank pour éliminer leurs adversaires.

	3. Victoire : Le dernier joueur encore en vie gagne la partie.




Les Tanks et leurs caractéristiques

+-----------+---------+-------------+--------+--------+--------------+-----+----------------------------------+
| Tank      | Vitesse | Cad. de tir | Dégâts | Portée | Vitesse obus | Vie | Ultime (3s)                      |
+-----------+---------+-------------+--------+--------+--------------+-----+----------------------------------+
| Classique |    3    |      3      |    2   |    4   |      3       |  3  | Bouclier (bloque 1 obus)         |
| Rapide    |    5    |      5      |    1   |    2   |      2       |  1  | Dégâts x2                        |
| Healer    |    3    |      3      |    2   |    3   |      3       |  2  | +2 cœurs                         |
| Mortier   |    2    |      3      |    2   |    3   |      3       |  3  | Tire au-dessus des obstacles     |
| Solide    |    1    |      1      |    4   |    2   |      2       |  4  | One-shot un ennemi               |
| Sniper    |    2    |      1      |    3   |    5   |      4       |  3  | Augmente la cadence de tir       |
+-----------+---------+-------------+--------+--------+--------------+-----+----------------------------------+

Chaque tank a un style de jeu unique, donc choisis bien en fonction de ta stratégie !


COMMANDES

	- Déplacement : Z, Q, S, D

	- Tirer : clic gauche

	- Activer l'ultime : X
