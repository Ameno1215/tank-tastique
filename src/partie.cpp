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
                    printf("bouton replay cliqué");
                    boutonScore.clicked = false;
                    visionnage = true;
                }
                if(boutonScore.isClicked(joueur[joueur_courant].worldMousePos)){
                    boutonReplay.clicked = false;
                    printf("bouton score cliqué");
                    visionnage = false;
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
    backgroundBounds = fondSprite.getGlobalBounds();

    if (joueur[joueur_courant].Spressed) {
        
        deplacement_verticale(mon_tank, rotation, speed); //on avance

        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite, backgroundBounds); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, -2*speed);
        }
    }

    if (joueur[joueur_courant].Zpressed) {
        deplacement_verticale(mon_tank, rotation, -speed);

        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite, backgroundBounds); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, 2*speed);
        }
    }

    if (joueur[joueur_courant].Dpressed){
        deplacement_rotation(mon_tank, &rotation, 1.2);
        
        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite, backgroundBounds); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_rotation(mon_tank, &rotation, -1.2);
        }
    }
    
    if (joueur[joueur_courant].Qpressed){
        deplacement_rotation(mon_tank, &rotation, -1.2);
        joueur[joueur_courant].Tank->updateHitbox(); //on met à jour la hitBox
        joueur[joueur_courant].Tank->updateCollision(testSprite, backgroundBounds); //check si ca touche un truc

        if (joueur[joueur_courant].Tank->isColliding()) { // si c'est le cas on recul
            deplacement_rotation(mon_tank, &rotation, 1.2);
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
                                joueur[i].pV--;
                            }
                            if(joueur[i].pV == 0){
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
            int index = mon_tank.getListeObus().ajouterFin(mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getRotation(), 0.2, 500, "Image/obus.png");
            mon_tank.getListeObus().set_time_dernier_tir(get_time_seconds());
            mon_tank.getListeObus().trouverNoeud(index)->obus.initTir(mon_tank.getTourelleSprite().getRotation(), mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y);
        }
    }

    testGagnant();
}

//cette fonction renvoie -1 si il y a plusieurs joueurs en jeu, l'indice du joueur gagnant sinon
int Partie::testGagnant(){
    int compt = 0;
    int joueurVivant;

    for(int i = 0; i<NB_JOUEUR; i++){
        
        if(joueur[i].pV > 0){
            compt ++;
            joueurVivant = i;
        }

    }

    if(compt == 1){ //si il reste que un seul joueur
        std::cout<<"Joueur "<<joueurVivant<<" a GAGNÉÉÉÉÉ"<<std::endl;
        partieFinie = true;
        return joueurVivant;
    }

    return -1;
}

