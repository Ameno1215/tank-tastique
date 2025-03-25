# Dossiers
SRC_DIR = src
OBJ_DIR = zobj
BIN_DIR = zbin
INCLUDE_DIR = include

# Executables
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# Compilateur et options
CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Flags SFML (ajustez si nécessaire)
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

# Fichiers sources pour le client
CLIENT_SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lobby.cpp $(SRC_DIR)/partie.cpp \
                   $(SRC_DIR)/joueur.cpp $(SRC_DIR)/bouton.cpp $(SRC_DIR)/deplacement.cpp $(SRC_DIR)/client.cpp \
                   $(SRC_DIR)/tank.cpp $(SRC_DIR)/tankClassique.cpp $(SRC_DIR)/tankBleu.cpp $(SRC_DIR)/tankBlanc.cpp $(SRC_DIR)/tir.cpp \
				   $(SRC_DIR)/tankRapide.cpp $(SRC_DIR)/tankHealer.cpp $(SRC_DIR)/tankMortier.cpp $(SRC_DIR)/tankSolide.cpp $(SRC_DIR)/tankSniper.cpp     

# Fichiers sources pour le serveur
SERVER_SRC_FILES = $(SRC_DIR)/server.cpp $(SRC_DIR)/partie.cpp $(SRC_DIR)/joueur.cpp $(SRC_DIR)/client.cpp $(SRC_DIR)/deplacement.cpp $(SRC_DIR)/tank.cpp $(SRC_DIR)/bouton.cpp $(SRC_DIR)/tankClassique.cpp $(SRC_DIR)/tankBleu.cpp $(SRC_DIR)/tankBlanc.cpp $(SRC_DIR)/tir.cpp \
				   $(SRC_DIR)/tankRapide.cpp $(SRC_DIR)/tankHealer.cpp $(SRC_DIR)/tankMortier.cpp $(SRC_DIR)/tankSolide.cpp $(SRC_DIR)/tankSniper.cpp      

# Fichiers objets générés pour le client
CLIENT_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%, $(CLIENT_SRC_FILES)))

# Fichiers objets générés pour le serveur
SERVER_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%, $(SERVER_SRC_FILES)))

# Compilation du client
$(CLIENT_EXEC): $(CLIENT_OBJ_FILES)
	@echo "Compilation du client..."
	@mkdir -p $(BIN_DIR)  # Crée le répertoire pour l'exécutable si nécessaire
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_FLAGS)

# Compilation du serveur
$(SERVER_EXEC): $(SERVER_OBJ_FILES)
	@echo "Compilation du serveur..."
	@mkdir -p $(BIN_DIR)  # Crée le répertoire pour l'exécutable si nécessaire
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_FLAGS)

# Compilation des fichiers sources en objets pour le client
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compilation de $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Création du dossier obj si non existant
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compilation complète
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Nettoyage des fichiers générés
clean:
	@echo "Suppression des fichiers compilés..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
