#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyser_semantic.h"

/* Vérifie si, dans la table T, un symbole du même nom existe déjà */
int Identifier_exists_already(Analyse_Table *T, const char *name)
{
    for (SymbolEntry *e = T->tete; e; e = e->suivant)
        if (strcmp(e->name, name) == 0)
            return 1;
    return 0;
}

/* Vérifie si un identifiant est visible (présent dans la table locale) */
int symbol_visible(Analyse_Table *T, const char *name)
{
    for (SymbolEntry *e = T->tete; e; e = e->suivant)
        if (strcmp(e->name, name) == 0)
            return 1;
    return 0;
}

/* Copie « shallow » de la table : les SymbolEntry sont dupliqués,
   préservant ainsi les déclarations d’ancêtres dans la copie. */
Analyse_Table *copier_table(Analyse_Table *original)
{
    Analyse_Table *copie = malloc(sizeof(Analyse_Table));
    if (!copie)
        return NULL;

    copie->count = original->count;
    copie->tete = NULL;
    SymbolEntry *prev = NULL;

    for (SymbolEntry *curr = original->tete; curr; curr = curr->suivant)
    {
        SymbolEntry *n = malloc(sizeof(SymbolEntry));
        if (!n)
        {
            free_table(copie);
            return NULL;
        }
        /* Copie champ à champ (shallow) */
        n->token = curr->token;
        n->type = curr->type;
        n->name = curr->name;
        n->index_block = curr->index_block;
        n->suivant = NULL;

        if (!copie->tete)
            copie->tete = n;
        else
            prev->suivant = n;
        prev = n;
    }
    return copie;
}

/* Libère entièrement une table et ses entrées */
void free_table(Analyse_Table *table)
{
    if (!table)
        return;
    SymbolEntry *e = table->tete;
    while (e)
    {
        SymbolEntry *next = e->suivant;
        free(e);
        e = next;
    }
    free(table);
}

int analyser_semantique(ASTNode *node, Analyse_Table *table)
{
    if (!node)
        return 1;

    printf(" type : %d, valeur : %s, ligne : %d\n",
           node->type, node->token.valeur ? node->token.valeur : "NULL", node->line);
    switch (node->type)
    {
    case NODE_BLOCK:
    {
        Analyse_Table *local = copier_table(table);
        if (!local)
        {
            fprintf(stderr, "Erreur allocation table locale\n");
            return 0;
        }
        ASTNode *child = node->first_child;
        while (child)
        {
            if (!analyser_semantique(child, local))
            {
                free_table(local);
                return 0;
            }
            child = child->next_sibling;
        }
        print_symbol_table(local);
        free_table(local);
        break;
    }

    case NODE_DECLARATION:
    {
        ASTNode *typeNode = node->first_child;
        ASTNode *identNode = typeNode->next_sibling;
        ASTNode *valueNode = identNode->next_sibling;

        if (!identNode || identNode->type != NODE_IDENTIFIER)
        {
            fprintf(stderr, "[ERREUR] Déclaration sans identifiant ligne %d\n",
                    node->line);
            return 0;
        }

        /* Vérification de redéfinition dans CE bloc (table locale) */
        if (Identifier_exists_already(table, identNode->token.valeur))
        {
            fprintf(stderr, "[ERREUR] Redéfinition de '%s' ligne %d\n",
                    identNode->token.valeur, node->line);
            return 0;
        }

        /* Création et insertion en tête */
        SymbolEntry *ID = malloc(sizeof(SymbolEntry));
        if (!ID)
        {
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            return 0;
        }
        ID->token = identNode->token;
        ID->type = typeNode->token.valeur;
        ID->name = identNode->token.valeur;
        ID->index_block = -1; // non utilisé ici
        ID->suivant = table->tete;
        table->tete = ID;
        table->count++;

        /* On analyse éventuellement l'initialisation */
        if (valueNode && !analyser_semantique(valueNode, table))
            return 0;
        break;
    }

    case NODE_ASSIGNMENT:
    {
        // Affectations =, +=, -=, ... : même logique
        ASTNode *ident = node->first_child;
        ASTNode *expr = ident->next_sibling;
        if (!symbol_visible(table, ident->token.valeur))
        {
            fprintf(stderr, "Variable '%s' non declaree ligne %d\n",
                    ident->token.valeur, node->line);
            return 0;
        }
        if (!analyser_semantique(expr, table))
            return 0;
        break;
    }

    case NODE_BINARY_EXPR:
    {
        ASTNode *left = node->data.binary.left;
        ASTNode *right = node->data.binary.right;

        if (left && !analyser_semantique(left, table))
            return 0;
        if (right && !analyser_semantique(right, table))
            return 0;
        break;
    }

    case NODE_UNARY_EXPR:
    {
        ASTNode *operand = node->data.unary.operande;
        if (operand && !analyser_semantique(operand, table))
            return 0;
        break;
    }

    case NODE_IF:
    {
        // structure if (cond) bloc [ else bloc ]
        ASTNode *cond = node->first_child;
        ASTNode *then_bloc = cond ? cond->next_sibling : NULL;
        ASTNode *else_bloc = then_bloc ? then_bloc->next_sibling : NULL;

        if (cond && !analyser_semantique(cond, table))
            return 0;
        if (then_bloc && !analyser_semantique(then_bloc, table))
            return 0;
        if (else_bloc && !analyser_semantique(else_bloc, table))
            return 0;
        break;
    }

    case NODE_FOR:
    {
        // for(init; cond; incr) bloc
        ASTNode *init = node->first_child;
        ASTNode *cond = init ? init->next_sibling : NULL;
        ASTNode *incr = cond ? cond->next_sibling : NULL;
        ASTNode *body = incr ? incr->next_sibling : NULL;

        if (init && !analyser_semantique(init, table))
            return 0;
        if (cond && !analyser_semantique(cond, table))
            return 0;
        if (incr && !analyser_semantique(incr, table))
            return 0;
        if (body && !analyser_semantique(body, table))
            return 0;
        break;
    }

    case NODE_WHILE:
    {
        // while(cond) bloc
        ASTNode *cond = node->first_child;
        ASTNode *body = cond ? cond->next_sibling : NULL;

        if (cond && !analyser_semantique(cond, table))
            return 0;
        if (body && !analyser_semantique(body, table))
            return 0;
        break;
    }

    case NODE_IDENTIFIER:
    {
        if (!symbol_visible(table, node->token.valeur))
        {
            fprintf(stderr, "[ERREUR] Identifiant '%s' non déclaré ligne %d\n",
                    node->token.valeur, node->line);
            return 0;
        }
        break;
    }

    case NODE_LITERAL:
    case NODE_POINTER:
    case NODE_KEYWORD:
    default:
    {
        // Pour tous les autres nœuds, on descend simplement
        ASTNode *child = node->first_child;
        while (child)
        {
            if (!analyser_semantique(child, table))
                return 0;
            child = child->next_sibling;
        }
        break;
    }
    }

    return 1;
}
