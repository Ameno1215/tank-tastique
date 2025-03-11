#ifndef TANK_RAPIDE_H
#define TANK_RAPIDE_H

#include "tank.hpp"

class Tank_rapide : public tank {
    public:
        Tank_rapide();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif