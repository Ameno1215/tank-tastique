#ifndef TANK_SOLIDE_H
#define TANK_SOLIDE_H

#include "tank.hpp"

class Tank_solide : public tank {
    public:
        Tank_solide();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif