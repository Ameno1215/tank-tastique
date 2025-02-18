#ifndef TIR_H
#define TIR_H

#include "../config/config.hpp"


class Obus {
    public :
        Obus() {}
        Obus(int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture);

        //accesseurs
        int get_x();
        int get_y();
        float get_ori();
        float get_vit();
        int get_porte();
        const sf::Texture& get_texture();
        sf::Sprite& get_Sprite();

        
        //setters
        void set_x(int new_x);
        void set_y(int new_y);
        void set_ori(float new_ori);
        void set_vit(float new_vit);
        void set_porte(int new_porte);
        void set_texture(const std::string& nom);

        
    private :
        int x;
        int y;
        float orientation;
        float vitesse;
        int porte;

        sf::Texture texture;
        sf::Sprite spriteObus;

};




#endif