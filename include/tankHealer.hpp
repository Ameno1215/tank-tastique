#ifndef TANK_HEALER_H
#define TANK_HEALER_H

#include "tank.hpp"

class Tank_healer : public tank {
    public:
        Tank_healer();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif