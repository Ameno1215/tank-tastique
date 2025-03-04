#ifndef TANK_BLANC_H
#define TANK_BLANC_H

#include "tank.hpp"

class Tank_blanc : public tank {
    public:
        Tank_blanc();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override; 
};



#endif