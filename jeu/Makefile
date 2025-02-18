# Définition du compilateur et des options
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I./config -I./classe

# Bibliothèques SFML
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Fichiers sources et objets
SRC = main.cpp classe/tank.cpp deplacement.cpp
OBJ = $(SRC:.cpp=.o)

# Nom de l'exécutable
EXEC = sfml-app

# Règle par défaut : compile et lie
all: $(EXEC)

# Compilation des fichiers objets
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Lien des objets pour créer l'exécutable avec SFML (important: SFML en fin de ligne)
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Phases de compilation
.PHONY: all clean
