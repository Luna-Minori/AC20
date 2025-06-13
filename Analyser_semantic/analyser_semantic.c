#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyser_semantic.h"

int Identifier_exists_already(Analyse_Table *T, const char *name, Pile *current)
{
    for (SymbolEntry *e = T->tete; e; e = e->suivant)
    {
        if (strcmp(e->name, name) == 0 && e->index_block == current->index) // on n’utilise plus de pointeur de bloc
            return 1;
    }
    return 0;
}

int symbol_visible(Analyse_Table *T, const char *name, Pile *current)
{
    for (SymbolEntry *e = T->tete; e; e = e->suivant)
    {
        if (strcmp(e->name, name) == 0)
        {
            // on remonte la chaîne des Pile parents
            for (Pile *p = current; p; p = p->parent)
            {
                if (e->index_block == p->index)
                    return 1;
            }
        }
    }
    return 0;
}

int analyser_semantique(ASTNode *node, Analyse_Table *table, Pile *pile, Analyse_Table *verif_table, int *index)
{
    if (!node)
        return 1;

    switch (node->type)
    {
    case NODE_BLOCK:
    {
        // On entre dans un nouveau bloc
        Pile *top_pile = create_block(pile, index);

        ASTNode *child = node->first_child;
        while (child)
        {
            if (!analyser_semantique(child, table, top_pile, verif_table, index))
                return 0;
            child = child->next_sibling;
        }
        break;
    }

    case NODE_DECLARATION:
    {
        ASTNode *typeNode = node->first_child;
        ASTNode *identNode = typeNode->next_sibling;
        ASTNode *valueNode = identNode->next_sibling;

        if (!identNode || identNode->type != NODE_IDENTIFIER)
        {
            fprintf(stderr, "[ERREUR] Déclaration sans identifiant ligne %d\n", node->line);
            return 0;
        }

        if (Identifier_exists_already(verif_table, identNode->token.valeur, pile))
        {
            fprintf(stderr, "[ERREUR] Redéfinition de '%s' ligne %d dans le bloc %d\n",
                    identNode->token.valeur, node->line, pile->index);
            return 0;
        }

        SymbolEntry *ID = malloc(sizeof(SymbolEntry));
        ID->index_block = pile->index;
        ID->token = identNode->token;
        ID->type = typeNode->token.valeur;
        ID->name = identNode->token.valeur;
        ID->suivant = NULL;

        if (!verif_table->tete)
        {
            verif_table->tete = ID;
        }
        else
        {
            SymbolEntry *temp = verif_table->tete;
            while (temp->suivant)
                temp = temp->suivant;
            temp->suivant = ID;
        }
        verif_table->count++;

        if (valueNode && !analyser_semantique(valueNode, table, pile, verif_table, index))
        {
            return 0;
        }

        break;
    }

    case NODE_ASSIGNMENT:
    {
        ASTNode *ident = node->first_child;
        ASTNode *expr = ident->next_sibling;

        if (!symbol_visible(verif_table, ident->token.valeur, pile))
        {
            fprintf(stderr, "[ERREUR] Variable '%s' utilisée sans déclaration ligne %d\n",
                    ident->token.valeur, node->line);
            return 0;
        }

        if (!analyser_semantique(expr, table, pile, verif_table, index))
        {
            return 0;
        }
        break;
    }

    case NODE_IDENTIFIER:
    {
        if (!symbol_visible(verif_table, node->token.valeur, pile))
        {
            fprintf(stderr, "[ERREUR] Identifiant '%s' non déclaré ligne %d\n",
                    node->token.valeur, node->line);
            return 0;
        }
        break;
    }

    default:
    {
        ASTNode *child = node->first_child;
        while (child)
        {
            if (!analyser_semantique(child, table, pile, verif_table, index))
                return 0;
            child = child->next_sibling;
        }
        break;
    }
    }

    return 1;
}
