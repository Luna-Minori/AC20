EXEC = Main.exe
CC = gcc
CFLAGS = -Wall -Wextra -g -I. -ILexer -IParser -IParser/Parse_ATS -IParser/grammaire_recognizers
RM = rm -f

SRC = Main.c \
    Lexer/lexer.c \
    Parser/parser.c \
    Parser/grammaire_recognizers/control_structures.c \
    Parser/grammaire_recognizers/expression.c \
    Parser/grammaire_recognizers/instruction.c \
    Parser/Parse_ATS/parse_control_structures.c \
    Parser/Parse_ATS/parse_instruction.c \
    Parser/Parse_ATS/parse_expression.c \
    Analyser_Semantic/analyser_semantic.c \
    utils.c

OBJ = $(SRC:.c=.o)

all: $(EXEC)
	@echo "Compilation terminée."
	@./$(EXEC)

run: $(EXEC)
	@./$(EXEC)

debug: $(EXEC)
	gdb ./$(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(EXEC) $(OBJ)
	@echo "Nettoyage terminé."

mrproper: clean
	@echo "Nettoyage complet terminé."
