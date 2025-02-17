#ifndef TANK_H
#define TANK_H

#include "../config/config.hpp"

class tank{
    public :
        int get_x();
        int get_y();
        int get_ori();
        int get_vit();
        
    private :
        int x;
        int y;
        double orientation;
        double vitesse;
}

#endif

