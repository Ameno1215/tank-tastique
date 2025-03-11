#ifndef TANK_SNIPER_H
#define TANK_SNIPER_H

#include "tank.hpp"

class Tank_sniper : public tank {
    public:
        Tank_sniper();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif