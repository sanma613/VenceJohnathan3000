CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Isrc -O2 -Wall -Wextra
BINDIR := bin
SRCDIR := src
OBJDIR := build

# Encuentra todos los archivos .cpp en src/
SRC := $(wildcard $(SRCDIR)/*.cpp)
# Excluye BoopGameMain.cpp de la librería core
CORE_SRC := $(filter-out $(SRCDIR)/BoopGameMain.cpp,$(SRC))
# Objetos de la librería core
CORE_OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SRC))
# Todos los objetos
OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

# Target por defecto
all: dirs boop_game agent_demo

# Crea los directorios necesarios
dirs:
	mkdir -p $(BINDIR) $(OBJDIR)

# Compila los archivos objeto
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compila el ejecutable del juego principal
boop_game: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(BINDIR)/boop_game

# Compila el ejecutable del agent_demo
agent_demo: $(CORE_OBJ) agent_demo.cpp
	$(CXX) $(CXXFLAGS) agent_demo.cpp $(CORE_OBJ) -o $(BINDIR)/agent_demo

# Limpia archivos generados
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all dirs clean boop_game agent_demo
