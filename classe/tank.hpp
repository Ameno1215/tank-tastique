#ifndef TANK_H
#define TANK_H

#include "../config/config.hpp"

class tank{
    public :

        //accesseurs
        int get_x();
        int get_y();
        double get_ori();
        double get_vit();
        
        //setters
        void set_x(int new_x);
        void set_y(int new_y);
        void set_ori(double new_ori);
        void set_vit(double new_vit);
        
    private :
        int x;
        int y;
        double orientation;
        double vitesse;
};

#endif