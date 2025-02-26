#include "partie.hpp"
#include "client.hpp"
#include "deplacement.hpp"
#include "tir.cpp"

Partie::Partie() {
    nbJoueur = 0;
    port_actuel = 0;
    joueur_courant = 0;
    if (!pvTexture.loadFromFile("Image/obus.png")) {
        std::cerr << "Erreur lors du chargement de la texture des PV !\n";
    }

    pvSprite.setTexture(pvTexture);
    pvSprite.setScale(0.2f, 0.2f);

    for (int i = 0; i < 10; i++) {
        std::string filename = "Image/explosion/explosion_frame" + std::to_string(i + 1) + ".png";
        if (!explosionTextureFrames[i].loadFromFile(filename)) {
            std::cerr << "Erreur : Impossible de charger l'image d'explosion (" << filename << ").\n";
        }
    }

    explosionSprite.setScale(4.0f,4.0f);

}

// Destructeur pour éviter les fuites mémoire
Partie::~Partie() {
    if (window) {
        delete window;
        window = nullptr;
    }
    
}

sf::Sprite& Partie::getpvSprite() { return pvSprite; }

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

std::string& Partie::get_buffer_missile() {
    return buffer_missile;
}

int Partie::get_portactuel(){
    return port_actuel;
}

int Partie::get_nbJoueur(){
    return nbJoueur;
}

void Partie::setBufferMissile(const std::string& newBuffer) {
        buffer_missile = newBuffer;
}

void Partie::getEvent() { 

    if (!window) return;

    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }
    
    // Réinitialiser les entrées clavier
    joueur[joueur_courant].Zpressed = joueur[joueur_courant].Spressed = joueur[joueur_courant].Qpressed = joueur[joueur_courant].Dpressed = joueur[joueur_courant].Clicked = joueur[joueur_courant].Tabpressed = false;

    if (window->hasFocus()) { //uniquement si on est sur la fenetre 
        joueur[joueur_courant].mousePos = sf::Mouse::getPosition(*window);                                  //recupération de la position de la souris
        joueur[joueur_courant].worldMousePos = window->mapPixelToCoords(joueur[joueur_courant].mousePos);   //la mettre dans le repère de la fenetre (je crois)
        joueur[joueur_courant].Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);                      //recuperation des touches pressées
        joueur[joueur_courant].Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        joueur[joueur_courant].Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        joueur[joueur_courant].Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        joueur[joueur_courant].Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        joueur[joueur_courant].Tabpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);
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
        //joueur[0].pV--; //test pour le PV 
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



    // CALCUL DEPLACEMENT OBUS
    Noeud* courant = mon_tank.getListeObus().get_tete();
    while (courant) {
        if (courant->obus.get_status()) {
            // Calcul du déplacement
            sf::Vector2f deltaMove;
            deltaMove.x = mon_tank.get_vitesse_obus() * sin(courant->obus.get_Sprite().getRotation() * M_PI / 180);
            deltaMove.y = -mon_tank.get_vitesse_obus() * cos(courant->obus.get_Sprite().getRotation() * M_PI / 180);
            
            // position = position + déplacement
            courant->obus.get_Sprite().setPosition(courant->obus.get_Sprite().getPosition().x + deltaMove.x, courant->obus.get_Sprite().getPosition().y + deltaMove.y);

            // test si depassement de la porté du tir
            int longueur = sqrt(pow(courant->obus.get_Sprite().getPosition().x - courant->obus.get_position_tir().x, 2) + pow(courant->obus.get_Sprite().getPosition().y - courant->obus.get_position_tir().y, 2));
            if (longueur > courant->obus.get_porte()) {
                courant->obus.set_status(0); // destruction obus
                Noeud * temp = courant->suivant;
                mon_tank.getListeObus().supprimer(courant->index);
                courant = temp;
            }
            else courant = courant->suivant;
        }
    }

    // TIR NOUVEL OBUS
    if (joueur[joueur_courant].Clicked) {
        if ((get_time_seconds() - mon_tank.getListeObus().get_time_dernier_tir()) > mon_tank.get_cadence_tir()) {
            // ajout création du nouvel obus et l'ajouter à la liste d'obus du tank
            int index = mon_tank.getListeObus().ajouterFin(mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getRotation(), 0.2, 500, "Image/obus.png");
            mon_tank.getListeObus().set_time_dernier_tir(get_time_seconds());
            mon_tank.getListeObus().trouverNoeud(index)->obus.initTir(mon_tank.getTourelleSprite().getRotation(), mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y);
            // window.draw(mon_tank.getListeObus().trouverNoeud(index)->obus.get_Sprite());

            // printf("%d\n", nb_obus());
            //mon_tank.getListeObus().afficher();
        }
    }

    //message de debugage
    //std::cout<<"Joueur "<<joueur_courant<<"position : "<<mon_tank.get_x()<<" "<<mon_tank.get_y()<<" orientiation : "<<mon_tank.get_ori()<<" orientation tourelle : "<< mon_tank.getTourelleSprite().getRotation()<<std::endl;
}

