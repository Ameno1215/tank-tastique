#include "lobby.hpp"

void Lobby::config() {
    window.clear();

    window.draw(backgroundSprite);

    //Section selection Nombre Joueur
    sf::Text titre;
    titre.setFont(font);
    titre.setString("Choisi le nombre de joueurs");
    titre.setCharacterSize(50);
    titre.setFillColor(sf::Color::White);

    sf::FloatRect textRect = titre.getLocalBounds();  // Obtenir les dimensions du texte
    titre.setOrigin(textRect.width / 2, textRect.height / 2);  // Définir l'origine du texte au centre
    titre.setPosition(window.getSize().x / 2, 230);  // Centrer le texte horizontalement et positionner verticalement

    // Création des boutons 1 à 6 et centrage
    std::vector<Bouton> boutons;
    float boutonWidth = 80;
    float boutonHeight = 50;
    float padding = 100;  // Espace entre les boutons

    // Calculer la position pour centrer les boutons
    float startX = (window.getSize().x - (boutonWidth * 6 + padding * 5)) / 2;  // Centrage horizontal
    
    for (int i = 1; i <= 6; i++) {
        boutons.emplace_back(startX + (i - 1) * (boutonWidth + padding), 300, boutonWidth, boutonHeight, std::to_string(i), font);
    }

    //Section selection Mode
    sf::Text titre2;
    titre2.setFont(font);
    titre2.setString("Choisi le Mode");
    titre2.setCharacterSize(50);
    titre2.setFillColor(sf::Color::White);

    sf::FloatRect textRect2 = titre2.getLocalBounds();  // Obtenir les dimensions du texte
    titre2.setOrigin(textRect2.width / 2, textRect2.height / 2);  // Définir l'origine du texte au centre
    titre2.setPosition(window.getSize().x / 2, 400);  // Centrer le texte horizontalement et positionner verticalement

    std::vector<Bouton> boutonsMode;
    float boutonWidth2 = 180;
    float boutonHeight2 = 50;
    float padding2 = 150;  // Espace entre les boutons

    // Calculer la position pour centrer les boutons
    float startX2 = (window.getSize().x - (boutonWidth2 * 2 + padding2)) / 2;  // Centrage horizontal
    
    for (int i = 1; i <= 2; i++) {
        std::string label;
        if(i == 1){
            label =  "Melee generale";
        }
        if(i == 2){
            label = "Equipe";
        }
        boutonsMode.emplace_back(startX2 + (i - 1) * (boutonWidth2 + padding2), 470, boutonWidth2, boutonHeight2, label, font);
    }

    // Créer un bouton "Valider"
    Bouton validerButton(window.getSize().x / 2 - boutonWidth / 2, 570, boutonWidth, boutonHeight, "Valider", font);

    int nbJoueur = false;
    int equipe = false; // a implémenter après

    // Boucle d'événements pour la sélection
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Récupérer la position de la souris
            sf::Vector2f mousePos(sf::Mouse::getPosition(window));

            for (auto& bouton : boutons) {
                bouton.update(mousePos);  // Effet de survol

                // Vérifier le clic gauche
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                    if (bouton.isClicked(mousePos)) {

                        // Réinitialiser le contour de tous les autres boutons
                        for (auto& otherButton : boutons) {
                            if (&otherButton != &bouton) {  // Si ce n'est pas le bouton cliqué
                                bouton.clicked = false;
                                otherButton.setOutline(sf::Color::Transparent, 0.0f);  // Pas de contour pour les autres boutons
                            }
                        }

                        nbJoueurChoisi = std::stoi(bouton.getLabel());
                        nbJoueur = true;
                        bouton.setOutline(sf::Color::Green, 4.0f);
                        std::cout << "Nombre de joueurs choisi : " << nbJoueurChoisi << std::endl;
                    }

                }
            }

            for (auto& bouton : boutonsMode) {
                bouton.update(mousePos);  // Effet de survol

                // Vérifier le clic gauche
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                    if (bouton.isClicked(mousePos)) {

                        // Réinitialiser le contour de tous les autres boutons
                        for (auto& otherButton : boutonsMode) {
                            if (&otherButton != &bouton) {  // Si ce n'est pas le bouton cliqué
                                bouton.clicked = false;
                                otherButton.setOutline(sf::Color::Transparent, 0.0f);  // Pas de contour pour les autres boutons
                            }
                        }
                        equipe = true;
                        bouton.setOutline(sf::Color::Green, 4.0f);
                        std::cout << "Mode choisi "<< std::endl;
                    }
                }
            }

            validerButton.update(mousePos);
            if(nbJoueur && equipe){
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (validerButton.isClicked(mousePos)) {
                        std::cout << "Validation effectuée ! Nombre de joueurs : " << nbJoueurChoisi << std::endl;
                        return;  // Ferme la fonction config
                    }
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);  // Dessiner l'image de fond
        
        window.draw(titre);  // Dessiner le titre
        for (auto& bouton : boutons) {
            bouton.draw(window);
        }

        window.draw(titre2);
        for (auto& bouton : boutonsMode) {
            bouton.draw(window);
        }

        if(nbJoueur && equipe){
            validerButton.draw(window);
        }

        window.display();
    }
}

Lobby::Lobby()
    : window(sf::VideoMode(1280, 720), "Test Connexion UDP", sf::Style::Titlebar | sf::Style::Close) {
    loadResources();
}

void Lobby::loadResources() {
    // Charger l'image de fond
    if (!backgroundTexture.loadFromFile("Image/lobby.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window.getSize().y) / backgroundSprite.getGlobalBounds().height
    );

    // Charger la police
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }
}


int Lobby::choix(){
    // Boutons
    Bouton button1(440, 300, 400, 150, "Creer Partie", font);
    Bouton button2(440, 500, 400, 150, "Rejoindre Partie", font);

    sf::Text responseText;
    responseText.setFont(font);
    responseText.setCharacterSize(20);
    responseText.setFillColor(sf::Color::White);
    responseText.setPosition(100, 300);

    sf::Event event;
    int choixUtilisateur = 0;

    while (window.isOpen()) {
        
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            sf::Vector2f mousePos(sf::Mouse::getPosition(window));
            button1.update(mousePos); // effet de grossissement 
            button2.update(mousePos);
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (button1.isClicked(mousePos)) {
                        config();
                        choixUtilisateur = 1;
                    }
                    if (button2.isClicked(mousePos)) {
                        choixUtilisateur = 2;
                    }
                }
            }
            
        }

        window.clear();
        window.draw(backgroundSprite);  // Dessiner l'image de fond
        button1.draw(window);
        button2.draw(window);
        window.display();

        if (choixUtilisateur != 0) {
            window.close();
            return choixUtilisateur;
        }
    }
    return 0; 
}


