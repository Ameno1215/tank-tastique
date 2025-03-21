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
        std::string getLabel();
        void setOutlineColor(sf::Color color);
        void setOutlineThickness(float thickness);
        void setOutline(sf::Color color, float thickness);

    private:
        sf::Text text;
        sf::Vector2f originalSize;
        sf::RectangleShape shape;
        bool isHovered = false;
        sf::Color outlineColor;  // La couleur du contour
        float outlineThickness = 0.0f;  // L'épaisseur du contour, par défaut pas de contour (0)
    };

    #endif
