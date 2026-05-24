#include "joueur.hpp"
#include "tankBlanc.hpp"
#include "tankHealer.hpp"
#include "tankMortier.hpp"
#include "tankRapide.hpp"
#include "tankSniper.hpp"
#include "tankSolide.hpp"

Joueur::Joueur() {
    this->id = 0;
    this->pseudo = "Anto";
    this->port = SERVER_PORT;
    setTank(std::make_unique<Tank_classique>());
}

void Joueur::recup_TankPos(float x, float y) {
    Tank->set_x(x);
    Tank->set_y(y);
}

std::unique_ptr<tank> Joueur::makeTankByType(int type) {
    switch (type) {
        case 1:
            return std::make_unique<Tank_classique>();
        case 2:
            return std::make_unique<Tank_rapide>();
        case 3:
            return std::make_unique<Tank_healer>();
        case 4:
            return std::make_unique<Tank_mortier>();
        case 5:
            return std::make_unique<Tank_solide>();
        case 6:
            return std::make_unique<Tank_sniper>();
        default:
            return std::make_unique<Tank_blanc>();
    }
}

void Joueur::setTank(std::unique_ptr<tank> newTank) {
    Tank = std::move(newTank); 
}

void Joueur::assignTankByType(int type) {
    setTank(makeTankByType(type));
    pV = Tank->get_vie();
}

void Joueur::resetInputs() {
    Zpressed = false;
    Spressed = false;
    Qpressed = false;
    Dpressed = false;
    Xpressed = false;
    Clicked = false;
    Tabpressed = false;
}

void Joueur::captureLocalInput(const sf::RenderWindow& window, bool alive) {
    resetInputs();
    mousePos = sf::Mouse::getPosition(window);
    worldMousePos = window.mapPixelToCoords(mousePos);

    if (!alive) {
        return;
    }

    Zpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
    Spressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    Qpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
    Dpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    Xpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
    Clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    Tabpressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);
}

network::InputState Joueur::toInputState() const {
    network::InputState input;
    input.playerId = id;
    input.z = Zpressed;
    input.q = Qpressed;
    input.s = Spressed;
    input.d = Dpressed;
    input.x = Xpressed;
    input.mouseX = static_cast<int>(worldMousePos.x);
    input.mouseY = static_cast<int>(worldMousePos.y);
    input.clicked = Clicked;
    return input;
}

void Joueur::applyInputState(const network::InputState& input) {
    Zpressed = input.z;
    Qpressed = input.q;
    Spressed = input.s;
    Dpressed = input.d;
    Xpressed = input.x;
    worldMousePos = sf::Vector2f(static_cast<float>(input.mouseX), static_cast<float>(input.mouseY));
    Clicked = input.clicked;
}

void Joueur::applyTankState(const network::TankState& state) {
    Tank->set_x(state.x);
    Tank->set_y(state.y);
    Tank->set_ori(state.baseRotation);
    Tank->getTourelleSprite().setRotation(state.turretRotation);
}

void Joueur::afficherTypeTank() const {
    if (Tank) {
        LOG_F(DEBUG, "Type de tank : %s", Tank->getType().c_str());
    } else {
        LOG_F(WARNING, "Aucun tank assigne");
    }
}
