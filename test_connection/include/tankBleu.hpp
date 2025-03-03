#ifndef TANK_BLEU_H
#define TANK_BLEU_H

#include "tank.hpp"

class Tank_bleu : public tank {
    public:
        Tank_bleu();
        void setTexture() override;
        std::string getType() const override;
        int get_type() const override;
};



#endif