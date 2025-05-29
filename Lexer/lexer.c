#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

void add_token(TokenList *list, Token token)
{
    Token *new_tokens = realloc(list->tokens, (list->count + 1) * sizeof(Token));
    if (new_tokens == NULL)
    {
        // Gestion d'erreur simple
        fprintf(stderr, "Erreur d'allocation mémoire pour les tokens\n");
        exit(EXIT_FAILURE);
    }
    list->tokens = new_tokens;
    list->tokens[list->count] = token;
    list->count++;
}

void free_token_list(TokenList *list)
{
    for (int i = 0; i < list->count; ++i)
    {
        free(list->tokens[i].valeur);
    }
    free(list->tokens);
    list->tokens = NULL;
    list->count = 0;
}

char *reset_word(char word[100])
{
    for (int i = 0; i < 100; i++)
    {
        word[i] = '\0'; // Initialise chaque caractère à '\0'
    }
}

void doubleOperator(char op1, char op2, char word[100], int n, TokenList *list)
{
    word[0] = op1;
    word[1] = op2;
    word[2] = '\0';
    Token token_o; // crée le token opérateur
    token_o.type = TOKEN_OPERATOR;
    token_o.ligne = n;
    token_o.valeur = malloc(strlen(word) + 1);
    strcpy(token_o.valeur, word);
    printf("TOKEN_OPERATOR: \"%s\"\n", token_o.valeur);
    add_token(list, token_o);
}

void assigment(char op1, char op2, char word[100], int n, TokenList *list)
{
    word[0] = op1;
    word[1] = op2;
    word[2] = '\0';
    Token token_o; // crée le token opérateur
    token_o.type = TOKEN_ASSIGNMENT;
    token_o.ligne = n;
    token_o.valeur = malloc(strlen(word) + 1);
    strcpy(token_o.valeur, word);
    printf("TOKEN_ASSIGNMENT: \"%s\"\n", token_o.valeur);
    add_token(list, token_o);
}

