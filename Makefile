# Nom de l'exécutable
EXEC = Main.exe

# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -g

# Commande pour supprimer des fichiers
RM = rm -f

# Fichiers sources et objets
SRC = Main.c
OBJ = $(SRC:.c=.o)

# Règle par défaut (compile et exécute)
all: $(EXEC)
	@echo "Compilation terminée."
	@./$(EXEC)


# Compilation de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers compilés
clean:
	$(RM) $(EXEC) $(OBJ)
	@echo "Nettoyage terminé."

# Nettoyer tout (exécutable + fichiers objets)
mrproper: clean
	@echo "Nettoyage complet terminé."
