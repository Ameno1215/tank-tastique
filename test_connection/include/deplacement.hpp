#ifndef DEPLACEMENT_HPP
#define DEPLACEMENT_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include "tank.hpp"

void deplacement_verticale(tank& mon_tank, float rotation, float speed);
void deplacement_rotation(tank& mon_tank, float* rotation, float indentation);

#endif // DEPLACEMENT_HPP
