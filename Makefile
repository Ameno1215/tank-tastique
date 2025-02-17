# Définition du compilateur et des options
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I./config -I./classe

# Fichiers sources et objets
SRC = main.cpp classe/tank.cpp
OBJ = $(SRC:.cpp=.o)

# Nom de l'exécutable
EXEC = tank_game

# Règle par défaut : compile et lie
all: $(EXEC)

# Compilation des fichiers objets
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Lien des objets pour créer l'exécutable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC)

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Phases de compilation
.PHONY: all clean
