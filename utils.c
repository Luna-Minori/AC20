#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"

int is_pointvirgule(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ";") == 0; }

int is_crochet(Token t) { return t.type == TOKEN_PUNCTUATION && (strcmp(t.valeur, "{") == 0 || strcmp(t.valeur, "}") == 0); }

int is_openingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "(") == 0; }

int is_closingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ")") == 0; }

int is_openingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "{") == 0; }

int is_closingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "}") == 0; }

int is_token_pointvirgule(TokenList *t, int *index)
{
    if (*index >= t->count)
        return 0;
    return is_pointvirgule(t->tokens[*index]);
}

int taille_liste(const char *liste[])
{
    int taille = 0;
    while (liste[taille] != NULL)
    {
        taille++;
    }
    return taille;
}

char find_separator(char word)
{
    const char separators[] = {
        ' ', '\n', '\t', ';', ',', '(', ')', '{', '}',
        '[', ']', '<', '>', '=', '!', '+', '-', '*', '/',
        '&', '|', '^', '%', '\0'};

    for (int i = 0; separators[i] != '\0'; i++)
    {
        if (separators[i] == word) // Ensure no stray characters
        {
            return separators[i];
        }
    }
    return 0; // Si aucun mot-clé n'est trouvé
}

char find_puntuation(char word)
{
    const char ponctuation[] = {';', ',', '(', ')', '{', '}',
                                '[', ']'};

    for (int i = 0; ponctuation[i] != '\0'; i++)
    {
        if (ponctuation[i] == word) // Ensure no stray characters
        {
            return ponctuation[i];
        }
    }
    return -1; // Si aucun mot-clé n'est trouvé
}

int find_single_operator(char *word)
{
    const char *operators[] = {
        "+", "-", "*", "/", "%", "&", "^", "<", ">", "!", NULL};
    for (int i = 0; i < taille_liste(operators); i++)
    {
        if (strcmp(word, operators[i]) == 0)
        {
            return i;
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int find_duo_operator(char first, char second)
{
    char word[3];
    word[0] = first;
    word[1] = second;
    word[2] = '\0';
    const char *duo_operators[] = {
        "++", "--", "==", "!=", "<=", ">=", "&&", "||",
        "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
        "<<", ">>", "->", NULL};

    for (int i = 0; i < taille_liste(duo_operators); i++)
    {
        if (strcmp(word, duo_operators[i]) == 0)
        {
            return i;
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int find_key_word(char *word)
{
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "default", "do",
        "double", "else", "enum", "float", "for", "if",
        "int", "long", "return", "short", "signed", "sizeof",
        "struct", "switch", "typedef", "unsigned", "void", "while", "main", "free", "malloc", NULL};
    // Parcours du tableau de mots-clés
    for (int i = 0; i < taille_liste(keywords); i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return i; // Retourne l'indice correspondant
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int is_type(char *word)
{
    const char *types[] = {
        "int", "float", "double", "char", "void", NULL};
    for (int i = 0; i < taille_liste(types); i++)
    {
        if (strcmp(word, types[i]) == 0)
        {
            return i; // Le mot est un type
        }
    }
    return -1; // Le mot n'est pas un type
}

int is_wordcondition(char *word)
{
    const char *conditions[] = {
        "if", "else", "switch", NULL};
    for (int i = 0; i < taille_liste(conditions); i++)
    {
        if (strcmp(word, conditions[i]) == 0)
        {
            return i; // Le mot est une condition
        }
    }
    return -1; // Le mot n'est pas une condition
}