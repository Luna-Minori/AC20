#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"

// Prototypes
int is_expression(TokenList *tokens, int *index);
int is_logical_or(TokenList *tokens, int *index);
int is_logical_and(TokenList *tokens, int *index);
int is_comparaison(TokenList *tokens, int *index);
int is_addition(TokenList *tokens, int *index);
int is_multiplication(TokenList *tokens, int *index);
int is_factor(TokenList *tokens, int *index);
int is_literal(TokenList *tokens, int *index);

// Helpers pour reconnaître les opérateurs
int is_valide_operande(Token t)
{
    return t.type == TOKEN_IDENTIFIER || t.type == TOKEN_NUMBER || t.type == TOKEN_CHAR || t.type == TOKEN_STRING;
}

int is_add_op(Token t)
{
    return t.type == TOKEN_OPERATOR && (strcmp(t.valeur, "+") == 0 || strcmp(t.valeur, "-") == 0);
}

int is_mul_op(Token t)
{
    return t.type == TOKEN_OPERATOR && (strcmp(t.valeur, "*") == 0 || strcmp(t.valeur, "/") == 0 || strcmp(t.valeur, "%") == 0);
}

// Racine
int is_expression(TokenList *tokens, int *index)
{
    return is_logical_or(tokens, index);
}

// ||
int is_logical_or(TokenList *tokens, int *index)
{
    int start = *index;
    if (!is_logical_and(tokens, index))
        return 0;
    while (*index < tokens->count && tokens->tokens[*index].type == TOKEN_OPERATOR && strcmp(tokens->tokens[*index].valeur, "||") == 0)
    {
        (*index)++;
        if (!is_logical_and(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

// &&
int is_logical_and(TokenList *tokens, int *index)
{
    int start = *index;
    if (!is_comparaison(tokens, index))
        return 0;

    while (*index < tokens->count && tokens->tokens[*index].type == TOKEN_OPERATOR && strcmp(tokens->tokens[*index].valeur, "&&") == 0)
    {
        (*index)++;
        if (!is_comparaison(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

// Comparaisons == != < <= > >=
int is_comparaison(TokenList *tokens, int *index)
{
    int start = *index;
    if (!is_addition(tokens, index))
        return 0;

    while (*index < tokens->count && tokens->tokens[*index].type == TOKEN_OPERATOR && is_comparaison_op(tokens->tokens[*index].valeur))
    {
        (*index)++;
        if (!is_addition(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

// + et -
int is_addition(TokenList *tokens, int *index)
{
    int start = *index;
    if (!is_multiplication(tokens, index))
        return 0;

    while (*index < tokens->count && is_add_op(tokens->tokens[*index]))
    {
        (*index)++;
        if (!is_multiplication(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

// * / %
int is_multiplication(TokenList *tokens, int *index)
{
    int start = *index;
    if (!is_factor(tokens, index))
        return 0;

    while (*index < tokens->count && is_mul_op(tokens->tokens[*index]))
    {
        (*index)++;
        if (!is_factor(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

// ++ ou -- ou littéral ou parenthèse
int is_factor(TokenList *tokens, int *index)
{
    int start = *index;
    Token t = tokens->tokens[*index];

    // 1) Préfixe ++ ou --
    if (t.type == TOKEN_OPERATOR &&
        (strcmp(t.valeur, "++") == 0 || strcmp(t.valeur, "--") == 0))
    {
        (*index)++;
        // après un ++/-- préfixe, on doit avoir à nouveau un factor
        if (!is_factor(tokens, index))
        {
            *index = start;
            return 0;
        }
        return 1;
    }

    // 2) Parenthèse ouvrante
    if (is_openingParenthesis(t))
    {
        (*index)++;
        if (!is_expression(tokens, index))
        {
            *index = start;
            return 0;
        }
        if (!is_closingParenthesis(tokens->tokens[*index]))
        {
            *index = start;
            return 0;
        }
        (*index)++;
        // après une sous-expr : on peut aussi avoir un ++/-- postfixe
        if (*index < tokens->count &&
            tokens->tokens[*index].type == TOKEN_OPERATOR &&
            (strcmp(tokens->tokens[*index].valeur, "++") == 0 ||
             strcmp(tokens->tokens[*index].valeur, "--") == 0))
        {
            (*index)++;
        }
        return 1;
    }

    // 3) Littéral (identifiant, nombre, etc.) avec postfixe ++/--
    if (is_literal(tokens, index))
    {
        if (*index < tokens->count &&
            tokens->tokens[*index].type == TOKEN_OPERATOR &&
            (strcmp(tokens->tokens[*index].valeur, "++") == 0 ||
             strcmp(tokens->tokens[*index].valeur, "--") == 0))
        {
            (*index)++;
        }
        return 1;
    }

    // si rien ne matche, on backtrack
    *index = start;
    return 0;
}

// identifiant, nombre, char, string
int is_literal(TokenList *tokens, int *index)
{
    Token t = tokens->tokens[*index];
    if (is_valide_operande(t))
    {
        (*index)++;
        return 1;
    }
    return 0;
}
