#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "control_structures.h"

int is_if(TokenList *tokens, int *index)
{
    int start = *index;

    // 1) Mot-clé if
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || strcmp(tokens->tokens[*index].valeur, "if") != 0)
    {
        return 0;
    }
    (*index)++;

    // 2) Parenthèse ouvrante
    if (*index >= tokens->count || !is_openingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    int tmp = *index;
    printf("token : %s\n", tokens->tokens[tmp].valeur);
    if (!is_expression(tokens, &tmp))
    {
        *index = start;
        return 0;
    }
    printf("token : %s\n", tokens->tokens[tmp].valeur);
    // succès → on avance l’index réel
    *index = tmp;

    // 4) Parenthèse fermante
    if (*index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;

    // 5) Bloc suivant
    if (!is_block(tokens, index))
    {
        *index = start;
        return 0;
    }

    // 6) else / else if
    if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_KEYWORD && strcmp(tokens->tokens[*index].valeur, "else") == 0)
    {
        (*index)++;
        // else if
        if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_KEYWORD && strcmp(tokens->tokens[*index].valeur, "if") == 0)
        {
            if (!is_if(tokens, index))
            {
                *index = start;
                return 0;
            }
        }
        else
        {
            // else suivi d’un bloc
            if (!is_block(tokens, index))
            {
                *index = start;
                return 0;
            }
        }
    }

    return 1;
}

// Grammaire visée :
// for_stmt → "for" "(" [ init ] ";" [ cond ] ";" [ post ] ")" block

int is_for(TokenList *tokens, int *index)
{
    int start = *index;
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 1) Mot-clé « for »
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || strcmp(tokens->tokens[*index].valeur, "for") != 0)
    {
        return 0;
    }
    (*index)++;
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 2) Parenthèse ouvrante
    if (*index >= tokens->count || !is_openingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 3) Segment d'init : declaration | assignment | expression | vide
    {
        int tmp = *index;
        if (is_declaration(tokens, &tmp) || is_assignment(tokens, &tmp) || is_expression(tokens, &tmp))
        {
            // on avance l’index principal seulement si quelque chose a matché
            *index = tmp;
        }
        // sinon, init vide → on ne touche pas *index
    }

    // 4) Premier « ; »
    if (!is_token_pointvirgule(tokens, index))
    {
        *index = start;
        return 0;
    }
    printf("token : %s\n", tokens->tokens[*index].valeur);
    (*index)++;
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 5) Segment de condition : expression | vide
    {
        int tmp = *index;
        if (is_expression(tokens, &tmp))
        {
            *index = tmp;
        }
        // sinon, cond vide → *index reste inchangé
    }

    // 6) Deuxième « ; »
    if (!is_token_pointvirgule(tokens, index))
    {
        *index = start;
        return 0;
    }
    printf("token : %s\n", tokens->tokens[*index].valeur);
    (*index)++;
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 7) Segment post : assignment | expression | vide
    {
        int tmp = *index;
        if (is_assignment(tokens, &tmp) || is_expression(tokens, &tmp))
        {
            *index = tmp;
        }
        // sinon, post vide → *index reste inchangé
    }
    printf("token : %s\n", tokens->tokens[*index].valeur);
    // 8) Parenthèse fermante
    if (*index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;
    printf("token avant bloc : %s\n", tokens->tokens[*index].valeur);
    // 9) Bloc
    if (!is_block(tokens, index))
    {
        *index = start;
        return 0;
    }
    printf("token : %s\n", tokens->tokens[*index].valeur);
    printf("sorite");
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

    if (!is_block(tokens, index))
    {
        *index = start;
        return 0;
    }

    return 1;
}
