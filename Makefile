# Nom de l'exécutable
EXEC = Main.exe

# Compilateur
CC = gcc

# Options de compilation (+ includes)
CFLAGS = -Wall -Wextra -g -I. -ILexer -IParser -IParser/Parse_ATS -IParser/grammaire_recognizers

# Commande pour supprimer des fichiers
RM = rm -f

# Fichiers sources
SRC = Main.c \
	Lexer/lexer.c \
	Parser/parser.c \
	Parser/grammaire_recognizers/control_structures.c \
	Parser/grammaire_recognizers/expression.c \
	Parser/grammaire_recognizers/instruction.c \
	Parser/Parse_ATS/parse_control_structures.c \
	Parser/Parse_ATS/parse_instruction.c \
	Parser/Parse_ATS/parse_expression.c \
	utils.c	

# Fichiers objets
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(EXEC)
	@echo "Compilation terminée."
	@./$(EXEC)

# Compilation de l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	$(RM) $(EXEC) $(OBJ)
	@echo "Nettoyage terminé."

mrproper: clean
	@echo "Nettoyage complet terminé."