void Partie::renderWindow() {

    window->clear();
    //affichage souris
    cursorSprite.setPosition(
        static_cast<float>(joueur[joueur_courant].mousePos.x) - cursorSprite.getGlobalBounds().width / 2,
        static_cast<float>(joueur[joueur_courant].mousePos.y) - cursorSprite.getGlobalBounds().height / 2
    );
    window->draw(cursorSprite);

    //affichage de chaque tank
    for(int i = 0; i<nbJoueur; i++){
        tank& mon_tank = joueur[i].Tank;

        window->draw(mon_tank.getBaseSprite());
        window->draw(mon_tank.getTourelleSprite());


        // OBUS
        std::istringstream stream(get_buffer_missile());
        char type;
        stream >> type;
        // vider la liste 
        for (int i = 0; i < nbJoueur; i++) {
            // printf("Avant vidage\n");
            // joueur[i].Tank.getListeObus().afficher();   
            joueur[i].Tank.getListeObus().vider();
            // printf("Apres vidage\n");
            // joueur[i].Tank.getListeObus().afficher();   
        }
    
        int nb_obus;
        stream >> nb_obus;
        // std::cout << "Nombre d'obus : " << nb_obus << std::endl;

        while (!stream.eof()) {
            int joueur_id;
            float x, y, rotation;

            stream >> joueur_id >> x >> y >> rotation;
            // std::cout << "Obus du joueur " << joueur_id << " -> Pos(" << x << ", " << y << ") | Rotation: " << rotation << std::endl;
            
            joueur[joueur_id].Tank.getListeObus().ajouterFin(static_cast<int>(x), static_cast<int>(y), rotation, 10000, 500, "Image/obus.png");
            // joueur[joueur_id].Tank.getListeObus().afficher();
        }

        // Affichage Obus
        Noeud* courant = mon_tank.getListeObus().get_tete();
        while (courant) {
            if (courant->obus.get_status()) {
                window->draw(courant->obus.get_Sprite());
            }
            courant = courant->suivant;
        }
    }

    //affchage des pV
    for (int i = 0; i < joueur[joueur_courant].pV; i++) {
        getpvSprite().setPosition(20 + i * 40, window->getSize().y - 50); // Alignement en bas à gauche
        window->draw(getpvSprite());
    }

    //toute les choses relatives uniquement joueur, c'est à dire aucun lien avec le server
    if(joueur[joueur_courant].Tabpressed){
        afficheTableauScore();
        std::cout<<"Tab pressed \n"<<std::endl;
    }

    window->display();
}

void Partie::sendData(){

    char buffer[100];  // Taille suffisante pour 5 floats sous forme de texte
    int test = 1;      // valeur à mettre par précaution à la fin du buffer

    sprintf(buffer, "%d %d %d %d %d %d %d %d %d", joueur_courant, joueur[joueur_courant].Zpressed ? 1 : 0, joueur[joueur_courant].Qpressed ? 1 : 0, joueur[joueur_courant].Spressed ? 1 : 0, joueur[joueur_courant].Dpressed ? 1 : 0, static_cast<int>(joueur[joueur_courant].worldMousePos.x), static_cast<int>(joueur[joueur_courant].worldMousePos.y), joueur[joueur_courant].Clicked ? 1 : 0, test);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        perror("❌ Erreur lors de l'envoi des données");
        return;
    } else {
        //std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::recieveData(){

    char buffer[1024];
    socklen_t addr_len = sizeof(client.recieve_servaddr);

    ssize_t n = recvfrom(client.recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client.recieve_servaddr, &addr_len);
    buffer[n] = '\0';

    if (buffer[0] == 'T') {
        int indice_joueur;
        float x,y,ori, oritourelle;
        sscanf(buffer, "T %d %f %f %f %f",&indice_joueur, &x, &y, &ori, &oritourelle);

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
    }

    // RECEPTION DE LA LSITE D'OBUS
    std::string message(buffer); 
    if (buffer[0] == 'O') { 
        setBufferMissile(std::string(buffer));
    }
    
    if(buffer[0] == 'V'){
        int pV[6];
        sscanf(buffer, "V %d %d %d %d %d %d", &pV[0], &pV[1], &pV[2], &pV[3], &pV[4], &pV[5]);

        for(int i = 0; i< NB_JOUEUR; i++){
            Joueur& joueuri = joueur[i];  
            joueuri.pV = pV[i];
        }
        
        if (n < 0) {
            perror("Erreur lors de la réception de la confirmation");
            close(client.recieve_sockfd);
            return;
        }
        // Affichage du buffer reçu
        //printf("Buffer pV reçu : %s tesssstttt \n", buffer);
    }
    
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
    joueur[joueur_courant].Tank.set_vit(0.2f);

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
    client.createBindedSocket();    //creation du port d'écoute sur le port dédié au client
    
    std::thread recievethread([this]() { //thread qui recoit les données processed par le server
        while (window->isOpen()) {
            recieveData();
        }
    });

    // Boucle de jeu en multi
    while (window->isOpen()) {
        getEvent();
        sendData();
        renderWindow();
        std::this_thread::sleep_for(std::chrono::milliseconds(8));  // Ajout d'un délai pour éviter une boucle trop rapide
    }

    if (recievethread.joinable()) {     // Fermeture du thread
        recievethread.join();
    }

    return 0;
}

void Partie::affichageConnexion() {
    if (window) {  
        delete window;
    }

    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "MULTI");
    windowSize = window->getSize();

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Image/imagechargement.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(window->getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window->getSize().y) / backgroundSprite.getGlobalBounds().height
    );

    sf::Texture tankChargementTexture;
    if (!tankChargementTexture.loadFromFile("Image/tank_chargement.png")) {
        std::cerr << "Erreur : Impossible de charger l'image du tank de chargement.\n";
    }
    sf::Sprite tankChargementSprite(tankChargementTexture);
    tankChargementSprite.setPosition(100, windowSize.y - tankChargementSprite.getGlobalBounds().height - 230);

    sf::Texture obusTexture;
    if (!obusTexture.loadFromFile("Image/obus.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de l'obus.\n";
    }
    sf::Sprite obusSprite(obusTexture);
    obusSprite.setScale(0.5f, 0.5f);
    float obusStartX = tankChargementSprite.getPosition().x + (tankChargementSprite.getGlobalBounds().width) * 1.12;
    float obusStartY = tankChargementSprite.getPosition().y * 1.6;
    obusSprite.setPosition(obusStartX, obusStartY);
    obusSprite.setRotation(90);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Impossible de charger la police.\n";
    }

    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(50);
    statusText.setFillColor(sf::Color::White);
    statusText.setStyle(sf::Text::Bold);
    statusText.setString("Connexion avec le serveur...");
    sf::FloatRect textBounds = statusText.getLocalBounds();
    statusText.setPosition((windowSize.x - textBounds.width) / 2.0f, 200);

    window->clear();
    window->draw(backgroundSprite);
    window->draw(statusText);
    window->display();

    float obusSpeed = 5.0f; 
    sf::Clock clock;
    sf::Clock timerClock;
    bool oneSecondPassed = false;
    int xexplosion, yexplosion;
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }

        if (!oneSecondPassed && timerClock.getElapsedTime().asSeconds() >= 1.0f) {
            oneSecondPassed = true;
        }

        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        if (client.get_etatConnexion() == -1 && oneSecondPassed) {
            statusText.setString("Connexion avec le serveur...");
        } else if (client.get_etatConnexion() == 0 && oneSecondPassed) {
            statusText.setString("En attente des joueurs...");
        } else if (client.get_etatConnexion() == 1 && oneSecondPassed) {
            window->close();
        }

        textBounds = statusText.getLocalBounds();
        statusText.setPosition((windowSize.x - textBounds.width) / 2.0f, 200);

        obusSprite.move(obusSpeed * deltaTime * 100, 0);

        // Si l'obus dépasse la limite droite de l'écran, on lance l'animation de l'explosion
        if (obusSprite.getPosition().x > windowSize.x - 200) {
            if (!explosionActive) {
                // Si l'explosion n'est pas déjà active, on l'active
                explosionActive = true;
                explosionClock.restart();  // Redémarre le chrono de l'explosion
                xexplosion = obusSprite.getPosition().x + 50;
                yexplosion = obusSprite.getPosition().y + 80;
            }
            // Réinitialiser l'obus à sa position de départ
            obusSprite.setPosition(obusStartX, obusStartY);
        }

        window->clear();
        window->draw(backgroundSprite);
        window->draw(statusText);
        window->draw(tankChargementSprite);
        window->draw(obusSprite);

        // Si l'explosion est active, on la dessine et on met à jour son animation
        if (explosionActive) {
            renderExplosion(xexplosion , yexplosion);
            window->draw(explosionSprite);
        }


        window->display();
    }
}


