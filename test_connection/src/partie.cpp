#include "partie.hpp"
#include "client.hpp"
#include "deplacement.hpp"

Partie::Partie() {
    nbJoueur = 0;
    port_actuel = 0;
    joueur_courant = 0;
}

// Destructeur pour éviter les fuites mémoire
Partie::~Partie() {
    if (window) {
        delete window;
        window = nullptr;
    }
}

// Fonction pour ajouter un joueur jusqu'à un maximum de 6
bool Partie::ajouteJoueur() {
    if (nbJoueur < 6) {
        port_actuel = 3001 + nbJoueur;
        joueur[nbJoueur].port = port_actuel;
        joueur[nbJoueur].id = nbJoueur; 
        nbJoueur++;
        std::cout << "joueur ajouté nb de joueur :"<< nbJoueur;
        return true;
    } else {
        std::cout << "Nombre maximal de joueurs atteint !" << std::endl;
        return false;
    }
}

int Partie::get_portactuel(){
    return port_actuel;
}

int Partie::get_nbJoueur(){
    return nbJoueur;
}

void Partie::getEvent() { 

    if (!window) return;

    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }

    // Réinitialiser les entrées clavier
    joueur[joueur_courant].Zpressed = joueur[joueur_courant].Spressed = joueur[joueur_courant].Qpressed = joueur[joueur_courant].Dpressed = false;

    if (window->hasFocus()) { //uniquement si on est sur la fenetre 
        joueur[joueur_courant].mousePos = sf::Mouse::getPosition(*window);                                  //recupération de la position de la souris
        joueur[joueur_courant].worldMousePos = window->mapPixelToCoords(joueur[joueur_courant].mousePos);   //la mettre dans le repère de la fenetre (je crois)
        joueur[joueur_courant].Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);                      //recuperation des touches pressées
        joueur[joueur_courant].Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        joueur[joueur_courant].Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        joueur[joueur_courant].Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    }
}

void Partie::update() {

    tank& mon_tank = joueur[joueur_courant].Tank; //pour pas que ca rallonge le code

    sf::Vector2f dir = joueur[joueur_courant].worldMousePos - mon_tank.getTourelleSprite().getPosition();  
    
    float speed = mon_tank.get_vit();
    float rotation = mon_tank.get_ori();
    float vit_canon = mon_tank.get_vit_canon();

    if (joueur[joueur_courant].Zpressed) {
        deplacement_verticale(mon_tank, rotation, speed);
        deplacement_verticale(mon_tank, rotation, speed);
    }

    if (joueur[joueur_courant].Spressed) {
        deplacement_verticale(mon_tank, rotation, -speed);
        deplacement_verticale(mon_tank, rotation, -speed);
    }

    if (joueur[joueur_courant].Qpressed)
        deplacement_rotation(mon_tank, &rotation, 1.2);
    
    if (joueur[joueur_courant].Dpressed)
        deplacement_rotation(mon_tank, &rotation, -1.2);
    
    // Mise à jour de l'angle de la tourelle truc à Joshua
    float angle_actu = mon_tank.getTourelleSprite().getRotation();
    float angle_voulu = atan2(dir.y, dir.x) * 180 / M_PI - 90;
    float diff = angle_voulu - angle_actu;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    
    mon_tank.getTourelleSprite().setRotation(angle_actu + diff * vit_canon); //rotation tourelle

    //message de debugage
    //std::cout<<"Joueur "<<joueur_courant<<"position : "<<mon_tank.get_x()<<" "<<mon_tank.get_y()<<" orientiation : "<<mon_tank.get_ori()<<" orientation tourelle : "<< mon_tank.getTourelleSprite().getRotation()<<std::endl;
}

void Partie::renderWindow() {

    window->clear();
    //affichage souris
    cursorSprite.setPosition(static_cast<float>(joueur[joueur_courant].mousePos.x), static_cast<float>(joueur[joueur_courant].mousePos.y));
    window->draw(cursorSprite);

    //affichage de chaque tank
    for(int i = 0; i<nbJoueur; i++){
        tank& mon_tank = joueur[i].Tank;
        std::cout<<"Draw tank :"<<i<<std::endl;
        window->draw(mon_tank.getBaseSprite());
        window->draw(mon_tank.getTourelleSprite());
    }

    //affchage des missiles 
    //........
    
    window->display();
}