void Partie::renderWindow(int multi) {

    window->clear();

    window->draw(fondSprite);

    // Obtenir les dimensions du fond
    backgroundBounds = fondSprite.getGlobalBounds();

    // Création de la View proportionnelle au background
    float scaleFactor = 0.5f; // Modifier selon le niveau de zoom souhaité
    view.setSize(backgroundBounds.width * scaleFactor, backgroundBounds.height * scaleFactor);

    // Récupérer la position du tank
    sf::Vector2f tankPos = joueur[joueur_courant].Tank->getBaseSprite().getPosition();

    // Calculer les limites de la vue pour qu'elle ne dépasse pas le background
    float halfViewWidth = view.getSize().x / 2.0f;
    float halfViewHeight = view.getSize().y / 2.0f;

    float minX = backgroundBounds.left + halfViewWidth;
    float maxX = backgroundBounds.left + backgroundBounds.width - halfViewWidth;
    float minY = backgroundBounds.top + halfViewHeight;
    float maxY = backgroundBounds.top + backgroundBounds.height - halfViewHeight;

    // Limiter la position de la vue pour qu'elle ne sorte pas du background
    float Xview = std::clamp(tankPos.x, minX, maxX); //si pos < minX -> pos = minX, symétrie pour > maxX et reste inchangée sinon
    float Yview = std::clamp(tankPos.y, minY, maxY);

    view.setCenter(Xview, Yview);

    window->setView(view); // Appliquer la View

    //position pour les boutons en fin de partie (c'est plus simple de les mettre la)
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f bottomLeftPosition(viewCenter.x - viewSize.x / 2 + 20, viewCenter.y + viewSize.y / 2 - 60);
    sf::Vector2f upperBottomLeftPosition(viewCenter.x - viewSize.x / 2 + 20, viewCenter.y + viewSize.y / 2 - 150);

    int gagnant = testGagnant();

    if(gagnant < 0){ //pas de gagnant 

        if(joueur[joueur_courant].pV > 0 || visionnage){  //Joueur courant pas game over ou est en train de regarder la partie

            //affichage Tank
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
                            
                            joueur[joueur_id].Tank->getListeObus().ajouterFin(static_cast<int>(x), static_cast<int>(y), rotation, 10000, 500, "Image/obus.png");
                            // joueur[joueur_id].Tank.getListeObus().afficher();
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

            // Affichage des PV
            sf::Vector2f viewCenter = view.getCenter();
            sf::Vector2f viewSize = view.getSize();
            sf::Vector2f pvBasePosition(viewCenter.x - viewSize.x / 2 + 20, viewCenter.y + viewSize.y / 2 - 50);  // Calcul de la position des PV en bas à gauche de la vue
            for (int i = 0; i < joueur[joueur_courant].pV; i++) {
                getpvSprite().setPosition(pvBasePosition.x + i * 40, pvBasePosition.y);
                window->draw(getpvSprite());
            }
 
            //Affichage tableau des scores
            if(joueur[joueur_courant].Tabpressed){
                afficheTableauScore();
            }
            
            else if(visionnage){
                boutonScore.setPosition(upperBottomLeftPosition);
                boutonScore.draw(*window);
            }
            if(boutonScore.clicked){
                printf("tessst");
            }
            window->draw(testSprite);
        }
        else{ //si joueur courant est game over et ne regarde pas la partie

            if(boutonScore.clicked){
                boutonReplay.setPosition(bottomLeftPosition);
                boutonReplay.draw(*window);
                afficheTableauScore();
            }
            else if(boutonReplay.clicked){
                boutonScore.setPosition(upperBottomLeftPosition);
                boutonScore.draw(*window);
            }
            else if(joueur[joueur_courant].pV < 1 && !visionnage){
                // Centrer le texte sur la fenêtre
                sf::Vector2f viewCenter = view.getCenter();
                sf::FloatRect textRect = gameOverText.getLocalBounds();
                gameOverText.setOrigin(textRect.width / 2, textRect.height / 2); // Définir l'origine au centre du texte
                gameOverText.setPosition(viewCenter); // Positionner le texte au centre de la vue
                window->draw(gameOverText);

                // Définir une nouvelle position en dessous du texte
                sf::Vector2f belowGameOverText(viewCenter.x, viewCenter.y + textRect.height + 50); // 50 pixels en dessous du texte
                sf::Vector2f belowGameOverText2(viewCenter.x, viewCenter.y + textRect.height + 130); // 130 pixels en dessous du texte

                // Centrer les boutons horizontalement
                sf::FloatRect replayBounds = boutonReplay.getLocalBounds(); // Supposons que cette méthode existe
                sf::FloatRect scoreBounds = boutonScore.getLocalBounds();   // Supposons que cette méthode existe

                boutonReplay.setPosition(sf::Vector2f(viewCenter.x - replayBounds.width / 2, belowGameOverText.y));
                boutonScore.setPosition(sf::Vector2f(viewCenter.x - scoreBounds.width / 2, belowGameOverText2.y));

                // Dessiner les boutons
                boutonReplay.draw(*window);
                boutonScore.draw(*window);
            }
        }
    }
    else if(gagnant == joueur_courant){  // si le joueur courant est le gagnant

        sf::Text victoryText;
        victoryText.setFont(font);
        victoryText.setString("VICTOIRE !!!");
        victoryText.setCharacterSize(100); 
        victoryText.setFillColor(sf::Color::Green); 
        victoryText.setStyle(sf::Text::Bold);
        
        sf::FloatRect textRect = victoryText.getLocalBounds();
        victoryText.setOrigin(textRect.width / 2, textRect.height / 2); // Définir l'origine au centre du texte
        victoryText.setPosition(viewCenter); // Positionner le texte au centre de la vue

        // Afficher le texte
        window->draw(victoryText);
    }
    else{ // si le joueur n'est pas gagnant
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.width / 2, textRect.height / 2); // Définir l'origine au centre du texte
        gameOverText.setPosition(viewCenter); // Positionner le texte au centre de la vue
        window->draw(gameOverText);

        boutonScore.draw(*window);
        boutonReplay.draw(*window);
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

        go = 1;
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
                joueur[joueur_id].setTank(std::make_unique<Tank_vert>());
            }
            else if (type == 2) {
                joueur[joueur_id].setTank(std::make_unique<Tank_bleu>());
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
    texturetest.loadFromFile("Image/base1.png");
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
    texturetest.loadFromFile("Image/base1.png");
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
    fondSprite.setScale(2, 2);
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
    while (!go) {
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
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
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
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
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

    while (!go) {
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

    // Récupérer le centre et la taille de la vue
    sf::Vector2f viewCenter = view.getCenter();

    // Définir la taille de la boîte centrale
    sf::Vector2f scoreBoxSize(400, 300); // Largeur x Hauteur
    sf::RectangleShape scoreBox(scoreBoxSize);
    scoreBox.setFillColor(sf::Color(0, 0, 0, 150)); // Fond noir semi-transparent
    scoreBox.setOutlineThickness(3);
    scoreBox.setOutlineColor(sf::Color::White);

    // Centrer la boîte dans la vue
    scoreBox.setPosition(viewCenter.x - scoreBoxSize.x / 2, viewCenter.y - scoreBoxSize.y / 2);

    window->draw(scoreBox); // Affichage de la boîte

    // Charger une police
    static sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Erreur: Impossible de charger la police!" << std::endl;
        return;
    }

    // Afficher le titre
    sf::Text title("Tableau des Scores", font, 24);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(scoreBox.getPosition().x + (scoreBoxSize.x - title.getLocalBounds().width) / 2,
                      scoreBox.getPosition().y + 20); // Titre centré horizontalement
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

        // Centrer le texte horizontalement dans la boîte
        playerText.setPosition(scoreBox.getPosition().x + (scoreBoxSize.x - playerText.getLocalBounds().width) / 2,
                               scoreBox.getPosition().y + 60 + i * 30); // Position verticale ajustée

        window->draw(playerText);
    }
}

int Partie::selectionTank() {
    sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }

    // Création des boutons
    Bouton boutonTankVert(700, 400, 200, 50, "Tank Vert", font);
    Bouton boutonTankBleu(1000, 400, 200, 50, "Tank Bleu", font);

    sf::Text title("Selection du Tank", font, 40);
    title.setPosition(750, 250);

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return -1;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f mousePos = window->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                if (boutonTankVert.isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_vert>());
                    return 1;
                } else if (boutonTankBleu.isClicked(mousePos)) {
                    joueur[joueur_courant].setTank(std::make_unique<Tank_bleu>());
                    return 2;
                }
            }
        }

        sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        boutonTankVert.update(mousePos);
        boutonTankBleu.update(mousePos);

        window->clear(sf::Color::Black);
        window->draw(title);
        boutonTankVert.draw(*window);
        boutonTankBleu.draw(*window);
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
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
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

