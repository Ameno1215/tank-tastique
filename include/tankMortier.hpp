#ifndef TANK_MORTIER_H
#define TANK_MORTIER_H

#include "tank.hpp"

class Tank_mortier : public tank {
    public:
        Tank_mortier();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif