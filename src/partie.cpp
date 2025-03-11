#include "partie.hpp"

Partie::Partie() {
    nbJoueur = 0;
    port_actuel = 0;
    joueur_courant = 0;
    if (!pvTexture.loadFromFile("Image/obus.png")) {
        std::cerr << "Erreur lors du chargement de la texture des PV !\n";
    }

    pvSprite.setTexture(pvTexture);
    pvSprite.setScale(0.2f, 0.2f);

    for (int i = 0; i < 11; i++) {
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

void Partie::set_go(int val)  {go = val; }

int Partie::get_go() { return go; }

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
        
        if(joueur[joueur_courant].pV > 0){
            joueur[joueur_courant].Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);                      //recuperation des touches pressées
            joueur[joueur_courant].Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            joueur[joueur_courant].Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
            joueur[joueur_courant].Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            joueur[joueur_courant].Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            joueur[joueur_courant].Tabpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);
        }
        else{
            boutonReplay.update(joueur[joueur_courant].worldMousePos);
            boutonScore.update(joueur[joueur_courant].worldMousePos);
            joueur[joueur_courant].Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);

            if(joueur[joueur_courant].Clicked){
                if(boutonReplay.isClicked(joueur[joueur_courant].worldMousePos)){
                    boutonScore.clicked = false;
                }
                if(boutonScore.isClicked(joueur[joueur_courant].worldMousePos)){
                    boutonReplay.clicked = false;
                }
            }
        }
    }
}

void Partie::update() {

    tank& mon_tank = *(joueur[joueur_courant].Tank); //pour pas que ca rallonge le code

    sf::Vector2f dir = joueur[joueur_courant].worldMousePos - mon_tank.getTourelleSprite().getPosition();  
    
    float speed = mon_tank.get_vit();
    float rotation = mon_tank.get_ori();
    float vit_canon = mon_tank.get_vit_canon();

    if (joueur[joueur_courant].Zpressed) {
        
        deplacement_verticale(mon_tank, rotation, speed); //on avance

        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, -2*speed);
        }
        //joueur[0].pV--; //test pour le PV 
    }

    if (joueur[joueur_courant].Spressed) {
        deplacement_verticale(mon_tank, rotation, -speed);

        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, 2*speed);
        }
    }

    if (joueur[joueur_courant].Qpressed){
        deplacement_rotation(mon_tank, &rotation, 1.2);
        
        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, -2.4);
        }
    }
    
    if (joueur[joueur_courant].Dpressed){
        deplacement_rotation(mon_tank, &rotation, -1.2);
        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, -2.4);
        }
    }
    
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
            else{

                bool obusDestructeur = false; //variable pour savoir si l'obus a touché quelque chose

                for(int i = 0; i<NB_JOUEUR; i++){
                    if(joueur[i].pV>0){
                        tank& testank = *(joueur[i].Tank);
                        testank.updateHitbox(); //il faut update car si le tank adverse bouge pas c'est pas bon 
                        testank.updateTouched(courant->obus.get_Sprite());
                        
                        if(testank.isTouched()){
                            listexplosion.ajouterFin(courant->obus.get_Sprite().getPosition().x, courant->obus.get_Sprite().getPosition().y, 0, false); //on rajoute une explosion à afficher
                            std::cout<<"x y ajouté en fin"<<std::endl;
                            if(joueur[i].pV > 0){
                                joueur[i].pV -= courant->obus.get_degat();
                            }
                            if(joueur[i].pV <= 0){
                                std::cout<<"joueur X a perdu";
                                joueur[i].pV = -1;
                                listexplosion.ajouterFin(joueur[i].Tank->getBaseSprite().getGlobalBounds().width/2 + joueur[i].Tank->getBaseSprite().getPosition().x, joueur[i].Tank->getBaseSprite().getGlobalBounds().height/2 + joueur[i].Tank->getBaseSprite().getPosition().y, 0, true);
                                joueur[i].vivant = false;
                            }
                            obusDestructeur = true;
                            courant->obus.set_status(0); // destruction obus
                            Noeud * temp = courant->suivant;
                            mon_tank.getListeObus().supprimer(courant->index);
                            courant = temp;
                            break;
                        }
                    }
                }
                if(!obusDestructeur){ //si l'obus n'a rien rencontré on détruit
                    courant = courant->suivant;
                }
            } 
        }
    }

    // TIR NOUVEL OBUS
    if (joueur[joueur_courant].Clicked) {
        if ((get_time_seconds() - mon_tank.getListeObus().get_time_dernier_tir()) > mon_tank.get_cadence_tir()) {
            // ajout création du nouvel obus et l'ajouter à la liste d'obus du tank
            int index = mon_tank.getListeObus().ajouterFin(mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getRotation(), mon_tank.get_vitesse_obus(), mon_tank.get_porte(), "Image/obus.png", mon_tank.get_degat());
            mon_tank.getListeObus().set_time_dernier_tir(get_time_seconds());
            mon_tank.getListeObus().trouverNoeud(index)->obus.initTir(mon_tank.getTourelleSprite().getRotation(), mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getTexture()->getSize().y * mon_tank.getTourelleSprite().getScale().y / 2);
            // window.draw(mon_tank.getListeObus().trouverNoeud(index)->obus.get_Sprite());

            // printf("%d\n", nb_obus());
            //mon_tank.getListeObus().afficher();
        }
    }

    testGagnant();

    //message de debugage
    //std::cout<<"Joueur "<<joueur_courant<<"position : "<<mon_tank.get_x()<<" "<<mon_tank.get_y()<<" orientiation : "<<mon_tank.get_ori()<<" orientation tourelle : "<< mon_tank.getTourelleSprite().getRotation()<<std::endl;
}

