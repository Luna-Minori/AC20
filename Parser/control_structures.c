#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "control_structures.h"
#include "expression.h"
#include "instruction.h"
#include "../utils.h"

int is_if(TokenList *tokens, int *index)
{
    int start = *index;

    // if
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || strcmp(tokens->tokens[*index].valeur, "if") != 0)
        return 0;
    (*index)++;

    // (
    if (*index >= tokens->count || !is_openingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // expression
    if (!is_expression(tokens, index))
    {
        *index = start;
        return 0;
    }

    // )
    if (*index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // {
    if (*index >= tokens->count || !is_openingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // instructions dans le bloc
    while (*index < tokens->count && !is_closingBrace(tokens->tokens[*index]))
    {
        if (!is_instruction(tokens, index))
        {
            *index = start;
            return 0;
        }
    }

    // }
    if (*index >= tokens->count || !is_closingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // Gestion optionnelle du else
    if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_KEYWORD && strcmp(tokens->tokens[*index].valeur, "else") == 0)
    {
        (*index)++;

        // else peut être suivi d'un if (else if)
        if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_KEYWORD && strcmp(tokens->tokens[*index].valeur, "if") == 0)
        {
            if (!is_if(tokens, index)) // appel récursif pour else if
            {
                *index = start;
                return 0;
            }
        }
        else
        {
            // else suivi d’un bloc
            if (*index >= tokens->count || !is_openingBrace(tokens->tokens[*index]))
            {
                *index = start;
                return 0;
            }
            (*index)++;

            while (*index < tokens->count && !is_closingBrace(tokens->tokens[*index]))
            {
                if (!is_instruction(tokens, index))
                {
                    *index = start;
                    return 0;
                }
            }

            if (*index >= tokens->count || !is_closingBrace(tokens->tokens[*index]))
            {
                *index = start;
                return 0;
            }
            (*index)++;
        }
    }

    return 1;
}

int is_for(TokenList *tokens, int *index)
{
    int start = *index;

    // for
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || strcmp(tokens->tokens[*index].valeur, "for") != 0)
        return 0;
    (*index)++;

    // (
    if (*index >= tokens->count || !is_openingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // 1er segment (initialisation : déclaration, affectation ou expression)
    int pos_backup = *index;
    if (!is_declaration(tokens, index))
    {
        *index = pos_backup;
        if (!is_assignment(tokens, index))
        {
            *index = pos_backup;
            if (!is_expression(tokens, index))
            {
                // segment vide, on ne bouge pas l'index
                *index = pos_backup;
            }
        }
    }
    // ;
    if (!is_token_pointvirgule(tokens, index))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // 2e segment (condition : expression ou vide)
    pos_backup = *index;
    if (!is_expression(tokens, index))
    {
        // segment vide accepté, on remet index à pos_backup
        *index = pos_backup;
    }

    // ;
    if (!is_token_pointvirgule(tokens, index))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // 3e segment (incrément : expression ou vide)
    pos_backup = *index;
    if (!is_expression(tokens, index))
    {
        // segment vide accepté
        *index = pos_backup;
    }

    // )
    if (*index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // {
    if (*index >= tokens->count || !is_openingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // instructions dans le bloc
    while (*index < tokens->count && !is_closingBrace(tokens->tokens[*index]))
    {
        if (!is_instruction(tokens, index))
        {
            *index = start;
            return 0;
        }
    }

    // }
    if (*index >= tokens->count || !is_closingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    return 1;
}

int is_while(TokenList *tokens, int *index)
{
    int start = *index;

    // while
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || strcmp(tokens->tokens[*index].valeur, "while") != 0)
        return 0;
    (*index)++;

    // (
    if (*index >= tokens->count || !is_openingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // expression
    if (!is_expression(tokens, index))
    {
        *index = start;
        return 0;
    }

    // )
    if (*index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // {
    if (*index >= tokens->count || !is_openingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // instructions dans le bloc
    while (*index < tokens->count && !is_closingBrace(tokens->tokens[*index]))
    {
        if (!is_instruction(tokens, index))
        {
            *index = start;
            return 0;
        }
    }

    // }
    if (*index >= tokens->count || !is_closingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    return 1;
}
