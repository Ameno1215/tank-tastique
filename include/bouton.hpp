    #ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include <SFML/Graphics.hpp>
    #include <string>

    class Bouton {
    public:
        Bouton() = default; // Ajoute un constructeur par défaut
        Bouton(float x, float y, float width, float height, const std::string& label, sf::Font& font);
        bool clicked = false;
        bool isClicked(sf::Vector2f mousePos);
        void update(sf::Vector2f mousePos);
        void draw(sf::RenderWindow& window);
        void setPosition(sf::Vector2f newPosition);
        sf::FloatRect getLocalBounds() const; 
        
    private:
        sf::Text text;
        sf::Vector2f originalSize;
        sf::RectangleShape shape;
        bool isHovered = false;
    };

    #endif
