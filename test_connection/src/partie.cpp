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
        std::cout << "nb de joueur :"<< nbJoueur;
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

void Partie::getEvent() { //par convention le joueur qui joue est joueur[0], pourrait etre rendu dynamique avec joueur[joueurcourant] mais blc c'est les event, plutot adapter joueurcourant en numjoueur
    if (!window) return;

    joueur[0].mousePos = sf::Mouse::getPosition(*window); //recupération de la position de la souris
    joueur[0].worldMousePos = window->mapPixelToCoords(joueur[0].mousePos);
    

    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }

    // Réinitialiser les entrées clavier
    joueur[0].Zpressed = joueur[0].Spressed = joueur[0].Qpressed = joueur[0].Dpressed = false;

    if (window->hasFocus()) {                   // récupération de touche pressée
        joueur[0].Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        joueur[0].Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        joueur[0].Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        joueur[0].Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    }
}

void Partie::update() {

    tank& mon_tank = joueur[joueur_courant].Tank; 

    sf::Vector2f dir = joueur[joueur_courant].worldMousePos - mon_tank.getTourelleSprite().getPosition();  
    
    float speed = mon_tank.get_vit();
    float rotation = mon_tank.get_ori();
    float vit_canon = mon_tank.get_vit_canon();


    if (joueur[0].Zpressed) {
        deplacement_verticale(mon_tank, mon_tank.get_ori(), speed);
        deplacement_verticale(mon_tank, rotation, speed);
    }

    if (joueur[0].Spressed) {
        deplacement_verticale(mon_tank, rotation, -speed);
        deplacement_verticale(mon_tank, rotation, -speed);
    }

    if (joueur[0].Qpressed)
        deplacement_rotation(mon_tank, &rotation, 0.2);
    
    if (joueur[0].Dpressed)
        deplacement_rotation(mon_tank, &rotation, -0.2);
    
    // Mise à jour de l'angle de la tourelle
    float angle_actu = mon_tank.getTourelleSprite().getRotation();
    float angle_voulu = atan2(dir.y, dir.x) * 180 / M_PI - 90;
    float diff = angle_voulu - angle_actu;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    
    mon_tank.getTourelleSprite().setRotation(angle_actu + diff * vit_canon);

    if ( mon_tank.get_x()-mon_tank.getBaseSprite().getLocalBounds().width / 2 == windowSize.x){
        mon_tank.set_x(0);
    }
    std::cout<<"position : "<<mon_tank.get_x()<<" "<<mon_tank.get_y()<<" orientiation : "<<mon_tank.get_ori()<<" orientation tourelle : "<< mon_tank.getTourelleSprite().getRotation()<<std::endl;
}

void Partie::renderWindow() {
    tank& mon_tank = joueur[joueur_courant].Tank;
    cursorSprite.setPosition(static_cast<float>(joueur[0].mousePos.x), static_cast<float>(joueur[0].mousePos.y));
    window->clear();
    window->draw(mon_tank.getBaseSprite());
    window->draw(mon_tank.getTourelleSprite());
    window->draw(cursorSprite);
    window->display();
}

void Partie::sendData(){

    char buffer[100];  // Taille suffisante pour 5 floats sous forme de texte
    int test = 1;

    sprintf(buffer, "%d %d %d %d %d %d %d %d",0, joueur[0].Zpressed ? 1 : 0, joueur[0].Qpressed ? 1 : 0, joueur[0].Spressed ? 1 : 0, joueur[0].Dpressed ? 1 : 0, static_cast<int>(joueur[0].worldMousePos.x), static_cast<int>(joueur[0].worldMousePos.y), test);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        //perror("❌ Erreur lors de l'envoi des données");
        return;
    } else {
        std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::recieveData(){
    char buffer[1024];
    tank& tankjoueur0 = joueur[joueur_courant].Tank;  // Utilisation d'une référence

    socklen_t addr_len = sizeof(client.recieve_servaddr);
    ssize_t n = recvfrom(client.recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client.recieve_servaddr, &addr_len);
    float x,y,ori, oritourelle;
    sscanf(buffer, "%f %f %f %f", &x, &y, &ori, &oritourelle);
    tankjoueur0.set_x(x);
    tankjoueur0.set_y(y);
    tankjoueur0.set_ori(ori);
    tankjoueur0.getTourelleSprite().setRotation(oritourelle);
    if (n < 0) {
        perror("Erreur lors de la réception de la confirmation");
        close(client.recieve_sockfd);
        return;
    }
    // Affichage du buffer reçu
    printf("Buffer reçu : %s\n", buffer);
}


void Partie::updatefromUDP() {

    tank& mon_tank = joueur[joueur_courant].Tank; 
    cursorSprite.setPosition(static_cast<float>(joueur[0].mousePos.x), static_cast<float>(joueur[0].mousePos.y));  //le server n'y touche pas
    mon_tank.set_ori(client.new_ori);
    mon_tank.set_x(client.new_x);
    mon_tank.set_y(client.new_y);
    mon_tank.getTourelleSprite().setRotation(client.new_angle);
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

    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    // Boucle de jeu
    while (window->isOpen()) {
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

    if (window) {
        delete window;
    }
    
    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "MULTIJOUEUR");
    windowSize = window->getSize();
    window->setMouseCursorVisible(false);
    
    joueur_courant = 0;

    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    client.num_port = 3000;
    client.createSocket();
    client.num_port = 3001;
    client.createBindedSocket();
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
