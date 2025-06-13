#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_instruction.h"

ASTNode *parse_declaration(TokenList *tokens, int *index, Analyse_Table *table, int *current_block_index)
{
    int start = *index;

    if (!is_declaration(tokens, index))
    {
        *index = start;
        return NULL;
    }

    ASTNode *decl = new_ATS(NODE_DECLARATION, NULL, NULL,
                            tokens->tokens[start],
                            tokens->tokens[start].ligne);

    int i = start;

    // Table symbole
    SymbolEntry *ID = malloc(sizeof(SymbolEntry));
    if (!ID)
    {
        perror("malloc SymbolEntry");
        exit(EXIT_FAILURE);
    }

    ID->index_block = *current_block_index;
    ID->token = tokens->tokens[i];       // token
    ID->type = tokens->tokens[i].valeur; // type (int, float etc)
    ID->name = tokens->tokens[i++].valeur;
    // type AST
    ASTNode *typeNode = new_ATS(NODE_KEYWORD, NULL, NULL,
                                ID->token, ID->token.ligne);
    add_child(decl, typeNode);

    // Gestion des pointeurs
    int pointer_count = 0;
    while (i < tokens->count &&
           tokens->tokens[i].type == TOKEN_OPERATOR &&
           strcmp(tokens->tokens[i].valeur, "*") == 0)
    {
        pointer_count++;
        i++;
    }

    if (pointer_count > 0)
    {
        ASTNode *pointerNode = new_ATS(NODE_POINTER, NULL, NULL,
                                       tokens->tokens[i - 1],
                                       tokens->tokens[i - 1].ligne);
        pointerNode->pointer_level = pointer_count;
        add_child(decl, pointerNode);
    }

    // Identifiant
    Token idTok = tokens->tokens[i++];
    ID->name = strdup(idTok.valeur);
    ID->suivant = table->tete;
    table->tete = ID;
    table->count++;

    ASTNode *idNode = new_ATS(NODE_IDENTIFIER, NULL, NULL, idTok, idTok.ligne);
    add_child(decl, idNode);

    // Affectation
    if (i < tokens->count &&
        tokens->tokens[i].type == TOKEN_ASSIGNMENT &&
        strcmp(tokens->tokens[i].valeur, "=") == 0)
    {
        i++;
        ASTNode *assignNode = parse_expression(tokens, &i);
        if (assignNode)
        {
            add_child(decl, assignNode);
        }
        else
        {
            fprintf(stderr, "Erreur : affectation invalide après la déclaration\n");
        }
    }

    *index = i;
    print_ast(decl, 0);
    return decl;
}

ASTNode *parse_assignment(TokenList *tokens, int *index)
{
    int start = *index;

    // Vérifie que la séquence est une assignation
    if (!is_assignment(tokens, index))
    {
        *index = start;
        return NULL;
    }
    // Création du nœud racine d'affectation (=)
    Token opTok = tokens->tokens[start + 1]; // token '='
    ASTNode *assign = new_ATS(NODE_ASSIGNMENT, NULL, NULL, opTok, opTok.ligne);

    // Enfant gauche : identifiant
    Token idTok = tokens->tokens[start];
    ASTNode *idNode = new_ATS(NODE_IDENTIFIER, NULL, NULL, idTok, idTok.ligne);
    add_child(assign, idNode);

    // Enfant droit : expression (après '=')
    int exprIndex = start + 2;
    ASTNode *expressionNode = parse_expression(tokens, &exprIndex);
    if (!expressionNode)
    {
        fprintf(stderr, "Erreur interne : impossible de parser l'expression d'affectation.\n");
        free_AST(idNode);
        free_AST(assign);
        *index = start;
        return NULL;
    }
    add_child(assign, expressionNode);

    *index = exprIndex;
    return assign;
}

ASTNode *parse_instruction(TokenList *tokens, int *index, Analyse_Table *table, int *current_block_index)
{
    int start = *index;
    ASTNode *node = NULL;

    // 1) Déclaration ;
    print_ast(node, 0); // Debug : affiche l'AST de l'expression
    node = parse_declaration(tokens, index, table, current_block_index);
    if (node != NULL)
    {
        if (*index < tokens->count && is_token_pointvirgule(tokens, index))
        {
            (*index)++; // consomme ';'
            print_ast(node, 0);
            return node;
        }
        free_AST(node);
        *index = start;
        return NULL;
    }
    *index = start;

    // 2) Affectation ;
    node = parse_assignment(tokens, index);
    if (node != NULL)
    {
        if (*index < tokens->count && is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            print_ast(node, 0);
            return node;
        }
        free_AST(node);
        *index = start;
        return NULL;
    }
    *index = start;

    // 3) Expression (appelée “expression statement”) ;
    node = parse_expression(tokens, index);
    if (node != NULL)
    {
        if (*index < tokens->count && is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            print_ast(node, 0);
            return node;
        }
        free_AST(node);
        *index = start;
        return NULL;
    }
    *index = start;

    // 4) if / for / while (pas de ;)
    node = parse_if(tokens, index, table, current_block_index);
    if (node != NULL)
    {
        print_ast(node, 0);
        return node;
    }
    *index = start;

    node = parse_for(tokens, index, table, current_block_index);
    if (node != NULL)
    {
        print_ast(node, 0);
        return node;
    }
    *index = start;

    node = parse_while(tokens, index, table, current_block_index);
    if (node != NULL)
    {
        print_ast(node, 0);
        return node;
    }
    *index = start;

    // 5) bloc seul (cas très rare : traitement direct d'un {...} comme instruction)
    node = parse_block(tokens, index, table, current_block_index);
    if (node != NULL)
    {
        print_ast(node, 0);
        return node;
    }
    *index = start;

    // rien reconnu
    return NULL;
}

// parse_block : construit un AST pour { instr* }
ASTNode *parse_block(TokenList *tokens, int *index, Analyse_Table *table, int *current_block_index)
{
    int start = *index;

    // is_block avance *index à la fin du bloc (après '}')
    if (!is_block(tokens, index))
    {
        *index = start; // réinitialiser si pas un bloc
        return NULL;
    }
    (*current_block_index)++; // met à jour l'index des blocs

    // Création du noeud BLOCK avec le token '{' à start
    Token braceTok = tokens->tokens[start];
    ASTNode *blockNode = new_ATS(NODE_BLOCK, NULL, NULL, braceTok, braceTok.ligne);

    // Parcours du bloc avec un index temporaire pos
    int pos = start + 1;     // après '{'
    while (pos < *index - 1) // *index pointe après '}', donc -1 est le '}'
    {
        ASTNode *inst = parse_instruction(tokens, &pos, table, current_block_index);
        if (!inst)
        {
            fprintf(stderr, "Erreur : instruction invalide dans bloc à la ligne %d\n", tokens->tokens[pos].ligne);
            free_AST(blockNode);
            *index = start;
            return NULL;
        }
        add_child(blockNode, inst);
    }

    // *index est déjà avancé par is_block, donc on le laisse

    (*current_block_index)--; // met à jour l'index des blocs
    return blockNode;
}
