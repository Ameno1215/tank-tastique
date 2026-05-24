#include "partie.hpp"

Partie::Partie() {
    nbJoueur = 0;
    port_actuel = 0;
    joueur_courant = 0;
    if (!pvTexture.loadFromFile("Image/pv.png")) {
        std::cerr << "Erreur lors du chargement de la texture des PV !\n";
    }

    pvSprite.setTexture(pvTexture);
    pvSprite.setScale(0.06f, 0.06f);

    mursTextures.resize(30);

    for (int i = 0; i < 16; i++) {
        std::string filename = "Image/murs/mur" + std::to_string(i + 1) + ".png";

        if (!mursTextures[i].loadFromFile(filename)) {
            std::cerr << "Erreur : Impossible de charger " << filename << std::endl;
        } else {
            sf::Sprite sprite;
            sprite.setTexture(mursTextures[i]);
            sprite.setScale(scale[i]);
            sprite.setPosition(positions[i]); // Appliquer la position unique
            mursSprites.push_back(sprite);
        }
    }

    for (int i = 16; i < 30; i++) {
        std::string filename = "Image/eaux/eau" + std::to_string(i - 15) + ".png";

        if (!mursTextures[i].loadFromFile(filename)) {
            std::cerr << "Erreur : Impossible de charger " << filename << std::endl;
        } else {
            sf::Sprite sprite;
            sprite.setTexture(mursTextures[i]);
            sprite.setScale(scale[i]);
            sprite.setPosition(positions[i]); // Appliquer la position unique
            mursSprites.push_back(sprite);
        }
    }


    for (int i = 0; i < 11; i++) {
        std::string filename = "Image/explosion/explosion_frame" + std::to_string(i + 1) + ".png";
        if (!explosionTextureFrames[i].loadFromFile(filename)) {
            std::cerr << "Erreur : Impossible de charger l'image d'explosion (" << filename << ").\n";
        }
    }

    for (int i = 0; i < 4; i++) {
        if (!regenTextures.loadFromFile("Image/base_vert.png")) {
            std::cerr << "Erreur : Impossible de charger " << "Image/base_vert.png" << std::endl;
        } else {
            regen[i][0] = 0; // y en a pas au debut
            regen[i][1] = 0;
            regen[i][2] = 0;
            sf::Sprite sprite;
            sprite.setTexture(regenTextures);
            sprite.setScale(0.2f, 0.2f);
            sf::Vector2f pos(regen[i][1], regen[i][2]);
            sprite.setPosition(pos);
            regenSprites.push_back(sprite);
        }
    }
    explosionSprite.setScale(4.0f,4.0f);
}

Partie::~Partie() {
    if (window) {
        delete window;
        window = nullptr;
    }
}

sf::Sprite& Partie::getpvSprite() { return pvSprite; }

//ajoute jusqu'à 6 joueur dans le tableau de joueurs
bool Partie::ajouteJoueur() {
    if (nbJoueur < 6) {
        port_actuel = 3001 + nbJoueur;
        joueur[nbJoueur].port = port_actuel;
        joueur[nbJoueur].id = nbJoueur; 
        nbJoueur++;
        return true;
    } else {
        std::cout << "Nombre maximal de joueurs atteint !" << std::endl;
        return false;
    }
}

void Partie::set_go(int val)  {go = val; }

int Partie::get_go() { return go; }

void Partie::set_coeur_ajouter(int val) { coeurAjouter = val; }

int Partie::get_coeur_ajouter() { return coeurAjouter; }

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

void Partie::set_nbJoueur(int i){
    nbJoueur = i;
}

//recupère les event : souris, click, touches et fermeture
void Partie::getEvent() { 

    if (!window) return;

    sf::Event event;

    while (window->pollEvent(event)) {

        if (event.type == sf::Event::Closed){

            //fermeture de la fenêtre
            window->close();

            //fermeture du processus contenant eventuellement le server
            std::ifstream pidFile("server.pid");
            if (!pidFile) {
                std::cerr << "[Client] Pas de server associé au processus client\n";
            }

            int pid;
            pidFile >> pid;
            if (pid > 0) {
                std::cout << "\nArrêt du serveur...\n";
                kill(pid, SIGTERM);  // Envoie SIGTERM au serveur
            }

            exit(0);

        }
            
    }
    
    joueur[joueur_courant].resetInputs();

    if (window->hasFocus()) { //uniquement si on est sur la fenetre 

        joueur[joueur_courant].captureLocalInput(*window, joueur[joueur_courant].pV > 0);
        
        if(joueur[joueur_courant].pV <= 0){ //si joueur game over

            //update des boutons replay et score
            boutonReplay.update(joueur[joueur_courant].worldMousePos);
            boutonScore.update(joueur[joueur_courant].worldMousePos);

            //recup le click pour les boutons
            joueur[joueur_courant].Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);

            if(joueur[joueur_courant].Clicked){

                if(boutonReplay.isClicked(joueur[joueur_courant].worldMousePos)){
                    boutonScore.clicked = false;  //logiquement l'autre est false
                    visionnage = true;            //en mode visionnage
                }

                if(boutonScore.isClicked(joueur[joueur_courant].worldMousePos)){
                    boutonReplay.clicked = false;
                    visionnage = false;  
                }

            }
        }
    }
}

void Partie::update() {

    tank& mon_tank = *(joueur[joueur_courant].Tank); //récupération du tank

    //-----------------------Deplacement-------------------------

    sf::Vector2f dir = joueur[joueur_courant].worldMousePos - mon_tank.getTourelleSprite().getPosition();  
    
    float speed = mon_tank.get_vit();
    float rotation = mon_tank.get_ori();
    float vit_canon = mon_tank.get_vit_canon();
    backgroundBounds = fondSprite.getGlobalBounds();

    if (joueur[joueur_courant].Spressed) {
        
        deplacement_verticale(mon_tank, rotation, speed);                                   //on avance
        mon_tank.updateHitbox();                                                            //on met à jour la hitBox
        hitboxes[joueur_courant] = mon_tank.get_tankHitbox();                               // on rajoute la hitbox à la liste
        mon_tank.updateCollision(hitboxes, backgroundBounds, joueur_courant, mursSprites);  //check si ca touche un truc
        updateRegen();                                                                      
        if (joueur[joueur_courant].Tank->isColliding()) {                                   // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, -2*speed);
        }
        
    }

    if (joueur[joueur_courant].Zpressed) {
        deplacement_verticale(mon_tank, rotation, -speed);

        mon_tank.updateHitbox(); //on met à jour la hitBox
        hitboxes[joueur_courant] = mon_tank.get_tankHitbox();
        mon_tank.updateCollision(hitboxes, backgroundBounds, joueur_courant, mursSprites); //check si ca touche un truc
        updateRegen();
        if (mon_tank.isColliding()) { // si c'est le cas on recul
            deplacement_verticale(mon_tank, rotation, 2*speed);
        }
    }

    if (joueur[joueur_courant].Dpressed){
        deplacement_rotation(mon_tank, &rotation, 1.2);
        
        mon_tank.updateHitbox(); //on met à jour la hitBox
        hitboxes[joueur_courant] = mon_tank.get_tankHitbox();
        mon_tank.updateCollision(hitboxes, backgroundBounds, joueur_courant, mursSprites); //check si ca touche un truc
        updateRegen();
        if (mon_tank.isColliding()) { // si c'est le cas on recul
            deplacement_rotation(mon_tank, &rotation, -1.2);
        }
    }
    
    if (joueur[joueur_courant].Qpressed){
        deplacement_rotation(mon_tank, &rotation, -1.2);
        mon_tank.updateHitbox(); //on met à jour la hitBox
        hitboxes[joueur_courant] = mon_tank.get_tankHitbox();
        mon_tank.updateCollision(hitboxes, backgroundBounds, joueur_courant, mursSprites); //check si ca touche un truc
        updateRegen();
        if (mon_tank.isColliding()) { // si c'est le cas on recul
            deplacement_rotation(mon_tank, &rotation, 1.2);
        }
    }
    
    // Mise à jour de l'angle de la tourelle
    float angle_actu = mon_tank.getTourelleSprite().getRotation();
    float angle_voulu = atan2(dir.y, dir.x) * 180 / M_PI - 90;
    float diff = angle_voulu - angle_actu;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    
    mon_tank.getTourelleSprite().setRotation(angle_actu + diff * vit_canon); //rotation tourelle


    //---------------------ULTI----------------------

    if(utltiActive[joueur_courant]==1){
        // tank rapide double dégat
        if (mon_tank.get_type() == 2) {
            mon_tank.set_degat(2);
        }
        // tank solide one shot
        else if (mon_tank.get_type() == 5) {
            mon_tank.set_degat(10);
        }
        // tank sniper augmente cadence de tir
        else if (mon_tank.get_type() == 6) {
            mon_tank.set_cadence_tir(MULT_CADENCE_TIR / 3);
        }
        // tank healer ajoute 2 coeurs
        else if (mon_tank.get_type() == 3) {
            // si les coeurs sont pas encore ajouter on les ajoute
            if (!get_coeur_ajouter()) {
                joueur[joueur_courant].pV += 2;
                set_coeur_ajouter(1);
            }
        }
    }
    else {
        if (mon_tank.get_type() == 2) {
            mon_tank.set_degat(1);
        }
        else if (mon_tank.get_type() == 5) {
            mon_tank.set_degat(4);
        }
        else if (mon_tank.get_type() == 6) {
            mon_tank.set_cadence_tir(MULT_CADENCE_TIR / 1);
        }
    }


    // ----------------------DEPLACEMENT OBUS -----------------------------

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

                for(int i = 0; i<nbJoueur; i++){

                    if(joueur[i].pV>0){

                        tank& testank = *(joueur[i].Tank);
                        testank.updateHitbox(); //il faut update car si le tank adverse bouge pas c'est pas bon 
                        testank.updateTouched(courant->obus.get_Sprite());
                        
                        if(testank.isTouched()){

                            listexplosion.ajouterFin(courant->obus.get_Sprite().getPosition().x, courant->obus.get_Sprite().getPosition().y, 0, 1); //on rajoute une explosion à afficher

                            if(joueur[i].pV > 0){

                                //condition pour ulti bouclier
                                if(joueur[i].Tank->get_type() ==  1 && utltiActive[i] == 1){

                                    if(joueur[i].Tank->ultiClassicUse == false){
                                        joueur[i].Tank->ultiClassicUse = true;
                                    }
                                    else{
                                        joueur[i].pV -= courant->obus.get_degat();
                                        stat[joueur_courant][3] += courant->obus.get_degat(); //ajoute le nb de dégats
                                    }

                                }
                                else{
                                    joueur[i].pV -= courant->obus.get_degat();
                                    stat[joueur_courant][3] += courant->obus.get_degat(); //ajoute le nb de dégats
                                }

                            }

                            if(joueur[i].pV <= 0){ //si game over

                                joueur[i].pV = -1;
                                listexplosion.ajouterFin(joueur[i].Tank->getBaseSprite().getGlobalBounds().width/2 + joueur[i].Tank->getBaseSprite().getPosition().x, joueur[i].Tank->getBaseSprite().getGlobalBounds().height/2 + joueur[i].Tank->getBaseSprite().getPosition().y, 0, 2);
                                joueur[i].vivant = false;
                                
                                if (i >= 0 && i < static_cast<int>(hitboxes.size())){

                                    hitboxes[i].clear(); // Marque comme "supprimée"

                                }
                            
                            }
                            
                            obusDestructeur = true;
                            courant->obus.set_status(0); // destruction obus
                            Noeud * temp = courant->suivant;
                            mon_tank.getListeObus().supprimer(courant->index);
                            courant = temp;
                            stat[joueur_courant][2]++; //ajoute un obus touché
                            break;
                        }
                    }
                }

                if(!obusDestructeur){
                    if(mon_tank.get_type() == 4 && utltiActive[joueur_courant] == 1){
                        //rien ahahaha
                    }
                    else{
                        for (int i = 0; i < 16; i++) {

                            sf::Sprite obus = courant->obus.get_Sprite();  
                        
                            if (mursSprites[i].getGlobalBounds().intersects(obus.getGlobalBounds())) {
                                obusDestructeur = true;
                                listexplosion.ajouterFin(courant->obus.get_Sprite().getPosition().x, courant->obus.get_Sprite().getPosition().y, 0, 0); //on rajoute une explosion à afficher en petit                   
                            }
                        
                            if (obusDestructeur) {
                                courant->obus.set_status(0);  
                                Noeud* temp = courant->suivant;
                                mon_tank.getListeObus().supprimer(courant->index);  
                                courant = temp;  
                                break; 
                            }
                        }
                    }
                }

                if(!obusDestructeur){ //si l'obus n'a rien rencontré
                    courant = courant->suivant;
                }

            } 
        }
    }


    // ---------------- TIR NOUVEL OBUS ------------------

    if (joueur[joueur_courant].Clicked) {

        if ((get_time_seconds() - mon_tank.getListeObus().get_time_dernier_tir()) > mon_tank.get_cadence_tir()) {

            // ajout création du nouvel obus et l'ajouter à la liste d'obus du tank
            int index = mon_tank.getListeObus().ajouterFin(mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getRotation(), mon_tank.get_vitesse_obus(), mon_tank.get_porte(), "Image/obus.png", mon_tank.get_degat());
            mon_tank.getListeObus().set_time_dernier_tir(get_time_seconds());
            mon_tank.getListeObus().trouverNoeud(index)->obus.initTir(mon_tank.getTourelleSprite().getRotation(), mon_tank.getTourelleSprite().getPosition().x, mon_tank.getTourelleSprite().getPosition().y, mon_tank.getTourelleSprite().getTexture()->getSize().y * mon_tank.getTourelleSprite().getScale().y / 2);
            stat[joueur_courant][1]++; //ajoute obus tiré

        }
    }

    stat[joueur_courant][0] = joueur[joueur_courant].pV;


    // ---------- TEST GAGNANT -------------

    if(client.mode == 1){
        testGagnant();
    }
    else{
        testEquipeGagnant();
    }
}


void Partie::updateRegen(){
    int r = joueur[joueur_courant].Tank->updateRegenCollision(regenSprites);
    if(r > -1 ){
        regen[r][0] = 0; 
        utltiActive[joueur_courant] = 0;
    }
}

//cette fonction renvoie -1 si il y a plusieurs joueurs en jeu, l'indice du joueur gagnant sinon
//met à true la varaible partiefinie si la partie est finie 
int Partie::testGagnant(){
    int compt = 0;
    int joueurVivant;

    for(int i = 0; i<nbJoueur; i++){
        
        if(joueur[i].pV > 0){
            compt ++;
            joueurVivant = i;
        }

    }

    if(compt == 1){ //si il reste que un seul joueur
        partieFinie = true;
        return joueurVivant;
    }

    return -1;
}

//renvoie -1 si pas d'équipe gagnante 1 si equipe 1 et 2 pour l'autre equipe
//met à true la varaible partiefinie si la partie est finie 
int Partie::testEquipeGagnant(){

    int gagnant = -1;
    int compt1 = 0;
    int compt2 = 0;

    for(int i = 0; i < get_nbJoueur(); i++){
        if(joueur[i].equipe == 1 && joueur[i].pV > 0){
            compt1 ++;
        }
        if(joueur[i].equipe == 2 && joueur[i].pV > 0){
            compt2 ++;
        }
    }

    if(compt1 == 0){ //equipe 1 decime
        gagnant = 2;
        partieFinie = true;
    }

    if(compt2 == 0){
        gagnant = 1;
        partieFinie = true;
    }

    return gagnant;

}

//fonction qui s'occupe du rendu de l'affichage
void Partie::renderWindow(int multi) {

    window->clear();

    // ------------- FOND --------------------

    window->draw(fondSprite);

    for(int i=16;i<30;i++){
        window->draw(mursSprites[i]);
    }

    for(int i=0;i<16;i++){
        window->draw(mursSprites[i]);
    }

    //-----------------VIEW--------------------------

    backgroundBounds = fondSprite.getGlobalBounds();

    float scaleFactor = 0.5f;
    view.setSize(backgroundBounds.width * scaleFactor, backgroundBounds.height * scaleFactor);

    sf::Vector2f tankPos = joueur[joueur_courant].Tank->getBaseSprite().getPosition();

    //calcul le centre de la view
    float halfViewWidth = view.getSize().x / 2.0f;
    float halfViewHeight = view.getSize().y / 2.0f;

    float minX = backgroundBounds.left + halfViewWidth;
    float maxX = backgroundBounds.left + backgroundBounds.width - halfViewWidth;
    float minY = backgroundBounds.top + halfViewHeight;
    float maxY = backgroundBounds.top + backgroundBounds.height - halfViewHeight;

    float Xview = std::clamp(tankPos.x, minX, maxX);
    float Yview = std::clamp(tankPos.y, minY, maxY);

    view.setCenter(Xview, Yview); //on centre la view 

    window->setView(view); // Appliquer la View

    //position pour les boutons en fin de partie (c'est plus simple de les mettre la)
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f bottomLeftPosition(viewCenter.x - viewSize.x / 2 + 20, viewCenter.y + viewSize.y / 2 - 60);
    sf::Vector2f upperBottomLeftPosition(viewCenter.x - viewSize.x / 2 + 20, viewCenter.y + viewSize.y / 2 - 150);

    // ----------------- Recupération des gagants ----------
    int gagnant;
    if(client.mode == 2){
        gagnant = testEquipeGagnant();
    }
    else{
        gagnant = testGagnant();
    }

    if(gagnant < 0){ //pas de gagnant 

        if (multi) {
            std::istringstream stream(get_buffer_missile());
            char type = '\0';
            int nbObusRecus = 0;

            if ((stream >> type >> nbObusRecus) && type == 'O') {
                for (int i = 0; i < nbJoueur; i++) {
                    joueur[i].Tank->getListeObus().vider();
                }

                for (int indexObus = 0; indexObus < nbObusRecus; ++indexObus) {
                    int joueur_id = -1;
                    float x = 0.f;
                    float y = 0.f;
                    float rotation = 0.f;

                    if (!(stream >> joueur_id >> x >> y >> rotation)) {
                        std::cerr << "[Client] Paquet d'obus incomplet recu: "
                                  << get_buffer_missile() << std::endl;
                        break;
                    }

                    if (joueur_id < 0 || joueur_id >= nbJoueur || !joueur[joueur_id].Tank) {
                        std::cerr << "[Client] Paquet d'obus invalide pour joueur "
                                  << joueur_id << std::endl;
                        continue;
                    }

                    joueur[joueur_id].Tank->getListeObus().ajouterFin(
                        static_cast<int>(x),
                        static_cast<int>(y),
                        rotation,
                        joueur[joueur_id].Tank->get_vitesse_obus(),
                        joueur[joueur_id].Tank->get_porte(),
                        "Image/obus.png",
                        joueur[joueur_id].Tank->get_degat()
                    );
                }
            }
        }

        if(joueur[joueur_courant].pV > 0 || visionnage){  //Joueur courant pas game over ou est en train de regarder la partie

            // ---------Affichage tank------------
            for(int i = 0; i<nbJoueur; i++){
    
                if(joueur[i].pV > 0){  // Si joueur vivant

                    tank& mon_tank = *(joueur[i].Tank);
                    
                    //------- couleur pour les équipes ------

                    if((joueur[i].equipe == joueur[joueur_courant].equipe) && (client.mode == 2)){

                        sf::Sprite coloredSprite = mon_tank.getBaseSprite();
                        coloredSprite.setColor(sf::Color(100, 100, 255, 255)); // Bleu avec un effet doux
                        window->draw(coloredSprite);
                        sf::Sprite coloredTourelle = mon_tank.getTourelleSprite();
                        coloredTourelle.setColor(sf::Color(100, 100, 255, 255)); // Bleu avec un effet doux
                        window->draw(coloredTourelle);

                    }
                    else{

                        window->draw(mon_tank.getBaseSprite());
                        window->draw(mon_tank.getTourelleSprite());

                    }

                    // -------- OBUS --------------

                    Noeud* courant = mon_tank.getListeObus().get_tete();
                    while (courant) {
                        if (courant->obus.get_status()) {
                            window->draw(courant->obus.get_Sprite());
                        }
                        courant = courant->suivant;
                    
                    }
                }

            }
    
            // -------   Explosions  ------------

            NoeudExplosion* courant = listexplosion.get_tete();
            while (courant) {
                if (courant->frameActu < 20 && courant->frameActu > 0) {
                    courant->explosionSprite.setTexture(explosionTextureFrames[courant->frameActu]);
                    courant->explosionSprite.setPosition(courant->x - courant->explosionSprite.getGlobalBounds().width / 2, courant->y - courant->explosionSprite.getGlobalBounds().height / 2);
                    if(courant->big == 2){
                        courant->explosionSprite.setScale(3.0f, 3.0f);
                    }
                    if(courant->big == 1){
                        courant->explosionSprite.setScale(1.4f, 1.4f);
                    }
                    if(courant->big == 0){
                        courant->explosionSprite.setScale(0.75f, 0.75f);
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

            //affichage regen
            for(int i = 0; i<4; i++){
                if(regen[i][0] == 1){
                    regenSprites[i].setPosition(regen[i][1],regen[i][2]);
                    regenSprites[i].setScale(0.2f, 0.2f);
                    window->draw(regenSprites[i]);
                }
            }
 
            //Affichage tableau des scores
            if(joueur[joueur_courant].Tabpressed){
                afficheTableauScore(0);
            }
            
            if(visionnage){
                boutonScore.setPosition(upperBottomLeftPosition);
                boutonScore.draw(*window);
            }
            else{
                sf::Vector2f ultiPosition(viewCenter.x + viewSize.x / 2 - 125, viewCenter.y + viewSize.y / 2 - 100);
                if(utltiActive[joueur_courant]==0){
                    joueur[joueur_courant].Tank->getSpriteUltiPret().setPosition(ultiPosition.x, ultiPosition.y);
                    window->draw(joueur[joueur_courant].Tank->getSpriteUltiPret());
                }
                if(utltiActive[joueur_courant]==1){
                    joueur[joueur_courant].Tank->getSpriteUlti().setPosition(ultiPosition.x, ultiPosition.y);
                    window->draw(joueur[joueur_courant].Tank->getSpriteUlti());
                }
                afficherMinimap();
            }
            
        }
        else{ //si joueur courant est game over et ne regarde pas la partie

            if(boutonScore.clicked){
                boutonReplay.setPosition(bottomLeftPosition);
                boutonReplay.draw(*window);
                afficheTableauScore(0);
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
    else if(client.mode == 1){ //gagnant (ou pas) en MG
        if(gagnant == joueur_courant){  // si le joueur courant est le gagnant

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
        }
    }
    else{ //gagnant (ou pas) en MME
        if(gagnant == joueur[joueur_courant].equipe){
            sf::Text victoryText;
            victoryText.setFont(font);
            victoryText.setString("VICTOIRE DE L'EQUIPE !");
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

//fonction qui affiche la minimap 
void Partie::afficherMinimap(){

    sf::Vector2f tankPos = joueur[joueur_courant].Tank->getBaseSprite().getPosition();
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();

    float minimapScale = 0.08f; //echelle de la minimap
    minimapBackground.setSize(sf::Vector2f(fondSprite.getGlobalBounds().width * minimapScale,
                                            fondSprite.getGlobalBounds().height * minimapScale));
    minimapBackground.setFillColor(sf::Color(0, 0, 0, 150)); // Noir semi-transparent

    // Placer la minimap en haut à gauche de la view
    minimapBackground.setPosition(viewCenter.x - viewSize.x / 2 + 10,  
                                viewCenter.y - viewSize.y / 2 + 10);

    float tankX_norm = tankPos.x / fondSprite.getGlobalBounds().width;
    float tankY_norm = tankPos.y / fondSprite.getGlobalBounds().height;

    sf::Vector2f minimapTankPos(
        minimapBackground.getPosition().x + tankX_norm * minimapBackground.getSize().x,
        minimapBackground.getPosition().y + tankY_norm * minimapBackground.getSize().y
    );

    // Mettre à jour le point rouge du tank
    tankPoint.setRadius(3.f);
    tankPoint.setFillColor(sf::Color::Red);
    tankPoint.setPosition(minimapTankPos);

    // Dessiner la minimap et le tank sur la fenêtre
    window->draw(minimapBackground);
    window->draw(tankPoint);
}

void Partie::sendData(){
    client.sendInput(joueur[joueur_courant]);
}

void Partie::sendTank(int type) {
    client.sendTankChoice(joueur_courant, type);
}

void Partie::recieveData(){
    char buffer[BUFFER_SIZE];
    ssize_t receivedBytes = 0;

    if (!client.receiveRawPacket(buffer, sizeof(buffer), receivedBytes)) {
        return;
    }

    const std::string packet(buffer, static_cast<std::size_t>(receivedBytes));

    if (const auto tankState = network::parseTankState(packet); tankState.has_value()) {
        joueur[tankState->playerId].applyTankState(tankState.value());
        utltiActive[tankState->playerId] = tankState->ultiState;
        return;
    }

    // RECEPTION DE LA LISTE D'OBUS
    if (!packet.empty() && packet[0] == 'O') { 
        setBufferMissile(packet);
        return;
    }
        
    if (!packet.empty() && packet[0] == 'E') {
        std::istringstream stream(packet.substr(2));  // Supprime 'E ' et crée un flux
        int x, y, big;

        while (stream >> x >> y >> big) {  // Extrait les couples de coordonnées car il peut y avoir plusieurs explosions à la fois
            listexplosion.ajouterFin(x, y, 0, big);
        }
        return;
    }

    if (const auto hp = network::parseHealthState(packet); hp.has_value()) {
        for(int i = 0; i< nbJoueur; i++){
            joueur[i].pV = hp.value()[i];
        }
        return;
    }

    // message pour dire de lancer la partie
    if (network::isReadySignal(packet)) {
        set_go(1);
        return;
    }

    if (network::parseStatsPacket(buffer, static_cast<std::size_t>(receivedBytes), stat)) {
        return;
    }

    if (!packet.empty() && packet[0] == 'R') {
        int values[12];

        int count = std::sscanf(packet.c_str(), "R %d %d %d %d %d %d %d %d %d %d %d %d", 
                                &values[0], &values[1], &values[2], 
                                &values[3], &values[4], &values[5], 
                                &values[6], &values[7], &values[8], 
                                &values[9], &values[10], &values[11]);

        if (count == 12) { 
            for (int i = 0, k = 0; i < 4; ++i) {
                for (int j = 0; j < 3; ++j, ++k) {
                    regen[i][j] = values[k];
                }
            }
        } else {
            std::cerr << " Erreur lors de la lecture des données de regen !" << std::endl;
        }
    }
    
}

//met à jour les tank des autre joueurs
void Partie::recieveTank(){
    std::string packet;
    if (!client.receivePacket(packet)) {
        return;
    }

    if (const auto tankList = network::parseTankList(packet); tankList.has_value()) {
        for (const auto& [joueur_id, type] : tankList.value()) {
            joueur[joueur_id].assignTankByType(type);
        }
    }
    else{
        std::cout<<"[Client] message pour connaitre tank des autres joueurs impossible à lire : "<<packet<<std::endl; 
    }
}

void Partie::initialiserpseudo(){
    const auto& pseudos = client.getPseudos();
    for(int i = 0; i < nbJoueur; i++){
        joueur[i].pseudo = pseudos[i];
    }
}

//attend que le server nous donne combien de joueurs ont choisi leur tank
int Partie::waitOthertank(){
    std::string packet;
    if (!client.receivePacket(packet)) {
        return -1;
    }

    const auto progress = network::parseTankChoiceProgress(packet);
    return progress.value_or(-1);
}

void Partie::recup_equip(){
    if(client.mode == 2){
        const auto& equipes = client.getEquipes();
        for(int i = 0; i < get_nbJoueur(); i++){
            joueur[i].equipe = equipes[i];
            std::cout<<joueur[i].equipe<<std::endl;
        }
    }
}

int Partie::multiJoueur(bool hebergePartieLocalement) {
    hebergePartie = hebergePartieLocalement;
    joueur_courant = 0;
    client.test = TEST;
    std::thread connexionThread(&Client::initconnexion, &this->client);

    affichageConnexion();
    initialiserGameOverUI();
    // Arrêter le thread proprement
    if (connexionThread.joinable()) {
        std::cout<<"[Client] arret du thread de connexion"<<std::endl;
        if(client.get_etatConnexion() == 1){
            std::cout<<"[Client] le client a eu le feu vert"<<std::endl;
        }
        else{
            std::cout<<"[Client] le client N'a PAS eu le feu vert"<<std::endl;
        }
        connexionThread.join();
    }

    nbJoueur = client.nbJoueur;
    joueur_courant = client.joueur.id;
    for (int i = 0; i < nbJoueur; ++i) {
        joueur[i].id = i;
    }
    initialiserpseudo();
    recup_equip(); //ne le fais pas si MG

    if (window) {
        delete window;
        window = nullptr;
    }
    
    char title[50];
    sprintf(title, "MULTIJOUEUR JOUEUR %d", joueur_courant);

    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), title);
    windowSize = window->getSize();
    
    if (!textureCurseur.loadFromFile("Image/curseur_rouge.png")) {
        std::cerr << "[Client] Erreur lors du chargement du curseur !\n";
        return -1;
    }

    cursorSprite.setTexture(textureCurseur);
    cursorSprite.setScale(0.12f, 0.12f);

    sf::Texture texturetest;
    texturetest.loadFromFile("Image/classique/base_classique.png");
    sf::Sprite cursorSprite(textureCurseur);
    cursorSprite.setScale(0.08f, 0.08f);

    fondTexture.loadFromFile("Image/cartef.png");
    fondSprite.setTexture(fondTexture);
    fondSprite.setScale(2, 2);
    
    // --------------------------CHOIX DU TANK--------------------------------
    int choix_tank = -1;
    for (int i = 0; i < nbJoueur; i++) {
        joueur[i].setTank(std::make_unique<Tank_blanc>());
    }
    
    // sélection par le joueur;
    choix_tank = selectionTank(); //recupération choix type de tank via window sfml
    sendTank(choix_tank); //envoie du choix de tank au server
    std::cout<<"[Client] tank envoyé"<<std::endl;

    nbchoix = - 1; //pour etre sur
    std::thread recieveTankChoix([this]() { // thread qui tourne en parallèle de l'affichage d'attente des autre joueurs
        while(nbchoix != nbJoueur){         // tant que pas tout le monde a fait son choix 
            nbchoix = waitOthertank();
        }
        recieveTank(); // 
        go = 1;
    });

    // Tant que pas recu du serveur que tous les joueurs ont tous les tanks
    while (!get_go()) {
        affichageAttenteTank();
    }

    if (recieveTankChoix.joinable()) {
        std::cout << "[Client] Fermeture du thread de réception des tanks..." << std::endl;
        recieveTankChoix.join();
        std::cout << "[Client] Thread de réception des tanks terminé proprement." << std::endl;
    }
    
    //-------------------------------JEU--------------------------------------
    window->setMouseCursorVisible(false);
    
    std::thread recievethread([this]() { //thread qui recoit les données processed par le server
        while (window->isOpen() && !partieFinie.load()) {
            recieveData();
        }
    });

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
                std::cout << "[Client] Fin de la partie signalée, arrêt dans 3 secondes..." << std::endl;
            }

            // ✅ Vérifier si 3 secondes se sont écoulées
            auto elapsed = std::chrono::steady_clock::now() - finPartieTime;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 3) {
                break; // Sortir de la boucle de jeu après 3 secondes
            }
        }
    }

    if (recievethread.joinable()) {
        std::cout << "[Client] Fermeture du thread de la main loop..." << std::endl;
        recievethread.join();
        std::cout << "[Client] Thread de la main loop terminé proprement." << std::endl;
    }
    
    int fin = finDePartie();

    if (window) {
        delete window;
        window = nullptr;
    }
    
    std::cout << "[Client] Fenêtre supprimée proprement." << std::endl;
    return fin;

}

int Partie::finDePartie() {

    if (window) {  
        delete window;
        window = nullptr;
    }

    window = new sf::RenderWindow(sf::VideoMode(1900, 1000), "Tableau des scores");
    windowSize = window->getSize();
    
    if (!fondTexture.loadFromFile("Image/imagechargement.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }

    sf::Sprite backgroundSprite(fondTexture);
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(windowSize.y) / backgroundSprite.getGlobalBounds().height
    );

    sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Erreur : Impossible de charger la police.\n";
    }

    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;

    // Récupérer la vue par défaut
    defaultView = window->getDefaultView();

    // Centrer la vue sur le centre de la fenêtre
    sf::Vector2u windowSize = window->getSize();
    defaultView.setCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);

    // Appliquer la vue
    window->setView(defaultView);

    // Utiliser la vue pour positionner des éléments
    sf::Vector2f center = defaultView.getCenter();
    float centerX = center.x;
    float centerY = center.y;

    Bouton boutonLobby(centerX - buttonWidth - 50, centerY * 1.5, buttonWidth, buttonHeight, "Retourner au Lobby", font);
    Bouton boutonQuitter(centerX + 50, centerY * 1.5, buttonWidth, buttonHeight, "Quitter", font);

    sf::Event event;
    while (window->isOpen()) {
        window->setView(defaultView); // Assurer que la vue reste correcte
        center = defaultView.getCenter(); // Mettre à jour le centre

        centerX = center.x;
        centerY = center.y;
        boutonLobby.setPosition(sf::Vector2f(centerX - buttonWidth - 50, centerY * 1.5));
        boutonQuitter.setPosition(sf::Vector2f(centerX + 50, centerY * 1.5));

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return 0;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window), defaultView);
                if (boutonLobby.isClicked(mousePos)) {
                    return 1;
                }
                if (boutonQuitter.isClicked(mousePos)) {
                    return 0;
                }
            }
        }

        sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window), defaultView);
        boutonLobby.update(mousePos);
        boutonQuitter.update(mousePos);

        window->clear();
        window->draw(backgroundSprite);
        afficheTableauScore(1);

        boutonLobby.draw(*window);
        boutonQuitter.draw(*window);

        // Affichage du curseur centré sur la souris
        cursorSprite.setPosition(
            mousePos.x - cursorSprite.getGlobalBounds().width / 2,
            mousePos.y - cursorSprite.getGlobalBounds().height / 2
        );

        window->draw(cursorSprite);
        window->display();
    }
    
    return 0;
}

void Partie::affichageConnexion() {
    if (window) {  
        delete window;
        window = nullptr;
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

    // Calcul de la position centrée en largeur
    float centerX = windowSize.x / 2.0f;
    int hauteurBloc = 600;
    // Texte d'invite
    sf::Text inviteText("Adresse IP du serveur hote :", font, 60);
    inviteText.setOrigin(inviteText.getLocalBounds().width / 2.0f, 0);
    inviteText.setPosition(centerX, hauteurBloc); // hauteurChoisie est à définir

    std::string ip;
    sf::Text ipText("", font, 60);
    ipText.setOrigin(ipText.getLocalBounds().width / 2.0f, 0);
    ipText.setPosition(centerX, hauteurBloc + 100);
    ipText.setFillColor(sf::Color::Cyan);

    // Texte d'invite
    sf::Text nomText("Rentre ton blase :", font, 60);
    nomText.setOrigin(nomText.getLocalBounds().width / 2.0f, 0);
    nomText.setPosition(centerX, hauteurBloc); // hauteurChoisie est à définir

    sf::Text hostInfoText("", font, 38);
    hostInfoText.setFillColor(sf::Color::White);
    hostInfoText.setPosition(centerX - 350, hauteurBloc + 10);

    std::string hostIp = sf::IpAddress::getLocalAddress().toString();

    // Zone de texte pour le pseudo
    std::string pseudo;
    sf::Text pseudoText("", font, 60);
    pseudoText.setOrigin(pseudoText.getLocalBounds().width / 2.0f, 0);
    pseudoText.setPosition(centerX, hauteurBloc + 100);
    pseudoText.setFillColor(sf::Color::Cyan);

    // Bouton de validation
    Bouton valider(centerX - 100, hauteurBloc + 220, 200, 50, "Valider", font);

    bool validePseudo = false;

    window->clear();
    window->draw(backgroundSprite);
    window->draw(statusText);
    window->display();

    float obusSpeed = 5.0f; 
    sf::Clock clock;
    sf::Clock timerClock;
    bool oneSecondPassed = false;
    int xexplosion, yexplosion;
    bool entree = false;

    while (window->isOpen()) {
        sf::Event event;
        joueur[joueur_courant].mousePos = sf::Mouse::getPosition(*window);                                  //recupération de la position de la souris
        joueur[joueur_courant].worldMousePos = window->mapPixelToCoords(joueur[joueur_courant].mousePos);   //la mettre dans le repère de la fenetre (je crois)
        valider.update(joueur[joueur_courant].worldMousePos);
        joueur[joueur_courant].Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        //maj de la position des entrées
        nomText.setOrigin(nomText.getLocalBounds().width / 2.0f, 0);
        nomText.setPosition(centerX, hauteurBloc); // hauteurChoisie est à définir
        pseudoText.setOrigin(pseudoText.getLocalBounds().width / 2.0f, 0);
        pseudoText.setPosition(centerX, hauteurBloc + 100);
        inviteText.setOrigin(inviteText.getLocalBounds().width / 2.0f, 0);
        inviteText.setPosition(centerX, hauteurBloc); // hauteurChoisie est à définir
        ipText.setOrigin(ipText.getLocalBounds().width / 2.0f, 0);
        ipText.setPosition(centerX, hauteurBloc + 100);

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                entree = true;
            }
            // Détection de la saisie clavier
            if (event.type == sf::Event::TextEntered){
                if(validePseudo){
                    if (event.text.unicode == '\b' && !ip.empty()) { // Suppression avec Backspace
                        ip.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode <= 126) { // Saisie classique
                        ip += static_cast<char>(event.text.unicode);
                    }
                    ipText.setString(ip); // Mise à jour du texte affiché
                }
                else{
                    if (event.text.unicode == '\b' && !pseudo.empty()) { // Suppression avec Backspace
                        pseudo.pop_back();
                    } else if (event.text.unicode >= 32 && event.text.unicode <= 126) { // Saisie classique
                        pseudo += static_cast<char>(event.text.unicode);
                    }
                    pseudoText.setString(pseudo); // Mise à jour du texte affiché
                }
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

        if(TEST){
            client.configureConnection("127.0.0.1", "Blase2test");
            validePseudo = true;  //le pseudo par defaut est envoyé côté client
        }
        else{
            if(!client.ipValide.load() && (joueur[joueur_courant].Clicked || entree)){
                if(valider.isClicked(joueur[joueur_courant].worldMousePos) || entree){
                    if(hebergePartie){
                        if(!pseudo.empty()){
                            entree = false;
                            std::cout << "[Client] Pseudo valide pour l'hote: "
                                      << pseudo << ". IP locale: "
                                      << "127.0.0.1" << std::endl;
                            client.configureConnection("127.0.0.1", pseudo);
                            std::cout << "[Client] Validation de l'IP terminee pour l'hote." << std::endl;
                            validePseudo = true;
                        }
                    }
                    else if(validePseudo && !ip.empty()){
                        entree = false;
                        std::cout << "[Client] IP du serveur saisie: "
                                  << ip << std::endl;
                        client.configureConnection(ip, client.joueur.pseudo);
                        std::cout << "[Client] Validation de l'IP terminee pour le client." << std::endl;
                    }
                    else{
                        if(!pseudo.empty()){
                            entree = false;
                            client.joueur.pseudo = pseudo;
                            std::cout << "[Client] Pseudo saisi: "
                                      << client.joueur.pseudo << std::endl;
                            validePseudo = true;
                        }       
                    }
                }
            }
        }

        window->clear();
        window->draw(backgroundSprite);
        window->draw(statusText);
        window->draw(tankChargementSprite);
        window->draw(obusSprite);

        if(!TEST){
            if(hebergePartie){
                hostInfoText.setString("Les autres joueurs doivent entrer cette IP : " + hostIp);
                window->draw(hostInfoText);
                window->draw(nomText);
                valider.draw(*window);
                window->draw(pseudoText);
            }
            else if(validePseudo){
                window->draw(inviteText);
                valider.draw(*window);
                window->draw(ipText);
            }
            else{
                window->draw(nomText);
                valider.draw(*window);
                window->draw(pseudoText);
            }
        }

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

        int count = nbchoix;

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
    chaine = "O ";
    chaine += std::to_string(nb_obus());
    chaine += '\n';

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


void Partie::afficheTableauScore(int fin) {
    if (!window) return;

    sf::Vector2f positionCentre;

    if (fin == 0) {
        positionCentre = view.getCenter(); // Centrage par rapport à la vue
    } else {
        sf::Vector2f center (950, 300); // Centre de la fenêtre
        positionCentre = center; // Centrage par rapport à la fenêtre
    }

    float scaleFactor = (fin == 1) ? 1.375f : 1.0f;

    // Déclarer scoreBoxSize en dehors des blocs if/else
    sf::Vector2f scoreBoxSize;

    // Définir la taille de la boîte centrale en fonction de la valeur de 'fin'
    if (fin == 0) {
        scoreBoxSize = sf::Vector2f(800, 400); // Taille pour fin == 0
    } else {
        scoreBoxSize = sf::Vector2f(1100, 550); // Taille pour fin != 0
    }
    sf::RectangleShape scoreBox(scoreBoxSize);
    scoreBox.setFillColor(sf::Color(0, 0, 0, 150)); 
    scoreBox.setOutlineThickness(3*scaleFactor);
    scoreBox.setOutlineColor(sf::Color::White);
    
    // Centrer la boîte dans la fenêtre
    scoreBox.setPosition(positionCentre.x - scoreBoxSize.x / 2, positionCentre.y - scoreBoxSize.y / 2);
    window->draw(scoreBox);

    // Recalcul dynamique des positions du texte
    static sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Erreur: Impossible de charger la police!" << std::endl;
        return;
    }

    sf::Text title("Tableau des Scores", font, 24*scaleFactor);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(scoreBox.getPosition().x + (scoreBoxSize.x - title.getLocalBounds().width) / 2,
                      scoreBox.getPosition().y + 20);
    window->draw(title);

    float startX = scoreBox.getPosition().x + 20;
    float startY = scoreBox.getPosition().y + 60;
    float columnSpacing = scoreBoxSize.x / 5.0f; // Ajustement dynamique
    float rowSpacing = 40; 
    float currentY = startY;

    // Affichage des en-têtes
    sf::Text headerText;
    headerText.setFont(font);
    headerText.setCharacterSize(22);
    headerText.setFillColor(sf::Color::White);

    std::vector<std::string> headers = {"Joueur", "Obus Tires", "Obus Touches", "Precision", "Degats"};
    for (size_t col = 0; col < headers.size(); col++) {
        headerText.setString(headers[col]);
        headerText.setPosition(startX + col * columnSpacing, currentY);
        window->draw(headerText);
    }

    currentY += rowSpacing;

    // Gestion du mode de jeu (équipe ou mêlée générale)
    if (client.mode == 2) { // Mode par équipe
        std::vector<int> equipe1, equipe2;
        for (int i = 0; i < nbJoueur; i++) {
            if (joueur[i].equipe == 1) {
                equipe1.push_back(i);
            } else {
                equipe2.push_back(i);
            }
        }

        if (!equipe1.empty()) {
            sf::Text equipe1Text("Equipe 1", font, 22);
            equipe1Text.setFillColor(sf::Color::White);
            equipe1Text.setStyle(sf::Text::Bold);
            equipe1Text.setPosition(startX, currentY);
            window->draw(equipe1Text);
            currentY += rowSpacing;
        }
        for (int i : equipe1) afficherStatJoueur(i, startX, columnSpacing, currentY, rowSpacing);

        if (!equipe1.empty() && !equipe2.empty()) {
            currentY += rowSpacing;
        }

        if (!equipe2.empty()) {
            sf::Text equipe2Text("Equipe 2", font, 22);
            equipe2Text.setFillColor(sf::Color::White);
            equipe2Text.setStyle(sf::Text::Bold);
            equipe2Text.setPosition(startX, currentY);
            window->draw(equipe2Text);
            currentY += rowSpacing;
        }
        for (int i : equipe2) afficherStatJoueur(i, startX, columnSpacing, currentY, rowSpacing);

    } else { // Mode mêlée générale
        for (int i = 0; i < nbJoueur; i++) {
            afficherStatJoueur(i, startX, columnSpacing, currentY, rowSpacing);
        }
    }
}

// Fonction auxiliaire pour afficher les stats d'un joueur
void Partie::afficherStatJoueur(int i, float startX, float columnSpacing, float &currentY, float rowSpacing) {
    sf::Text playerText;
    playerText.setFont(font);
    playerText.setCharacterSize(20);
    playerText.setFillColor(sf::Color::White);

    // Convertir les valeurs en string
    std::ostringstream stat1Stream, stat2Stream, stat3Stream;
    stat1Stream << std::fixed << std::setprecision(0) << stat[i][1];
    stat2Stream << std::fixed << std::setprecision(0) << stat[i][2];
    stat3Stream << std::fixed << std::setprecision(0) << stat[i][3];

    std::string stat1Str = stat1Stream.str();
    std::string stat2Str = stat2Stream.str();
    std::string stat3Str = stat3Stream.str();

    // Calcul de la précision
    float precision = (stat[i][1] > 0) ? (stat[i][2] / stat[i][1]) * 100 : 0;
    std::ostringstream precisionStream;
    precisionStream << std::fixed << std::setprecision(2) << precision;
    std::string precisionStr = precisionStream.str() + "%";

    // Stocker les valeurs dans un vecteur pour affichage
    std::vector<std::string> playerStats = {
        joueur[i].pseudo, stat1Str, stat2Str, precisionStr, stat3Str};

    // Affichage des statistiques du joueur
    for (size_t col = 0; col < playerStats.size(); col++) {
        playerText.setString(playerStats[col]);
        playerText.setPosition(startX + col * columnSpacing, currentY);
        window->draw(playerText);
    }

    currentY += rowSpacing; // Avancer à la ligne suivante
}


//renvoie le numéro du tank
int Partie::selectionTank() {

    window->setMouseCursorVisible(false);

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
    sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
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

    sf::Vector2f mousePos;

    while (window->isOpen()) {
        sf::Event event;
        window->clear(sf::Color::Black);
        window->draw(backgroundSprite);
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return -1;
            }
            if (window->hasFocus()) { //uniquement si on est sur la fenetre 
                mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));   //la mettre dans le repère de la fenetre (je crois)
            }
            for(int i = 0; i < 6; i ++){
                boutons[i].update(mousePos);
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
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

        for (size_t i = 0; i < tanks.size(); ++i) {

            // Définir la taille de la boîte pour chaque tank
            sf::Vector2f scoreBoxSize(305, 455); // Ajuste selon besoin
            sf::RectangleShape scoreBox(scoreBoxSize);
            scoreBox.setFillColor(sf::Color(0, 0, 0, 150));
            scoreBox.setOutlineThickness(3);
            scoreBox.setOutlineColor(sf::Color::White);
        
            // Positionner la boîte en fonction de la position du tank
            int boxX = startX + (i % numColumns) * (tankWidth + paddingX) - 20;
            int boxY = startY + (i / numColumns) * (tankHeight + paddingY + 150) - 20;
            scoreBox.setPosition(boxX - 72, boxY + 25);
        
            window->draw(scoreBox); // Dessiner la boîte
        
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
                statLabel.setPosition(statsStartX + (totalWidth / 2 - textWidth / 2) - 6 * squareSize, statsStartY + j * (squareSize + 5));
        
                window->draw(statLabel);
            }
        }
        

        //affichage souris
        cursorSprite.setPosition(
            static_cast<float>(mousePos.x) - cursorSprite.getGlobalBounds().width / 2,
            static_cast<float>(mousePos.y) - cursorSprite.getGlobalBounds().height / 2
        );

        window->draw(cursorSprite);

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
