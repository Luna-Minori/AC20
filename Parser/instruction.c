#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expression.h"
#include "instruction.h"
#include "control_structures.h"

int is_assignment(TokenList *tokens, int *index)
{
    int start = *index;

    // IDENTIFIER
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_IDENTIFIER)
        return 0;
    (*index)++;

    // =
    if (*index >= tokens->count || !(tokens->tokens[*index].type == TOKEN_OPERATOR && strcmp(tokens->tokens[*index].valeur, "=") == 0))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // <expression>
    if (!is_expression(tokens, index))
    {
        *index = start;
        return 0;
    }

    return 1;
}

int is_declaration(TokenList *tokens, int *index)
{
    int start = *index;

    // TYPE
    if (*index >= tokens->count || (tokens->tokens[*index].type != TOKEN_KEYWORD &&
                                    is_type(tokens->tokens[*index].valeur) != -1))
        return 0;
    (*index)++;

    // IDENTIFIER
    if (*index >= tokens->count ||
        tokens->tokens[*index].type != TOKEN_IDENTIFIER)
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // = (optionnel)
    if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_OPERATOR &&
        strcmp(tokens->tokens[*index].valeur, "=") == 0)
    {
        (*index)++;
        // <expression>
        if (!is_expression(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    return 1;
}

int is_instruction(TokenList *tokens, int *index)
{
    // Exemple : une instruction peut être une expression suivie d’un ;
    int start = *index;

    if (is_expression(tokens, index))
    {
        //;
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Expression reconnue de %d à %d\n", start, *index - 1);
            return 1;
        }
    }

    if (is_assignment(tokens, index))
    {
        // ;
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Expression reconnue de %d à %d\n", start, *index - 1);
            return 1;
        }
    }
    if (is_declaration(tokens, index))
    {
        // ;
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Expression reconnue de %d à %d\n", start, *index - 1);
            return 1;
        }
    }
    if (is_if(tokens, index))
    {
        printf("Condition if reconnue de %d à %d\n", start, *index - 1);
        return 1;
    }
    if (is_for(tokens, index))
    {
        printf("Condition if reconnue de %d à %d\n", start, *index - 1);
        return 1;
    }
    if (is_while(tokens, index))
    {
        printf("Condition if reconnue de %d à %d\n", start, *index - 1);
        return 1;
    }

    *index = start; // backtrack
    return 0;
}