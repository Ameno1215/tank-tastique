#ifndef TANK_VERT_H
#define TANK_VERT_H

#include "tank.hpp"

class Tank_vert : public tank {
    public:
        Tank_vert();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif