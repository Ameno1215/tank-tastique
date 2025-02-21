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

    mousePos = sf::Mouse::getPosition(*window); //recupération de la position de la souris
   
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }

    // Réinitialiser les entrées clavier
    Zpressed = Spressed = Qpressed = Dpressed = false;

    if (window->hasFocus()) {                   // récupération de touche pressée
        Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    }
}

void Partie::update() {

    tank& mon_tank = joueur[joueur_courant].Tank; 
    cursorSprite.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);
    sf::Vector2f dir = worldMousePos - mon_tank.getTourelleSprite().getPosition();  

    sf::Vector2f movement(0.f, 0.f); 

    float speed = mon_tank.get_vit();
    float rotation = mon_tank.get_ori();
    float vit_canon = mon_tank.get_vit_canon();

    if (Zpressed) {
        deplacement_verticale(mon_tank, movement, rotation, mon_tank.getBaseSprite(), speed);
        deplacement_verticale(mon_tank, movement, rotation, mon_tank.getTourelleSprite(), speed);
    }

    if (Spressed) {
        deplacement_verticale(mon_tank, movement, rotation, mon_tank.getBaseSprite(), -speed);
        deplacement_verticale(mon_tank, movement, rotation, mon_tank.getTourelleSprite(), -speed);
    }

    if (Qpressed)
        deplacement_rotation(mon_tank, mon_tank.getBaseSprite(), &rotation, 0.2);
    
    if (Dpressed)
        deplacement_rotation(mon_tank, mon_tank.getBaseSprite(), &rotation, -0.2);
    
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
}

void Partie::renderWindow() {
    tank& mon_tank = joueur[joueur_courant].Tank;
    window->clear();
    window->draw(mon_tank.getBaseSprite());
    window->draw(mon_tank.getTourelleSprite());
    window->draw(cursorSprite);
    window->display();
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
    Client client;

    std::thread connexionThread(&Client::initconnexion, &client);

    affichageConnexion(client);

    // Arrêter le thread proprement
    if (connexionThread.joinable()) {
        connexionThread.join();
    }

    return 0;
}

void Partie::affichageConnexion(Client& client){
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
