#include "bouton.hpp"

Bouton::Bouton(float x, float y, float width, float height, const std::string& label, sf::Font& font) {
    originalSize = sf::Vector2f(width, height);
    shape.setSize(originalSize);
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color::White);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    
    // Centrer le texte dans le bouton
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(x + width / 2, y + height / 2);
}

bool Bouton::isClicked(sf::Vector2f mousePos) {
    return shape.getGlobalBounds().contains(mousePos);
}

void Bouton::update(sf::Vector2f mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        if (!isHovered) {
            shape.setSize(originalSize * 1.2f);
            shape.setPosition(shape.getPosition().x - (originalSize.x * 0.1f), shape.getPosition().y - (originalSize.y * 0.1f));
            isHovered = true;
        }
    } else {
        if (isHovered) {
            shape.setSize(originalSize);
            shape.setPosition(shape.getPosition().x + (originalSize.x * 0.1f), shape.getPosition().y + (originalSize.y * 0.1f));
            isHovered = false;
        }
    }
}

void Bouton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}
