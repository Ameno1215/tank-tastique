# Dossiers
SRC_DIR = src
OBJ_DIR = zobj
BIN_DIR = zbin
INCLUDE_DIR = include
TANK_DIR = ../jeu/classe

# Executables
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# Compilateur et options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Flags SFML
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources pour le client
CLIENT_SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lobby.cpp $(SRC_DIR)/partie.cpp \
                   $(SRC_DIR)/joueur.cpp $(SRC_DIR)/bouton.cpp $(SRC_DIR)/client.cpp \
                   $(TANK_DIR)/tank.cpp

# Fichiers sources pour le serveur
# Fichiers sources pour le serveur
SERVER_SRC_FILES = $(SRC_DIR)/server.cpp $(SRC_DIR)/partie.cpp $(SRC_DIR)/joueur.cpp $(SRC_DIR)/client.cpp $(TANK_DIR)/tank.cpp


# Fichiers objets générés
CLIENT_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%, $(CLIENT_SRC_FILES)))
CLIENT_OBJ_FILES += $(patsubst $(TANK_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(TANK_DIR)/%, $(CLIENT_SRC_FILES)))

SERVER_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(SRC_DIR)/%, $(SERVER_SRC_FILES)))
SERVER_OBJ_FILES += $(patsubst $(TANK_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter $(TANK_DIR)/%, $(SERVER_SRC_FILES)))

# Compilation du client
$(CLIENT_EXEC): $(CLIENT_OBJ_FILES)
	@echo "🔧 Compilation du client..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_FLAGS)

# Compilation du serveur
$(SERVER_EXEC): $(SERVER_OBJ_FILES)
	@echo "🔧 Compilation du serveur..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_FLAGS)

# Compilation des fichiers sources en objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "📄 Compilation de $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TANK_DIR)/%.cpp | $(OBJ_DIR)
	@echo "📄 Compilation de $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Création du dossier obj si non existant
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compilation complète
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Nettoyage
clean:
	@echo "🧹 Suppression des fichiers compilés..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
