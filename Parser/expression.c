#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"

int is_expression(TokenList *tokens, int *index);
int is_no_priority_op(TokenList *tokens, int *index);
int is_terme(TokenList *tokens, int *index);
int is_priority_op(TokenList *tokens, int *index);
int is_factor(TokenList *tokens, int *index);
int is_literal(TokenList *tokens, int *index);

int is_valide_operande(Token t) { return t.type == TOKEN_IDENTIFIER || t.type == TOKEN_NUMBER || t.type == TOKEN_CHAR || t.type == TOKEN_STRING; }

int is_add_op(Token t) { return t.type == TOKEN_OPERATOR && (strcmp(t.valeur, "+") == 0 || strcmp(t.valeur, "-") == 0); }

int is_mul_op(Token t) { return t.type == TOKEN_OPERATOR && (strcmp(t.valeur, "*") == 0 || strcmp(t.valeur, "/") == 0 || strcmp(t.valeur, "%") == 0); }

int is_expression(TokenList *tokens, int *index)
{
    if (!is_terme(tokens, index))
        return 0;
    return is_no_priority_op(tokens, index);
}

int is_no_priority_op(TokenList *tokens, int *index)
{
    Token t = tokens->tokens[*index];
    if (is_add_op(t))
    {
        (*index)++;
        if (!is_terme(tokens, index))
            return 0;
        return is_no_priority_op(tokens, index);
    }
    return 1; // vide
}

int is_terme(TokenList *tokens, int *index)
{
    if (!is_factor(tokens, index))
        return 0; // faux
    return is_priority_op(tokens, index);
}

int is_priority_op(TokenList *tokens, int *index)
{
    Token t = tokens->tokens[*index];
    if (is_mul_op(t))
    {
        (*index)++;
        if (!is_factor(tokens, index))
            return 0; // faux
        return is_priority_op(tokens, index);
    }
    return 1; // epsilon
}

int is_factor(TokenList *tokens, int *index)
{
    Token t = tokens->tokens[*index];
    if (is_openingParenthesis(t))
    {
        (*index)++;
        if (!is_expression(tokens, index))
            return 0; // faux
        if (!is_closingParenthesis(tokens->tokens[*index]))
            return 0; // faux
        (*index)++;
        return 1; // vrai
    }
    return is_literal(tokens, index); //
}

int is_literal(TokenList *tokens, int *index)
{
    Token t = tokens->tokens[*index];
    if (is_valide_operande(t))
    {
        (*index)++;
        return 1; // valide
    }
    return 0; // invalide
}