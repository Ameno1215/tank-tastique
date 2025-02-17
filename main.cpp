#include <iostream>
#include <stdio.h>

#include "config/config.hpp"
#include "classe/tank.hpp"

int main(){
    // Créer un objet de la classe tank
    tank mon_tank;

    mon_tank.set_x(10); 
    mon_tank.set_y(8); 
    mon_tank.set_ori(0.5); 
    mon_tank.set_vit(6); 
    
    std::cout << "Position X : " << mon_tank.get_x() << std::endl;
    std::cout << "Position Y : " << mon_tank.get_y() << std::endl;
    std::cout << "Orientation : " << mon_tank.get_ori() << std::endl;
    std::cout << "Vitesse : " << mon_tank.get_vit() << std::endl;

    return 0;
}