int Partie::nb_obus() {
    int compteur_obus = 0;

    for (int i = 0; i < nbJoueur; i++) {
        tank& mon_tank = joueur[i].Tank;
    
        Noeud* courant = mon_tank.getListeObus().get_tete();

        while (courant) {
            compteur_obus++;
            courant = courant->suivant;
        }
    }
    
    return compteur_obus;
}


void Partie::string_obus(std::string& chaine) {
    chaine = "O";

    chaine += std::to_string(nb_obus()) + " \n";

    for (int i = 0; i < nbJoueur; i++) {
        tank& mon_tank = joueur[i].Tank;
        Noeud* courant = mon_tank.getListeObus().get_tete();

        while (courant) {

            chaine += std::to_string(i);
            chaine += " " + std::to_string(courant->obus.get_Sprite().getPosition().x);
            chaine += " " + std::to_string(courant->obus.get_Sprite().getPosition().y);
            chaine += " " + std::to_string(courant->obus.get_Sprite().getRotation());
            chaine += "\n";
            
            courant = courant->suivant;
        }
    }
    
    chaine += "1";
}

void Partie::renderExplosion(int x, int y) {

    // Gérer le temps écoulé pour passer à la frame suivante de l'explosion
    if (explosionClock.getElapsedTime().asSeconds() > 0.05f) {  // 50ms entre chaque frame
        explosionClock.restart();  // Réinitialiser le chrono pour la prochaine frame

        // Passer à la frame suivante
        currentFrameExplo++;
        if (currentFrameExplo >= 10) {
            currentFrameExplo = 0;  // Revenir à la première frame après la dernière
            explosionActive = false;  // Terminer l'animation après avoir fait le cycle complet
        }

        // Mettre à jour la texture de l'explosion avec la frame actuelle
        explosionSprite.setTexture(explosionTextureFrames[currentFrameExplo]);
    }

    // Centrer l'explosion sur les coordonnées données
    explosionSprite.setPosition(x - explosionSprite.getGlobalBounds().width / 2,
                                y - explosionSprite.getGlobalBounds().height / 2);
}


void Partie::afficheTableauScore() {
    if (!window) return;

    // Définir la taille et la position de la boîte centrale
    sf::RectangleShape scoreBox(sf::Vector2f(400, 300)); // Largeur x Hauteur
    scoreBox.setFillColor(sf::Color(0, 0, 0, 150)); // Fond noir semi-transparent
    scoreBox.setOutlineThickness(3);
    scoreBox.setOutlineColor(sf::Color::White);
    scoreBox.setPosition((window->getSize().x - scoreBox.getSize().x) / 2,
                         (window->getSize().y - scoreBox.getSize().y) / 2);

    window->draw(scoreBox); // Affichage de la boîte

    // Charger une police
    static sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) { // Assure-toi d'avoir une police dans ton projet
        std::cerr << "Erreur: Impossible de charger la police!" << std::endl;
        return;
    }

    // Afficher le titre
    sf::Text title("Tableau des Scores", font, 24);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(scoreBox.getPosition().x + 100, scoreBox.getPosition().y + 10);
    window->draw(title);

    // Afficher les joueurs et leurs stats
    for (int i = 0; i < nbJoueur; i++) {
        sf::Text playerText;
        playerText.setFont(font);
        playerText.setCharacterSize(20);
        playerText.setFillColor(sf::Color::White);

        // Construire la ligne du joueur (Nom - Pv - Score)
        std::string playerInfo = joueur[i].pseudo + " - Pv: " + std::to_string(joueur[i].pV) + " - Score: " + std::to_string(joueur[i].pts);
        playerText.setString(playerInfo);

        // Positionner le texte dans la boîte
        playerText.setPosition(scoreBox.getPosition().x + 20, scoreBox.getPosition().y + 50 + i * 30);

        window->draw(playerText);
    }
}