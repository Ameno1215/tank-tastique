
#include "tir.hpp"



Obus::Obus(int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture) 
    : orientation(orientation), vitesse(vitesse), porte(porte) 
{
    set_texture(nomTexture);  // Charge la texture
    spriteObus.setTexture(texture);  // Applique la texture au sprite
    spriteObus.setPosition(x, y);  // Définit la position du sprite
    spriteObus.setScale(0.06f, 0.06f);
    set_status(0);
}

//accesseurs
sf::Vector2i Obus::get_position_tir() const{
    return position_tir;
}

float Obus::get_orientation() const {
    return orientation;
}

float Obus::get_vitesse() const {
    return vitesse;
}

int Obus::get_porte() const {
    return porte;
}

const sf::Texture& Obus::get_texture() const {
    return texture;
}

sf::Sprite& Obus::get_Sprite() {
    return spriteObus;
}

int Obus::get_status() const {
    return status;
}

double Obus::get_time_tir() const {
    return time_tir;
}



// Setters
void Obus::set_position_tir(int new_x, int new_y) {
    position_tir.x = new_x ;
    position_tir.y = new_y;
}

void Obus::set_orientation(float new_ori) {
    orientation = new_ori;  // Modifie l'orientation du tank
}

void Obus::set_vitesse(float new_vit) {
    vitesse = new_vit;  // Modifie la vitesse du tank
}

void Obus::set_porte(int new_porte) {
    porte = new_porte;
}

void Obus::set_texture(const std::string& nom) {
    if (!texture.loadFromFile(nom)) {
        std::cerr << "Erreur : Impossible de charger la texture " << nom << std::endl;
    }
}

void Obus::set_status(int new_status) {
    status = new_status;
}

void Obus::set_time_tir(double new_time) {
    time_tir = new_time;
}



double get_time_seconds() {
    return std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}




void Obus::initTir(float rotation_tourelle, int x_tourelle, int y_tourelle) {
     get_Sprite().setRotation(rotation_tourelle + 180);

    // placement de l'obus au bout du cannon
    get_Sprite().setOrigin(get_Sprite().getLocalBounds().width / 2, get_Sprite().getLocalBounds().height / 2);
    int x_tir = x_tourelle + 70*sin((180 - get_Sprite().getRotation()) * M_PI / 180);
    int y_tir = y_tourelle + 70*cos((180 - get_Sprite().getRotation()) * M_PI / 180);;
    // printf("%lf\n", mon_tank.getTourelleSprite().getRotation());
    set_position_tir(x_tir, y_tir);
    get_Sprite().setPosition(get_position_tir().x, get_position_tir().y);
    set_status(1);
    set_time_tir(get_time_seconds());
    
    // echelle
    get_Sprite().setScale(0.08f, 0.08f);
}






// destructeur
ListeObus::~ListeObus() {
    Noeud* courant = tete;
    while (courant) {
        Noeud* prochainNoeud = courant->suivant;
        delete courant;
        courant = prochainNoeud;
    }
}

 void ListeObus::set_time_dernier_tir(double new_time) {
    time_dernier_tir = new_time;
}

double ListeObus::get_time_dernier_tir() { return time_dernier_tir; }


int ListeObus::ajouterFin(int x, int y, float orientation, float vitesse, int porte, const std::string& nomTexture) {
    Noeud* nouveauNoeud = new Noeud(compteur++, x, y, orientation, vitesse, porte, nomTexture); 
    if (!tete) { // Si la liste est vide
        tete = nouveauNoeud;
        queue = nouveauNoeud;
    } else {
        // Lier le dernier nœud au nouveau noeud
        queue->suivant = nouveauNoeud; 
        queue = nouveauNoeud;  
    }

    return nouveauNoeud->index;
}

void ListeObus::supprimer(int indexASupprimer) {
    Noeud* courant = tete;
    Noeud* precedent = nullptr;

    while (courant) {
        if (courant->index == indexASupprimer) { // Condition d'égalité par index
            if (precedent) {
                precedent->suivant = courant->suivant; // Lier le nœud précédent au suivant
            } else {
                tete = courant->suivant; // Si c'est le premier nœud
            }
            if (courant == queue) {
                queue = precedent; // Si c'est le dernier nœud
            }
            delete courant; // Supprimer le nœud
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }
}


Noeud* ListeObus::trouverNoeud(int index) {
    Noeud* courant = tete;
    while (courant) {
        if (courant->index == index) {
            return courant;
        }
        courant = courant->suivant;
    }
    return nullptr; // Retourne nullptr si aucun nœud avec l'index donné n'est trouvé
}


void ListeObus::afficher() const {
    Noeud* courant = tete;
    while (courant) {
        std::cout << "Obus Index: " << courant->index
                  << " | Position: (" << courant->obus.get_position_tir().x << ", " << courant->obus.get_position_tir().y << ")"
                  << " | Orientation: " << courant->obus.get_orientation()
                  << " | Vitesse: " << courant->obus.get_vitesse()
                  << " | Portée: " << courant->obus.get_porte()
                  << " | Temps: " << courant->obus.get_time_tir()
                  << std::endl;
        courant = courant->suivant;
    }
}

Noeud* ListeObus::get_tete() {
    return tete;
}

