# Dossiers
SRC_DIR = src
OBJ_DIR = zobj
BIN_DIR = zbin
INCLUDE_DIR = include
TANK_DIR = ../jeu/classe

# Executable
CLIENT_EXEC = $(BIN_DIR)/client

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Flags SFML
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources
SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lobby.cpp $(SRC_DIR)/partie.cpp \
            $(SRC_DIR)/joueur.cpp $(SRC_DIR)/bouton.cpp $(TANK_DIR)/tank.cpp

# Générer les fichiers .o correspondants
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%, $(SRC_FILES)))
OBJ_FILES += $(patsubst $(TANK_DIR)/%.cpp, $(OBJ_DIR)/tank.o, $(filter $(TANK_DIR)/%, $(SRC_FILES)))

# Compilation du client
$(CLIENT_EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_FLAGS)

# Compilation des fichiers sources en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/tank.o: $(TANK_DIR)/tank.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Création du dossier obj si non existant
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Tout compiler
all: $(CLIENT_EXEC)

# Nettoyage des fichiers objets et exécutables
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
