#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

typedef struct tree
{
    struct tree **tree;
    TokenType type;
    int child_count;
} ParserTree;

int is_pointvirgule(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ";") == 0; }

int is_crochet(Token t) { return t.type == TOKEN_PUNCTUATION && (strcmp(t.valeur, "{") == 0 || strcmp(t.valeur, "}") == 0); }

int is_openingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "(") == 0; }

int is_closingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ")") == 0; }

int is_openingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "{") == 0; }

int is_closingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "}") == 0; }

int is_valide_operande(Token t) { return t.type == TOKEN_IDENTIFIER || t.type == TOKEN_NUMBER || t.type == TOKEN_CHAR || t.type == TOKEN_STRING; }

int is_expression(TokenList *ligne)
{
    if (!ligne || ligne->count == 0)
        return 0;

    int search_operator = 0;
    int parenthesis_balance = 0;

    for (int i = 0; i < ligne->count; ++i)
    {
        Token current = ligne->tokens[i];

        // Fin d'expression explicite
        if (is_pointvirgule(current))
        {
            return parenthesis_balance == 0 && search_operator == 1;
        }

        if (is_openingParenthesis(current))
        {
            parenthesis_balance++;
            continue;
        }

        if (is_closingParenthesis(current))
        {
            parenthesis_balance--;
            if (parenthesis_balance < 0)
                return 0; // trop de ')'
            continue;
        }

        if (!search_operator)
        {
            if (is_valide_operande(current))
            {
                search_operator = 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            int single_op = find_single_operator(current.valeur);
            int duo_op = -1;
            if (current.valeur[1] != '\0')
                duo_op = find_duo_operator(current.valeur[0], current.valeur[1]);

            if (single_op != -1 || duo_op != -1)
            {
                search_operator = 0;
            }
            else
            {
                return 0;
            }
        }
    }

    // Si on arrive ici sans point-virgule mais expression bien formée
    return search_operator == 1 && parenthesis_balance == 0;
}

int is_assignment(TokenList *ligne)
{
    if (ligne == NULL)
        return 0;
    // identifiant, un opérateur d'assignation et une expression
    if (ligne->tokens[0].type == TOKEN_IDENTIFIER && ligne->tokens[1].type == TOKEN_ASSIGNMENT)
    {
        // On vérifie que le token suivant est une expression
        TokenList sub_ligne;
        sub_ligne.tokens = &ligne->tokens[2]; // On ignore les deux premiers tokens
        sub_ligne.count = ligne->count - 2;

        if (sub_ligne.count == 0)
            return 0; // Pas d'expression après l'assignation

        // Vérifier si la sous-ligne est une expression valide
        if (is_expression(&sub_ligne))
            return 1; // C'est une assignation valide
        else
            return 0; // Pas une assignation valide
    }
    return 0; // Pas une assignation valide
}

int is_declaration(TokenList *ligne)
{
    if (ligne == NULL)
        return 0;

    // Vérifier si le premier token est un mot-clé de déclaration
    if (ligne->tokens[0].type == TOKEN_KEYWORD)
    {
        if (is_type(ligne->tokens[0].valeur) != -1)
        {
            TokenList sub_ligne;
            sub_ligne.tokens = &ligne->tokens[1]; // ignore le premier token
            sub_ligne.count = ligne->count - 2;

            if (sub_ligne.count == 0)
                return 0; // Pas d'expression après l'assignation

            if (is_assignment(&sub_ligne))
            {
                return 1; // C'est une déclaration valide
            }
            else
            {
                return 0; // Pas une déclaration valide
            }
        }
    }
    {
        // Vérifier si le deuxième token est un identifiant
        if (ligne->tokens[1].type == TOKEN_IDENTIFIER)
        {
            // On peut ajouter d'autres vérifications pour les déclarations plus complexes
            return 1; // C'est une déclaration valide
        }
    }
    return 0; // Pas une déclaration valide
}

void aff(TokenList *ligne)
{
    if (ligne == NULL || ligne->count == 0)
        return;

    printf("\nLigne: ");
    for (int i = 0; i < ligne->count; ++i)
    {
        printf("%s ", ligne->tokens[i].valeur);
    }
}

int is_conditionIf(TokenList *ligne)
{
    if (ligne == NULL || ligne->count < 5)
        return 0;

    printf("\nAnalyse de la condition if: ");
    aff(ligne);

    int keyword = ligne->tokens[0].type == TOKEN_KEYWORD && strcmp(ligne->tokens[0].valeur, "if") == 0;
    int openingParenthesis = is_openingParenthesis(ligne->tokens[1]);
    int closingParenthesis = is_closingParenthesis(ligne->tokens[ligne->count - 2]);
    int openingBrace = is_openingBrace(ligne->tokens[ligne->count - 1]);

    // Préparer une sous-liste entre les parenthèses
    int sub_count = ligne->count - 4; // if ( ... ) {
    if (sub_count <= 0)
        return 0;

    TokenList sub_ligne;
    sub_ligne.count = sub_count;
    sub_ligne.tokens = malloc(sizeof(Token) * sub_count);

    for (int i = 0; i < sub_count; i++)
    {
        sub_ligne.tokens[i] = ligne->tokens[i + 2]; // tokens entre les parenthèses
    }

    int expression = is_expression(&sub_ligne);
    free(sub_ligne.tokens); // éviter fuite mémoire

    printf("keyword = %d, openingBrace = %d, openingParenthesis = %d, closingParenthesis = %d, expression = %d\n",
           keyword, openingBrace, openingParenthesis, closingParenthesis, expression);

    if (keyword && openingParenthesis && closingParenthesis && openingBrace && expression)
        return 1;

    return 0;
}

int analyse_ligne(TokenList *ligne)
{
    if (ligne == NULL || ligne->count == 0)
        return 1;

    if (is_expression(ligne))
    {
        // traitement de l'expression
        aff(ligne);
        printf("\nExpression valide");
        return 0;
    }
    if (is_assignment(ligne))
    {
        // traitement de l'assignation
        aff(ligne);
        printf("\nAssignation valide");
        return 0;
    }
    if (is_declaration(ligne))
    {
        // traitement de la déclaration
        aff(ligne);
        printf("\nDeclaration valide");
        return 0;
    }
    if (is_conditionIf(ligne))
    {
        // traitement de la condition if
        aff(ligne);
        printf("\nCondition if valide");
        return 0;
    }

    return 1;
}

void parser(TokenList *list)
{
    if (list == NULL || list->count == 0)
        return;

    TokenList *ligne = malloc(sizeof(TokenList));
    ligne->tokens = NULL;
    ligne->count = 0;
    for (int i = 0; i < list->count; ++i)
    {
        Token current = list->tokens[i];
        ligne->tokens = realloc(ligne->tokens, sizeof(Token) * (ligne->count + 1));
        ligne->tokens[ligne->count++] = current;
        // Si c’est un point-virgule, on analyse la ligne
        if (is_pointvirgule(current) || is_crochet(current))
        {
            aff(ligne);
            analyse_ligne(ligne);

            // Réinitialiser la ligne
            free(ligne->tokens);
            ligne->tokens = NULL;
            ligne->count = 0;
        }
    }

    // Nettoyage final
    free(ligne->tokens);
    free(ligne);
}
