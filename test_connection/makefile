# Nom des exécutables
CLIENT_EXEC = client
SERVER_EXEC = server

# Compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Flags pour SFML (nécessaires uniquement pour le client)
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

# Compilation du client avec SFML
$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT_EXEC) $(SFML_FLAGS)

# Compilation du serveur
$(SERVER_EXEC): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER_EXEC)

# Tout compiler
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Nettoyer les fichiers compilés
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)
