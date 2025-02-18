# Nom des exécutables
CLIENT_EXEC = client
SERVER_EXEC = server

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Flags pour SFML (utilisés pour le client ET le serveur)
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp joueur.cpp ../jeu/classe/tank.cpp  # Ajout de tank.cpp

# Compilation du client
$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT_EXEC) $(SFML_FLAGS)

# Compilation du serveur (avec SFML)
$(SERVER_EXEC): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER_EXEC) $(SFML_FLAGS)

# Tout compiler
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Nettoyer les fichiers compilés
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)