void lexer(char *file, TokenList *list)
{
    char *buffer = file;
    char word[100];
    int index_word = 0;
    int index_buffer = 0;
    int specifique = 0;
    int wordInCreation = 0;
    int numero_ligne = 1;

    while (buffer[index_buffer] != '\0') // Tant que on est pas à la fin du fichier
    {
        while (buffer[index_buffer] == ' ' || buffer[index_buffer] == '\n' || buffer[index_buffer] == '\t')
        {
            index_buffer++;
            if (buffer[index_buffer] == '\n')
            {
                numero_ligne++; // on saute le */
            }
        }

        while (buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '/') // Gestion des commentaires
        {
            index_buffer += 2;                                                   // on saute le //
            while (buffer[index_buffer] != '\0' && buffer[index_buffer] != '\n') // lit toute la ligne
            {
                index_buffer++;
            }

            if (buffer[index_buffer] == '\n')
            {
                numero_ligne++;
            }
        }
        while (buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '*') // Gestion des commentaires
        {
            index_buffer += 2;                                                                                     // saut le /*                                                                                   // on saute le /*
            while (buffer[index_buffer] != '\0' && buffer[index_buffer] != '*' && buffer[index_buffer + 1] != '/') // lit toute la chaine
            {
                index_buffer++;
                if (buffer[index_buffer] == '\n')
                {
                    numero_ligne++; // on saute le */
                }
            }
            if (buffer[index_buffer] != '\0')
            {
                index_buffer += 2; // on saute le */
            }
        }

        index_word = 0;
        reset_word(word); // Réinitialiser le mot
        while (buffer[index_buffer] != '\0' && !find_separator(buffer[index_buffer]))
        {
            specifique = 0;
            if (word[0] != '\0')
            {
                wordInCreation = 1;
            }
            else
            {
                wordInCreation = 0;
            }
            // Si ca ressmeble a un cas spécifique
            if (buffer[index_buffer] == '"') // Gestion des String pour avoir "un seul token
            {
                index_buffer++;
                index_word = 0;

                while (buffer[index_buffer] != '"' && buffer[index_buffer] != '\0') // lit toute la chaine
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;
                }

                if (buffer[index_buffer] == '"')
                {
                    word[index_word] = '\0';
                    Token token_s; // crée le token string
                    token_s.type = TOKEN_STRING;
                    token_s.ligne = numero_ligne;
                    token_s.valeur = malloc(strlen(word) + 1);
                    strcpy(token_s.valeur, word);
                    printf("TOKEN_STRING: \"%s\"\n", token_s.valeur);
                    add_token(list, token_s);
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                    specifique = 1;
                }
                else
                {
                    printf("Erreur: Chaîne de caractères non terminée\n");
                    return;
                }
            }
            else if (buffer[index_buffer] == '\'') // Gestion des caractères pour avoir 'un seul token'
            {
                index_buffer++;
                index_word = 0;

                while (buffer[index_buffer] != '\'' && buffer[index_buffer] != '\0') // lit toute la chaine
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;
                }

                if (buffer[index_buffer] == '\'')
                {
                    word[index_word] = '\0';
                    index_buffer++;

                    Token token_ch; // crée le token caractère
                    token_ch.type = TOKEN_CHAR;
                    token_ch.ligne = numero_ligne;
                    token_ch.valeur = malloc(strlen(word) + 1);
                    strcpy(token_ch.valeur, word);
                    printf("TOKEN_CHAR: \"%s\"\n", token_ch.valeur);
                    add_token(list, token_ch);
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                    specifique = 1;
                }
                else
                {
                    printf("Erreur: Caractère non terminé\n");
                    return;
                }
            }
            else if ((wordInCreation == 0) && (buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9')) // Gestion des nombres (entiers et flottants)
            {
                index_word = 0;
                while (buffer[index_buffer] != '\0' && buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // Gestion des entiers
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;
                }
                if (buffer[index_buffer] == '.') // Gestion des floats si c'est le cas
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;

                    if (buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // Vérifie si le caractere est bien un chiffre sinon err
                    {
                        while (buffer[index_buffer] != '\0' && buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9')
                        {
                            word[index_word] = buffer[index_buffer];
                            index_word++;
                            index_buffer++;
                        }
                    }
                    else
                    {
                        printf("Erreur: Flottant malformé\n");
                        return;
                    }
                }

                if (buffer[index_buffer] == 'e' || buffer[index_buffer] == 'E') // Gestion nombre scientifique
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;

                    if (buffer[index_buffer] == '+' || buffer[index_buffer] == '-') // Gestion du signe de l'exposant
                    {
                        word[index_word] = buffer[index_buffer];
                        index_word++;
                        index_buffer++;
                    }

                    if (buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // lit l'exposant
                    {
                        while (buffer[index_buffer] != '\0' && buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9')
                        {
                            word[index_word] = buffer[index_buffer];
                            index_word++;
                            index_buffer++;
                        }
                        if (buffer[index_buffer] == '.') // Gestion des exposants floats si c'est le cas
                        {
                            word[index_word] = buffer[index_buffer];
                            index_word++;
                            index_buffer++;

                            if (buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // Vérifie si le caractere est bien un chiffre sinon err
                            {
                                while (buffer[index_buffer] != '\0' && buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9')
                                {
                                    word[index_word] = buffer[index_buffer];
                                    index_word++;
                                    index_buffer++;
                                }
                            }
                            else
                            {
                                printf("Erreur: Exposant Flottant malformé\n");
                                return;
                            }
                        }
                        else
                        {
                            printf("Erreur: Exposant malformé après 'e' ou 'E'\n");
                            return;
                        }
                    }
                }
                printf("char d'apres le mot: %c\n", buffer[index_buffer]);
                word[index_word] = '\0'; // Terminer le mot
                Token token_n;           // Créer le token nombre
                token_n.type = TOKEN_NUMBER;
                token_n.ligne = numero_ligne;
                token_n.valeur = malloc(strlen(word) + 1);
                strcpy(token_n.valeur, word);
                printf("TOKEN_NUMBER: \"%s\"\n", token_n.valeur);
                add_token(list, token_n);
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
                specifique = 1;
            }
            if (find_puntuation(buffer[index_buffer]) != -1)
            {
                char temp[2];
                temp[0] = buffer[index_buffer];
                temp[1] = '\0';
                Token token_p;
                token_p.type = TOKEN_PUNCTUATION;
                token_p.ligne = numero_ligne;
                token_p.valeur = malloc(strlen(temp) + 1);
                strcpy(token_p.valeur, temp);
                printf("TOKEN_PUNCTUATION: \"%s\"\n", token_p.valeur);
                add_token(list, token_p);
            }
            if (specifique == 0)
            {
                word[index_word] = buffer[index_buffer];
                word[index_word + 1] = '\0'; // Terminer le mot
                index_word++;

                if (find_key_word(word) != -1)
                {
                    Token token_k;
                    token_k.type = TOKEN_KEYWORD;
                    token_k.ligne = numero_ligne;
                    token_k.valeur = malloc(strlen(word) + 1);
                    strcpy(token_k.valeur, word);
                    printf("TOKEN_KEYWORD: \"%s\"\n", token_k.valeur);
                    add_token(list, token_k);
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                }
                else if (buffer[index_buffer] == '=' || find_duo_assigment(buffer[index_buffer], buffer[index_buffer + 1]) != -1) // Gestion de l'égalité
                {
                    if (buffer[index_buffer] == '=')
                    {
                        Token token_a;
                        token_a.type = TOKEN_ASSIGNMENT; // type spécial
                        token_a.ligne = numero_ligne;
                        token_a.valeur = malloc(2);
                        strncpy(token_a.valeur, "=", 2);
                        printf("TOKEN_ASSIGNMENT: \"%s\"\n", token_a.valeur);
                        add_token(list, token_a);
                        index_word = 0;
                        reset_word(word);
                        specifique = 1;
                    }
                    else
                    {

                        assigment(buffer[index_buffer], buffer[index_buffer + 1], word, numero_ligne, list);
                        index_buffer++;
                        index_word = 0;
                        reset_word(word); // Réinitialiser le mot
                        specifique = 1;
                    }
                }
                else if (find_duo_operator(buffer[index_buffer], buffer[index_buffer + 1]) != -1) // Gestion des opérateurs ++ et --
                {
                    doubleOperator(buffer[index_buffer], buffer[index_buffer + 1], word, numero_ligne, list);
                    index_buffer++;
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                    specifique = 1;
                }
            }
            index_buffer++;
        }
        if (buffer[index_buffer] == '\n')
            numero_ligne++;

        if (buffer[index_buffer] != '\0')
        {
            if (buffer[index_buffer] == '#') // Gestion des préprocesseurs
            {
                index_word = 0;
                word[index_word] = buffer[index_buffer];
                index_buffer++;
                index_word++;

                while (buffer[index_buffer] != '\0' && buffer[index_buffer] != '\n' && buffer[index_buffer] != ' ') // lit toute la chaine
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;
                }
                word[index_word] = '\0'; // Terminer le mot

                Token token_pp; // crée le token préprocesseur
                token_pp.type = TOKEN_PREPROCESSOR;
                token_pp.ligne = numero_ligne;
                token_pp.valeur = malloc(strlen(word) + 1);
                strcpy(token_pp.valeur, word);
                printf("TOKEN_PREPROCESSOR: \"%s\"\n", token_pp.valeur);
                add_token(list, token_pp);
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
                specifique = 1;
            }
            else if (word[0] != '\0' && word != NULL)
            {
                Token token_i;
                token_i.type = TOKEN_IDENTIFIER;
                token_i.ligne = numero_ligne;
                token_i.valeur = malloc(strlen(word) + 1);
                strcpy(token_i.valeur, word);
                printf("TOKEN_IDENTIFIER: \"%s\"\n", token_i.valeur);
                add_token(list, token_i);
            }

            reset_word(word); // Réinitialiser le mot
            word[0] = buffer[index_buffer];
            word[1] = '\0';                                                              // Terminer le mot
            if (find_duo_operator(buffer[index_buffer], buffer[index_buffer + 1]) != -1) // Gestion des opérateurs ++ et --
            {
                doubleOperator(buffer[index_buffer], buffer[index_buffer + 1], word, numero_ligne, list);
                index_buffer++;
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
                specifique = 1;
            }
            else if (buffer[index_buffer] == '=' || find_duo_assigment(buffer[index_buffer], buffer[index_buffer + 1]) != -1) // Gestion de l'égalité
            {
                if (buffer[index_buffer] == '=')
                {
                    Token token_a;
                    token_a.type = TOKEN_ASSIGNMENT; // type spécial
                    token_a.ligne = numero_ligne;
                    token_a.valeur = malloc(2);
                    strncpy(token_a.valeur, "=", 2);
                    printf("TOKEN_ASSIGNMENT: \"%s\"\n", token_a.valeur);
                    add_token(list, token_a);
                    index_word = 0;
                    reset_word(word);
                    specifique = 1;
                }
                else
                {

                    assigment(buffer[index_buffer], buffer[index_buffer + 1], word, numero_ligne, list);
                    index_buffer++;
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                    specifique = 1;
                }
            }
            else if (find_single_operator(word) != -1)
            {
                Token token_o;
                token_o.type = TOKEN_OPERATOR;
                token_o.ligne = numero_ligne;
                token_o.valeur = malloc(strlen(word) + 1);
                strcpy(token_o.valeur, word);
                printf("TOKEN_OPERATOR: \"%s\"\n", token_o.valeur);
                add_token(list, token_o);
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
            }
            if (find_puntuation(buffer[index_buffer]) != -1)
            {
                char temp[2];
                temp[0] = buffer[index_buffer];
                temp[1] = '\0';
                Token token_p;
                token_p.type = TOKEN_PUNCTUATION;
                token_p.ligne = numero_ligne;
                token_p.valeur = malloc(strlen(temp) + 1);
                strcpy(token_p.valeur, temp);
                printf("TOKEN_PUNCTUATION: \"%s\"\n", token_p.valeur);
                add_token(list, token_p);
            }
            index_buffer++;
        }
    }
}