int Partie::testGagnant(){
    int compt = 0;
    int joueurVivant;

    for(int i = 0; i<NB_JOUEUR; i++){
        
        if(joueur[i].pV > 0){
            compt ++;
            joueurVivant = i;
        }
    }

    if(compt == 1){
        std::cout<<"Joueur "<<joueurVivant<<" a GAGNÉÉÉÉÉ"<<std::endl;
        partieFinie = true;
        return joueurVivant;
    }

    return -1;
}

void Partie::renderWindow(int multi) {

    window->clear();

    window->draw(fondSprite);
    int gagnant = testGagnant();
    if(gagnant < 0){
        if(joueur[joueur_courant].pV > 0 || visionnage){  //Pas game over
            //affichage de chaque tank
            for(int i = 0; i<nbJoueur; i++){
                if(joueur[i].pV > 0){  //vivant
                    tank& mon_tank = *(joueur[i].Tank);
    
                    window->draw(mon_tank.getBaseSprite());
                    window->draw(mon_tank.getTourelleSprite());
    
                    // OBUS
                    if (multi) {
                        std::istringstream stream(get_buffer_missile());
                        char type;
                        stream >> type;
                        // vider la liste 
                        for (int i = 0; i < nbJoueur; i++) {
                            // printf("Avant vidage\n");
                            // joueur[i].Tank.getListeObus().afficher();   
                            joueur[i].Tank->getListeObus().vider();
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
                            joueur[joueur_id].Tank->getListeObus().ajouterFin(static_cast<int>(x), static_cast<int>(y), rotation, joueur[joueur_id].Tank->get_vitesse_obus(), joueur[joueur_id].Tank->get_porte(), "Image/obus.png", joueur[joueur_id].Tank->get_degat());
                            // joueur[joueur_id].Tank->getListeObus().afficher();
                        }
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
            }
    
            //affichage des explosions
            NoeudExplosion* courant = listexplosion.get_tete();
            while (courant) {
                if (courant->frameActu < 20 && courant->frameActu > 0) {
                    courant->explosionSprite.setTexture(explosionTextureFrames[courant->frameActu]);
                    courant->explosionSprite.setPosition(courant->x - courant->explosionSprite.getGlobalBounds().width / 2, courant->y - courant->explosionSprite.getGlobalBounds().height / 2);
                    if(courant->big){
                        std::cout<<"affichage en Grand";
                        courant->explosionSprite.setScale(3.0f, 3.0f);
                    }
                    window->draw(courant->explosionSprite);
                }
                courant = courant->suivant;
                listexplosion.majEnSautantFrame();
            }
    
            //affchage des pV
            sf::Vector2f pvBasePosition(20, window->getSize().y*0.95); // Position en bas à gauche (20px du bas)
            sf::Vector2f worldPvPosition = window->mapPixelToCoords(sf::Vector2i(pvBasePosition)); // Conversion en coord monde

            for (int i = 0; i < joueur[joueur_courant].pV; i++) {
                getpvSprite().setPosition(worldPvPosition.x + i * 40, worldPvPosition.y);
                window->draw(getpvSprite());
            }
 
    
            //toute les choses relatives uniquement joueur, c'est à dire aucun lien avec le server
            if(joueur[joueur_courant].Tabpressed){
                afficheTableauScore();
            }
            
            window->draw(testSprite);
        }
    
        if(boutonScore.clicked){
            boutonReplay.draw(*window);
            afficheTableauScore();
            visionnage = false;
        }
        else if(boutonReplay.clicked){
            boutonScore.draw(*window);
            visionnage = true;
        }
        else if(joueur[joueur_courant].pV < 1 && !visionnage){
            window->draw(gameOverText);
            boutonScore.draw(*window);
            boutonReplay.draw(*window);
        }
    }
    else if(gagnant == joueur_courant){
        // Création du texte
        sf::Text victoryText;
        victoryText.setFont(font);
        victoryText.setString("VICTOIRE !!!");
        victoryText.setCharacterSize(100); // Taille du texte
        victoryText.setFillColor(sf::Color::Green); // Couleur rouge
        victoryText.setStyle(sf::Text::Bold); // Style en gras
        
        // Centrer le texte sur la fenêtre
        sf::FloatRect textRect = victoryText.getLocalBounds();
        victoryText.setOrigin(textRect.width / 2, textRect.height / 2);
        victoryText.setPosition(400, 300); // Centré sur la fenêtre (800x600)
        window->draw(victoryText);
    }
    else{
        if(boutonScore.clicked){
            boutonReplay.draw(*window);
            afficheTableauScore();
            visionnage = false;
        }
        else if(boutonReplay.clicked){
            boutonScore.draw(*window);
            visionnage = true;
        }
        else if(joueur[joueur_courant].pV < 1 && !visionnage){
            window->draw(gameOverText);
            boutonScore.draw(*window);
            boutonReplay.draw(*window);
        }
    }
    

    //affichage souris
    cursorSprite.setPosition(
        static_cast<float>(joueur[joueur_courant].worldMousePos.x) - cursorSprite.getGlobalBounds().width / 2,
        static_cast<float>(joueur[joueur_courant].worldMousePos.y) - cursorSprite.getGlobalBounds().height / 2
    );
    window->draw(cursorSprite);

    window->display();
}

void Partie::sendData(){

    char buffer[100];  // Taille suffisante pour 5 floats sous forme de texte
    int test = 1;      // valeur à mettre par précaution à la fin du buffer

    sprintf(buffer, "A %d %d %d %d %d %d %d %d %d", joueur_courant, joueur[joueur_courant].Zpressed ? 1 : 0, joueur[joueur_courant].Qpressed ? 1 : 0, joueur[joueur_courant].Spressed ? 1 : 0, joueur[joueur_courant].Dpressed ? 1 : 0, static_cast<int>(joueur[joueur_courant].worldMousePos.x), static_cast<int>(joueur[joueur_courant].worldMousePos.y), joueur[joueur_courant].Clicked ? 1 : 0, test);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        perror("❌ Erreur lors de l'envoi des données");
        return;
    } else {
        //std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::sendTank(int type) {

    char buffer[100]; 
    int test = 1;      // valeur à mettre par précaution à la fin du buffer

    sprintf(buffer, "K %d %d %d", joueur_courant, type, test);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        perror("❌ Erreur lors de l'envoi du type de tank");
        return;
    } else {
        //std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::sendReceptionTank() {

    char buffer[20]; 

    sprintf(buffer, "M %d 1", joueur_courant);
    int n = sendto(client.sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&client.servaddr, sizeof(client.servaddr));
    if (n < 0) {
        perror("❌ Erreur lors de l'envoi réception de tous les tanks");
        return;
    } else {
        //std::cout << "📨 Données envoyées : " << buffer << std::endl;
    }
}

void Partie::recieveData(){

    char buffer[1024];
    socklen_t addr_len = sizeof(client.recieve_servaddr);

    ssize_t n = recvfrom(client.recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client.recieve_servaddr, &addr_len);
    if (n < 0) {
        perror("Erreur lors de la réception");
        return;
    }
    buffer[n] = '\0';

    if (buffer[0] == 'T') {
        int indice_joueur;
        float x,y,ori, oritourelle;
        sscanf(buffer, "T %d %f %f %f %f",&indice_joueur, &x, &y, &ori, &oritourelle);

        tank& tankjoueur = *(joueur[indice_joueur].Tank);  // Utilisation d'une référence
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
    if (buffer[0] == 'O') { 
        setBufferMissile(std::string(buffer));
    }
        
    if (buffer[0] == 'E') {
        std::istringstream stream(std::string(buffer).substr(2));  // Supprime 'E ' et crée un flux
        int x, y, big;

        while (stream >> x >> y >> big) {  // Extrait les couples de coordonnées car il peut y avoir plusieurs explosions à la fois
            bool isBig = (big != 0);  // Convertit en booléen
            listexplosion.ajouterFin(x, y, 0, isBig);
        }
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

    // message pour dire de lancer la partie
    if(buffer[0] == 'W'){
        if (n < 0) {
            perror("Erreur lors de la réception de la confirmation");
            close(client.recieve_sockfd);
            return;
        }
        
        set_go(1);
    }
    
}

void Partie::recieveTank(){

    char buffer[1024];
    socklen_t addr_len = sizeof(client.recieve_servaddr);

    ssize_t n = recvfrom(client.recieve_sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client.recieve_servaddr, &addr_len);
    // Vérification des erreurs
    if (n <= 0) {
        std::cerr << "Erreur de réception ou message vide." << std::endl;
        return;
    }

    buffer[n] = '\0';  // Ajout du caractère de fin de chaîne
    std::string message(buffer);
    
    std::istringstream stream(message);
    char type;
    stream >> type;
    if (buffer[0] == 'B') { 
        while (!stream.eof()) {
            int joueur_id;
            int type;

            stream >> joueur_id >> type;
            
            // modfif du type du tank du joueur
            if (type == 1) {
                joueur[joueur_id].setTank(std::make_unique<Tank_classique>());
            }
            else if (type == 2) {
                joueur[joueur_id].setTank(std::make_unique<Tank_rapide>());
            }
            else if (type == 3) {
                joueur[joueur_id].setTank(std::make_unique<Tank_healer>());
            }
            else if (type == 4) {
                joueur[joueur_id].setTank(std::make_unique<Tank_mortier>());
            }
            else if (type == 5) {
                joueur[joueur_id].setTank(std::make_unique<Tank_solide>());
            }
            else if (type == 6) {
                joueur[joueur_id].setTank(std::make_unique<Tank_sniper>());
            }
            // std::cout << "J " << joueur_id << " T " << type << "\n";
            
            
        }
    }    
}


int Partie::Solo() {
    // Libérer la mémoire si une fenêtre existait déjà
    if (window) {
        delete window;
    }
    
    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "SOLO");
    windowSize = window->getSize();
    window->setMouseCursorVisible(true);
    
    joueur_courant = 0;
    nbJoueur =  1;

    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    selectionTank();

    window->clear();

    window->setMouseCursorVisible(false);

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    sf::Texture texturetest;
    texturetest.loadFromFile("Image/classique/base_classique.png");
    sf::Sprite cursorSprite(textureCurseur);
    testSprite.setTexture(texturetest);
    cursorSprite.setScale(0.08f, 0.08f);
    testSprite.setScale(0.08f, 0.08f);
    testSprite.setPosition(300, 300);

    // Boucle de jeu
    while (window->isOpen()) {
        getEvent();
        update();
        renderWindow(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(8));

    }

    return 0;
}

int Partie::multiJoueur() {
    joueur_courant = 0;
    
    std::thread connexionThread(&Client::initconnexion, &this->client);

    affichageConnexion();
    initialiserGameOverUI();
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
    
    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    sf::Texture texturetest;
    texturetest.loadFromFile("Image/classique/base_classique.png");
    sf::Sprite cursorSprite(textureCurseur);
    testSprite.setTexture(texturetest);
    cursorSprite.setScale(0.08f, 0.08f);
    testSprite.setScale(0.08f, 0.08f);
    testSprite.setPosition(300, 300);

    
    int numport = client.num_port;
    client.num_port = 3000;         //creation du port d'envoie sur le port 3000
    client.createSocket();
    client.num_port = numport;
    client.createBindedSocket();    //creation du port d'écoute sur le port dédié au client
    
    fondTexture.loadFromFile("Image/Keep_Off_The_Grass.png");
    fondSprite.setTexture(fondTexture);
    fondSprite.setScale(
        static_cast<float>(window->getSize().x) / fondSprite.getGlobalBounds().width,
        static_cast<float>(window->getSize().y) / fondSprite.getGlobalBounds().height
    );
    // CHOIX DU TANK
    int choix_tank = -1;
    for (int i = 0; i < nbJoueur; i++) {
        joueur[i].setTank(std::make_unique<Tank_blanc>());
    }
    
    // sélection par le joueur;
    choix_tank = selectionTank();
    sendTank(choix_tank);

    std::thread tankThread([this]() {
        int count = 0;
        while (window->isOpen()) {

            recieveTank();
            count = 0;
            for (int i = 0; i < nbJoueur; i++) {
                if (joueur[i].Tank->get_type() != 0) count++;
            }

            if (count == nbJoueur) {
                sendReceptionTank();
                break; // si tous les tanks sont mis à jours on stop la reception
            }
        }
   
    });

    std::thread recievethread([this]() { //thread qui recoit les données processed par le server
        while (window->isOpen() && !partieFinie.load()) {
            recieveData();
        }
    });
    
    // Tant que pas recu du serveur que tous les joueurs ont tous les tanks
    while (!get_go()) {
        affichageAttenteTank();
    }

    if (tankThread.joinable()) {
        std::cout << "Fermeture du thread de sélection des tanks..." << std::endl;
        tankThread.join();
        std::cout << "Thread de sélection des tanks terminé proprement." << std::endl;
    }    
    
    //JEU 
    window->setMouseCursorVisible(false);
    
    // **Boucle de jeu avec timer de fin**
    std::chrono::time_point<std::chrono::steady_clock> finPartieTime; // Stocker le moment de fin

    while (window->isOpen()) {
        getEvent();
        sendData();
        renderWindow(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(8)); // Délai pour éviter une boucle trop rapide

        if (partieFinie.load()) {
            // ✅ Démarrer le timer dès que la partie est terminée
            if (finPartieTime.time_since_epoch().count() == 0) {
                finPartieTime = std::chrono::steady_clock::now();
                std::cout << "Fin de la partie signalée, arrêt dans 3 secondes..." << std::endl;
            }

            // ✅ Vérifier si 3 secondes se sont écoulées
            auto elapsed = std::chrono::steady_clock::now() - finPartieTime;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 3) {
                break; // Sortir de la boucle de jeu après 3 secondes
            }
        }
    }

    if (recievethread.joinable()) {
        std::cout << "Fermeture du thread de réception..." << std::endl;
        recievethread.join();
        std::cout << "Thread terminé proprement." << std::endl;
    }

    if (window) {
        delete window;
        window = nullptr;
    }
    
    std::cout << "Fenêtre supprimée proprement." << std::endl;
    
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


void Partie::affichageAttenteTank() {
    
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Image/imagechargement.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }

    
    sf::Sprite backgroundSprite(backgroundTexture);
    // backgroundSprite.setScale(
    //     static_cast<float>(window->getSize().x) / backgroundSprite.getGlobalBounds().width,
    //     static_cast<float>(window->getSize().y) / backgroundSprite.getGlobalBounds().height
    // );

        // Fonction pour ajuster la taille du fond
    auto updateBackgroundSize = [&]() {
        backgroundSprite.setScale(
            static_cast<float>(window->getSize().x) / backgroundSprite.getLocalBounds().width,
            static_cast<float>(window->getSize().y) / backgroundSprite.getLocalBounds().height
        );
    };

    updateBackgroundSize(); // Appliquer le scale une fois

    // Initialiser la vue pour qu'elle corresponde toujours à la fenêtre
    sf::View view(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
    window->setView(view);

    sf::Texture tankChargementTexture;
    if (!tankChargementTexture.loadFromFile("Image/tank_chargement.png")) {
        std::cerr << "Erreur : Impossible de charger l'image du tank de chargement.\n";
    }
    sf::Sprite tankChargementSprite(tankChargementTexture);
    tankChargementSprite.setPosition(100, window->getSize().y - tankChargementSprite.getGlobalBounds().height - 230);

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

    sf::Text statusText("Les adversaires selectionnent leur tank...", font, 50);
    statusText.setFillColor(sf::Color::White);
    statusText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = statusText.getLocalBounds();
    statusText.setPosition((window->getSize().x - textBounds.width) / 2.0f, 200);

    sf::RectangleShape barOutline(sf::Vector2f(window->getSize().x * 0.7, 30));
    barOutline.setFillColor(sf::Color::Transparent);
    barOutline.setOutlineThickness(3);
    barOutline.setOutlineColor(sf::Color::White);
    barOutline.setPosition(window->getSize().x / 2 - barOutline.getSize().x / 2, window->getSize().y * 0.8);

    sf::RectangleShape barFill(sf::Vector2f(0, 30));
    barFill.setFillColor(sf::Color::White);
    barFill.setPosition(barOutline.getPosition().x, window->getSize().y * 0.8);

    float obusSpeed = 5.0f;
    sf::Clock clock;
    sf::Clock timerClock;
    bool oneSecondPassed = false;
    int xexplosion, yexplosion;

    while (!get_go()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
        }

        if (!oneSecondPassed && timerClock.getElapsedTime().asSeconds() >= 1.0f) {
            oneSecondPassed = true;
        }

        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();


        textBounds = statusText.getLocalBounds();
        statusText.setPosition((window->getSize().x - textBounds.width) / 2.0f, 200);

        obusSprite.move(obusSpeed * deltaTime * 100, 0);

        if (obusSprite.getPosition().x > window->getSize().x - 200) {
            if (!explosionActive) {
                explosionActive = true;
                explosionClock.restart();
                xexplosion = obusSprite.getPosition().x + 50;
                yexplosion = obusSprite.getPosition().y + 80;
            }
            obusSprite.setPosition(obusStartX, obusStartY);
        }

        int count = 0;
        for (int i = 0; i < nbJoueur; i++) {
            if (joueur[i].Tank->get_type() != 0) count++;
        }

        float progress = static_cast<float>(count) / nbJoueur;
        barFill.setSize(sf::Vector2f(barOutline.getSize().x * progress, 30));

        window->clear();
        window->draw(backgroundSprite);
        window->draw(statusText);
        window->draw(tankChargementSprite);
        window->draw(obusSprite);
        window->draw(barOutline);
        window->draw(barFill);

        if (explosionActive) {
            renderExplosion(xexplosion, yexplosion);
            window->draw(explosionSprite);
        }

        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    }
}


int Partie::nb_obus() {
    int compteur_obus = 0;

    for (int i = 0; i < nbJoueur; i++) {
        tank& mon_tank = *(joueur[i].Tank);
    
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
        tank& mon_tank = *(joueur[i].Tank);
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


int Partie::selectionTank() {
    
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }

    struct TankInfo {
        std::string name;
        std::string imagePath;
        int speed, fireRate, damage, range, bulletSpeed, health;
    };

    std::vector<TankInfo> tanks = {
        {"Tank Classique", "Image/classique/tankClassique.png", 3, 3, 2, 4, 3, 4},
        {"Tank Rapide", "Image/petit/petit_tank.png", 5, 5, 1, 2, 5, 1},
        {"Tank Soigneur", "Image/healer/tankHealer.png", 3, 3, 2, 3, 3, 2},
        {"Tank Mortier", "Image/mortier/mortier.png", 2, 3, 2, 3, 3, 3},
        {"Tank Solide", "Image/solide/tank_solide.png", 1, 1, 4, 2, 2, 4},
        {"Tank Sniper", "Image/sniper/sniper_tank.png", 2, 1, 3, 5, 4, 3}
    };

    std::vector<sf::Texture> textures(tanks.size());
    std::vector<sf::Sprite> sprites(tanks.size());
    std::vector<Bouton> boutons;
    std::vector<sf::Text> tankNames;

    int tankWidth = 150, tankHeight = 250;
    int squareSize = 20;
    int numColumns = 3;
    int numRows = (tanks.size() + numColumns - 1) / numColumns;

    int windowWidth = window->getSize().x;
    int windowHeight = window->getSize().y;

    int paddingX = 200;  // Augmentation de l'espacement horizontal entre les tanks
    int paddingY = 80;   // Espacement vertical déjà ajusté précédemment

    int startX = (windowWidth - numColumns * (tankWidth + paddingX)) / 2 + paddingX / 2;
    int startY = (windowHeight - numRows * (tankHeight + paddingY + 150)) / 2;

    for (size_t i = 0; i < tanks.size(); ++i) {
        int col = i % numColumns, row = i / numColumns;
        int x = startX + col * (tankWidth + paddingX);
        int y = startY + row * (tankHeight + paddingY + 150);

        if (!textures[i].loadFromFile(tanks[i].imagePath)) {
            std::cerr << "Impossible de charger l'image pour " << tanks[i].name << "\n";
        }
        sprites[i].setTexture(textures[i]);
        sprites[i].setOrigin(textures[i].getSize().x / 2, textures[i].getSize().y / 2);
        sprites[i].setRotation(90);
        sprites[i].setPosition(x + tankWidth / 2, y + tankHeight / 2);
        sprites[i].setScale(0.2f, 0.2f);


        tankNames.emplace_back(tanks[i].name, font, 25);
        tankNames.back().setPosition(x + (tankWidth - tankNames.back().getLocalBounds().width) / 2, y + 20);

        boutons.emplace_back(x + (tankWidth - 150) / 2, y + 200, 150, 40, "Choisir", font);
    }

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return -1;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                if (boutons[0].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_classique>());
                    return 1;
                } else if (boutons[1].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_rapide>());
                    return 2;
                
                } else if (boutons[2].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_healer>());
                    return 3;
                } else if (boutons[3].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_mortier>());
                    return 4;
                } else if (boutons[4].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_solide>());
                    return 5;
                } else if (boutons[5].isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_sniper>());
                    return 6;
                }
            }
        }

        window->clear(sf::Color::Black);
        for (size_t i = 0; i < tanks.size(); ++i) {
            window->draw(tankNames[i]);
            window->draw(sprites[i]);
            boutons[i].draw(*window);

            std::vector<int> stats = {tanks[i].speed, tanks[i].fireRate, tanks[i].damage, tanks[i].range, tanks[i].bulletSpeed, tanks[i].health};
            std::vector<std::string> labels = {"Vitesse", "Cadence", "Degats", "Portee", "Vitesse Obus", "Vie"};

            int statsStartX = startX + (i % numColumns) * (tankWidth + paddingX) + (tankWidth - 5 * (squareSize + 5)) / 2;
            int statsStartY = startY + (i / numColumns) * (tankHeight + paddingY + 150) + tankHeight + 20;

            for (size_t j = 0; j < stats.size(); ++j) {
                for (int k = 0; k < 5; ++k) {
                    sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
                    square.setPosition(statsStartX + k * (squareSize + 5), statsStartY + j * (squareSize + 5));
                    square.setFillColor(k < stats[j] ? sf::Color::White : sf::Color(50, 50, 50));
                    window->draw(square);
                }
                sf::Text statLabel(labels[j], font, 15);
                float textWidth = statLabel.getLocalBounds().width;
                float totalWidth = 5 * (squareSize + 5);
                statLabel.setPosition(statsStartX + (totalWidth / 2 - textWidth / 2) - 6 * squareSize , statsStartY + j * (squareSize + 5));
                
                window->draw(statLabel);
            }
        }
        window->display();
    }
    return 0;
}


void Partie::affiche_type_tank() {
    std::cout << "Les types de tanks de la parties sont : \n";
    for (int i = 0; i < nbJoueur; i++) {
        std::cout << "joueur " << i << " ";
        joueur[i].afficherTypeTank();
    }
}

void Partie::initialiserGameOverUI() {
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Erreur de chargement de la police" << std::endl;
    }

    // Initialisation du texte "GAME OVER"
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);

    // Centrage du texte
    gameOverText.setPosition(
        window->getSize().x / 2 - gameOverText.getGlobalBounds().width / 2,
        window->getSize().y / 3 - gameOverText.getGlobalBounds().height / 2
    );

    // Initialisation des boutons (créés dynamiquement)
    boutonScore = Bouton(window->getSize().x / 2 - 100, window->getSize().y / 2, 200, 50, "Tableau Score", font);
    boutonReplay = Bouton(window->getSize().x / 2 - 100, window->getSize().y / 2 + 70, 200, 50, "Voir Partie", font);
}

