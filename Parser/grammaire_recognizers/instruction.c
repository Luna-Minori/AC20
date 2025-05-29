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
    // opérateur d’affectation
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_ASSIGNMENT)
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
    return 1;
}

int is_declaration(TokenList *tokens, int *index)
{
    int start = *index;
    // TYPE
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_KEYWORD || is_type(tokens->tokens[*index].valeur) == -1)
    {
        return 0;
    }
    (*index)++;
    // IDENTIFIER
    if (*index >= tokens->count || tokens->tokens[*index].type != TOKEN_IDENTIFIER)
    {
        *index = start;
        return 0;
    }
    (*index)++;
    // = ou += ou -= etc <expression> (optionnel)
    if (*index < tokens->count && tokens->tokens[*index].type == TOKEN_ASSIGNMENT && strcmp(tokens->tokens[*index].valeur, "=") == 0)
    {
        (*index)++;
        if (!is_expression(tokens, index))
        {
            *index = start;
            return 0;
        }
    }
    printf("sortie declaration valide \n");
    return 1;
}

int is_instruction(TokenList *tokens, int *index)
{
    int start = *index;

    // Déclaration
    if (is_declaration(tokens, index))
    {
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Declaration reconnue [%d..%d] : ", start, *index - 1);
            for (int i = start; i < *index; i++)
                printf("%s ", tokens->tokens[i].valeur);
            printf("\n");
            return 1;
        }
        *index = start;
    }

    // Affectation
    if (is_assignment(tokens, index))
    {
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Assignment reconnue [%d..%d] : ", start, *index - 1);
            for (int i = start; i < *index; i++)
                printf("%s ", tokens->tokens[i].valeur);
            printf("\n");
            return 1;
        }
        *index = start;
    }

    // Expression
    if (is_expression(tokens, index))
    {
        if (is_token_pointvirgule(tokens, index))
        {
            (*index)++;
            printf("Expression reconnue [%d..%d] : ", start, *index - 1);
            for (int i = start; i < *index; i++)
                printf("%s ", tokens->tokens[i].valeur);
            printf("\n");
            return 1;
        }
        *index = start;
    }

    // Structures de contrôle
    if (is_if(tokens, index))
    {
        printf("If reconnue [%d..%d]\n", start, *index - 1);
        return 1;
    }
    if (is_for(tokens, index))
    {
        printf("For reconnue [%d..%d]\n", start, *index - 1);
        return 1;
    }
    if (is_while(tokens, index))
    {
        printf("While reconnue [%d..%d]\n", start, *index - 1);
        return 1;
    }

    *index = start;
    return 0;
}

// Renvoie 1 si on a un bloc { instr* }, avance index au-delà de '}'
int is_block(TokenList *tokens, int *index)
{
    int start = *index;
    // 1) '{'
    printf("token bloc : %s\n", tokens->tokens[*index].valeur);
    if (*index >= tokens->count || !is_openingBrace(tokens->tokens[*index]))
    {
        return 0;
    }
    (*index)++;
    printf("token bloc : %s\n", tokens->tokens[*index].valeur);

    // 2) instructions à l’intérieur
    while (*index < tokens->count && !is_closingBrace(tokens->tokens[*index]))
    {
        printf("token bloc : %s\n", tokens->tokens[*index].valeur);
        if (!is_instruction(tokens, index))
        {
            printf("token bloc erreure : %s\n", tokens->tokens[*index].valeur);
            // si une instruction échoue, on considère que le bloc n'est pas valide
            *index = start;
            return 0;
        }
    }

    // 3) '}'
    if (*index >= tokens->count || !is_closingBrace(tokens->tokens[*index]))
    {
        *index = start;
        return 0;
    }
    (*index)++;
    return 1;
}