void Partie::sendData(){

    char buffer[100];  // Taille suffisante pour 5 floats sous forme de texte
    int test = 1;      // valeur à mettre par précaution à la fin du buffer

    sprintf(buffer, "%d %d %d %d %d %d %d %d", joueur_courant, joueur[joueur_courant].Zpressed ? 1 : 0, joueur[joueur_courant].Qpressed ? 1 : 0, joueur[joueur_courant].Spressed ? 1 : 0, joueur[joueur_courant].Dpressed ? 1 : 0, static_cast<int>(joueur[joueur_courant].worldMousePos.x), static_cast<int>(joueur[joueur_courant].worldMousePos.y), test);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        perror("❌ Erreur lors de l'envoi des données");
        return;
    } else {
        std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::recieveData(){

    char buffer[1024];
    socklen_t addr_len = sizeof(client.recieve_servaddr);

    ssize_t n = recvfrom(client.recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client.recieve_servaddr, &addr_len);

    int indice_joueur;
    float x,y,ori, oritourelle;
    sscanf(buffer, "%d %f %f %f %f",&indice_joueur, &x, &y, &ori, &oritourelle);

    tank& tankjoueur = joueur[indice_joueur].Tank;  // Utilisation d'une référence
    tankjoueur.set_x(x);
    tankjoueur.set_y(y);
    tankjoueur.set_ori(ori);
    tankjoueur.getTourelleSprite().setRotation(oritourelle);

    if (n < 0) {
        perror("Erreur lors de la réception de la confirmation");
        close(client.recieve_sockfd);
        return;
    }
    // Affichage du buffer reçu
    //printf("Buffer reçu : %s du port %d\n", buffer, client.num_port);
}

int Partie::Solo() {
    // Libérer la mémoire si une fenêtre existait déjà
    if (window) {
        delete window;
    }
    
    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "SOLO");
    windowSize = window->getSize();
    window->setMouseCursorVisible(false);
    
    joueur_courant = 0;
    nbJoueur =  1;

    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    // Boucle de jeu
    while (window->isOpen()) {
        std::cout<<"joueur courant : "<<joueur_courant<<std::endl;
        getEvent();
        update();
        renderWindow();
    }

    return 0;
}

int Partie::multiJoueur() {
    joueur_courant = 0;
    
    std::thread connexionThread(&Client::initconnexion, &this->client);

    affichageConnexion();

    // Arrêter le thread proprement
    if (connexionThread.joinable()) {
        connexionThread.join();
    }

    nbJoueur = client.nbJoueur;
    joueur_courant = client.joueur.id;

    if (window) {
        delete window;
    }
    
    char title[50];
    sprintf(title, "MULTIJOUEUR JOUEUR %d", joueur_courant);

    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), title);
    windowSize = window->getSize();
    window->setMouseCursorVisible(false);
    
    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    
    int numport = client.num_port;
    client.num_port = 3000;         //creation du port d'envoie sur le port 3000
    client.createSocket();
    client.num_port = numport;
    client.createBindedSocket();    //creation du port d'écoute sur les ports de chaque client
    
    // Boucle de jeu en multi
    while (window->isOpen()) {
        getEvent();
        sendData();
        recieveData();
        renderWindow();
    }

    return 0;
}

void Partie::affichageConnexion(){
    if (window) {  // Pour libérer la mémoire si une fenêtre existait déjà
        delete window;
    }
    
    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "MULTI");
    windowSize = window->getSize();

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Image/fond.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(window->getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window->getSize().y) / backgroundSprite.getGlobalBounds().height
    );

    // Chargement de la police
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }

    // Création du texte d'affichage
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(50);
    statusText.setFillColor(sf::Color::White);
    statusText.setStyle(sf::Text::Bold);

    // Afficher "Connexion avec le serveur..." pendant 1 seconde avant la boucle principale
    statusText.setString("Connexion avec le serveur...");

    // Centrer le texte au milieu de la fenêtre
    sf::FloatRect textBounds = statusText.getLocalBounds();
    statusText.setPosition(
        (windowSize.x - textBounds.width) / 2.0f,
        (windowSize.y - textBounds.height) / 2.0f
    );

    // Affichage initial
    window->clear();
    window->draw(backgroundSprite);
    window->draw(statusText);
    window->display();

    // Pause de 1 seconde
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }

        // Mettre à jour le texte en fonction de l'état de connexion
        if (client.get_etatConnexion() == -1) {
            statusText.setString("Connexion avec le serveur...");
        } else if (client.get_etatConnexion() == 0) {
            statusText.setString("En attente des joueurs...");
        } else if (client.get_etatConnexion() == 1) {
            window->close();
        }

        // Centrer le texte au milieu de la fenêtre
        sf::FloatRect textBounds = statusText.getLocalBounds();
        statusText.setPosition(
            (windowSize.x - textBounds.width) / 2.0f,
            (windowSize.y - textBounds.height) / 2.0f
        );

        window->clear();
        window->draw(backgroundSprite);
        window->draw(statusText);
        window->display();
    }
}
