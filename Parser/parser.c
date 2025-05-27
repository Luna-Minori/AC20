#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "instruction.h"

typedef struct tree
{
    struct tree **tree;
    TokenType type;
    int child_count;
} ParserTree;

void parser(TokenList *list)
{
    if (list == NULL || list->count == 0)
        return;

    int index = 0;
    while (index < list->count)
    {
        // Exemple : on tente d’analyser une instruction complète (comme une affectation ou appel de fonction)
        int start_index = index;

        if (is_instruction(list, &index))
        {
            printf("Instruction reconnue de %d à %d\n", start_index, index - 1);
            // Ici tu pourrais appeler une fonction pour construire l’AST ou exécuter l’instruction
        }
        else
        {
            printf("Erreur de parsing a l'index :  %d (token: %s)\n", index, list->tokens[index].valeur);
            break;
        }
    }
}