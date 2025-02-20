#ifndef DEPLACEMENT_HPP
#define DEPLACEMENT_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include "classe/tank.hpp"
#include "deplacement.hpp"

void deplacement_verticale(tank& mon_tank, sf::Vector2f& movement, float rotation, sf::Sprite& sprite, float speed);
void deplacement_rotation(tank& mon_tank, sf::Sprite& sprite, float* rotation, float indentation);

#endif // DEPLACEMENT_HPP
