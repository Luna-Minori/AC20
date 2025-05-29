#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "instruction.h"

void parser(TokenList *tokens)
{
    if (!tokens || tokens->count == 0)
        return;

    // On crée un nœud racine de type BLOCK pour tout le programme
    ASTNode *root = new_ATS(NODE_BLOCK, NULL, NULL,
                            tokens->tokens[0], tokens->tokens[0].ligne);

    int index = 0;
    while (index < tokens->count)
    {
        int start = index;

        // On essaie de parser une instruction complète (avec point-virgule si nécessaire)
        ASTNode *inst = parse_instruction(tokens, &index);
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
    // TODO : free_ast(root);
}
