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

    // Par défaut, pas de contour
    shape.setOutlineThickness(outlineThickness);  // L'épaisseur du contour est 0 au départ
    shape.setOutlineColor(outlineColor);  // Pas de couleur de contour au départ

}

bool Bouton::isClicked(sf::Vector2f mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        clicked = true;  // Affecte true SEULEMENT si le bouton est bien cliqué
        return true;
    }
    return false;
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

// Méthode pour définir la position du bouton
void Bouton::setPosition(sf::Vector2f newPosition) {
    shape.setPosition(newPosition);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(newPosition.x + shape.getSize().x / 2.0f, newPosition.y + shape.getSize().y / 2.0f);
}

sf::FloatRect Bouton::getLocalBounds() const {
    return shape.getGlobalBounds();
}

std::string Bouton::getLabel(){
    return text.getString();  
}

void Bouton::setOutlineColor(sf::Color color) {
    outlineColor = color;
    shape.setOutlineColor(outlineColor);
}

void Bouton::setOutlineThickness(float thickness) {
    outlineThickness = thickness;
    shape.setOutlineThickness(outlineThickness);
}

void Bouton::setOutline(sf::Color color, float thickness) {
    outlineColor = color;
    outlineThickness = thickness;
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);
}