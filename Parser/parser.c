#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "instruction.h"

void parser(TokenList *tokens, ASTNode *root, int *current_block_index)
{
    if (!tokens || tokens->count == 0)
    {
        fprintf(stderr, "Erreur : liste de tokens vide\n");
        return;
    }

    if (!root)
    {
        fprintf(stderr, "Erreur : AST root NULL\n");
        return;
    }

    int index = 0;
    while (index < tokens->count)
    {
        int start = index;

        // On essaie de parser une instruction complète (avec point-virgule si nécessaire)
        ASTNode *inst = parse_instruction(tokens, &index, current_block_index);
        if (!inst)
        {
            fprintf(stderr, "Erreur de parsing a l'index %d (token: %s)\n",
                    index, tokens->tokens[index].valeur);
            break;
        }

        // Si l’index n’a pas bougé, on risque une boucle infinie
        if (index == start)
        {
            fprintf(stderr, "Aucun token consomme a l'index %d\n", index);
            break;
        }
        add_child(root, inst);
    }

    print_ast(root, 0);
    //  TODO : free_ast(root);
}
