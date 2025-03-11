#ifndef TANK_CLASSIQUE_H
#define TANK_CLASSIQUE_H

#include "tank.hpp"

class Tank_classique : public tank {
    public:
        Tank_classique();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};


#endif