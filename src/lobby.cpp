#include "lobby.hpp"
#include "bouton.hpp"

int Lobby::choix(){

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Test Connexion UDP", sf::Style::Titlebar | sf::Style::Close);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Image/lobby.png")) {
        std::cerr << "Erreur : Impossible de charger l'image de fond.\n";
    }

    // Créer un sprite avec la texture
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window.getSize().y) / backgroundSprite.getGlobalBounds().height
    );

    // Charger la police
    sf::Font font;
    if (!font.loadFromFile("Image/the-bomb-sound.regular.ttf")) {
        std::cerr << "Impossible de charger la police, le texte ne s'affichera pas.\n";
    }

    // Boutons
    Bouton button1(440, 300, 400, 150, "Solo", font);
    Bouton button2(440, 500, 400, 150, "Multijoueur", font);

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
