#ifndef TIR_H
#define TIR_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>
#include <iostream>
#include <cmath>
#include <cstring>

#define CADENCE_EXPLO 3

class Obus {
    public :
        Obus() {}
        Obus(int x_origine, int y_origine, float orientation, float vitesse, int porte, const std::string& nomTexture);

        //accesseurs
        sf::Vector2i get_position_tir() const;
        float get_orientation() const;
        float get_vitesse() const;
        int get_porte() const;
        const sf::Texture& get_texture() const;
        sf::Sprite& get_Sprite();
        int get_status() const;
        double get_time_tir() const;
        
        
        //setters
        void set_position_tir(int new_x, int new_y);
        void set_orientation(float new_ori);
        void set_vitesse(float new_vit);
        void set_porte(int new_porte);
        void set_texture(const std::string& nom);
        void set_status(int new_status);
        void set_time_tir(double new_time);

        void initTir(float rotation_tourelle, int x_tourelle, int y_tourelle);

        
    private :
        sf::Vector2i position_tir;
        sf::Vector2i position_actuelle;
        float orientation;
        float vitesse;
        int porte;
        int status; // 1 tiré, 0 pas encore tiré
        double time_tir;

        

        sf::Texture texture;
        sf::Sprite spriteObus;

};


//  noeud pour la liste chainé 
struct Noeud {
    Obus obus;       
    Noeud* suivant;
    int index;

    // constructeur
    Noeud(int idx, int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture) : obus(x, y, orientation, vitesse, porte, nomTexture), suivant(nullptr), index(idx) {} 
};


class ListeObus {
    public:
        // constructeur
        ListeObus() : tete(nullptr), queue(nullptr), compteur(0), time_dernier_tir(0) {}
        // destructeur
        ~ListeObus();

        int ajouterFin(int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture);
        void supprimer(int index);
        Noeud* trouverNoeud(int index);
        void afficher() const;

        void set_time_dernier_tir(double new_time);
        double get_time_dernier_tir();
        Noeud* get_tete();
        void vider();

    private:
        Noeud* tete;
        Noeud* queue;
        int compteur;
        double time_dernier_tir;
};

struct NoeudExplosion {
    int x, y, frameActu;
    NoeudExplosion* suivant;
    sf::Sprite explosionSprite;
    sf::Texture explosionTexture;
    bool big;
    NoeudExplosion(int x, int y, int frame, bool big) : x(x), y(y), frameActu(frame), suivant(nullptr), big(big){}
};

class ListeExplosion {
public:
    // Constructeur
    ListeExplosion() : tete(nullptr), queue(nullptr), compteur(0) {}
    
    // Destructeur
    ~ListeExplosion() { vider(); }
    
    int ajouterFin(int x, int y, int frameActu, bool big);
    
    void supprimer(int index);
    
    NoeudExplosion* trouverNoeud(int index);
    
    void afficher() const;
    
    NoeudExplosion* get_tete();

    void maj();
    void majEnSautantFrame();

    void toCharArray(char buffer[100]);

    void vider();

    bool nouveau;
    int sauterFrame = CADENCE_EXPLO;

private:
    NoeudExplosion* tete;
    NoeudExplosion* queue;
    int compteur;
};

double get_time_seconds();


#endif