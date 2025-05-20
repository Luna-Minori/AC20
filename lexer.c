#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TOKEN_KEYWORD,      // Mot-clé comme "if", "int", etc.
    TOKEN_IDENTIFIER,   // Identificateur (nom de variable, fonction)
    TOKEN_OPERATOR,     // Opérateurs comme +, -, *, /, etc.
    TOKEN_NUMBER,       // Nombre entier ou flottant
    TOKEN_STRING,       // Chaîne de caractères
    TOKEN_CHAR,         // Caractère
    TOKEN_PREPROCESSOR, // Directives de préprocesseur comme #include, #define
    TOKEN_PUNCTUATION,  // Ponctuation comme (, ), {, }, ;, etc.
    TOKEN_END           // Fin de fichier
} TokenType;

typedef struct
{
    TokenType type;
    int index;
    int ligne;
    char *valeur;
} Token;

typedef struct T
{
    TokenType type;
    char *value;
    struct T *lbranches;
} Tree;

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
    return 0; // Si aucun mot-clé n'est trouvé
}

int find_operator(char *word)
{
    const char *operators[] = {
        "+", "-", "*", "/", "%", "++", "--", "=", "+=", "-=", "*=", "/=", "%=",
        "&", "^", "<", ">", "<=", ">=", "==", "!=",
        "&&", "||", "!", ",", ":", "->", NULL};

    for (int i = 0; i < taille_liste(operators); i++)
    {
        if (strcmp(word, operators[i]) == 0)
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
        "struct", "switch", "typedef", "unsigned", "void", "while", "main", NULL};
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

char *reset_word(char word[100])
{
    for (int i = 0; i < 100; i++)
    {
        word[i] = '\0'; // Initialise chaque caractère à '\0'
    }
}

void analyser_lexical(char *file)
{
    char *buffer = file;
    char word[100];
    int index_word = 0;
    int index_buffer = 0;
    int specifique = 0;
    int wordInCreation = 0;

    while (buffer[index_buffer] != '\0') // Tant que on est pas à la fin du fichier
    {
        while (buffer[index_buffer] == ' ' || buffer[index_buffer] == '\n' || buffer[index_buffer] == '\t')
        {
            index_buffer++;
        }

        while (buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '/') // Gestion des commentaires
        {
            index_buffer += 2;                                                   // on saute le //
            while (buffer[index_buffer] != '\0' && buffer[index_buffer] != '\n') // lit toute la ligne
            {
                index_buffer++;
            }
        }
        while (buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '*') // Gestion des commentaires
        {
            index_buffer += 2;                                                                                     // saut le /*                                                                                   // on saute le /*
            while (buffer[index_buffer] != '\0' && buffer[index_buffer] != '*' && buffer[index_buffer + 1] != '/') // lit toute la chaine
            {
                index_buffer++;
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
                    index_buffer++;

                    Token token_s; // crée le token string
                    token_s.type = TOKEN_STRING;
                    token_s.valeur = malloc(strlen(word) + 1);
                    strcpy(token_s.valeur, word);
                    printf("TOKEN_STRING: \"%s\"\n", token_s.valeur);
                    free(token_s.valeur);
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
                    token_ch.valeur = malloc(strlen(word) + 1);
                    strcpy(token_ch.valeur, word);
                    printf("TOKEN_CHAR: \"%s\"\n", token_ch.valeur);
                    free(token_ch.valeur);
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
                word[index_word] = '\0'; // Terminer le mot
                Token token_n;           // Créer le token nombre
                token_n.type = TOKEN_NUMBER;
                token_n.valeur = malloc(strlen(word) + 1);
                strcpy(token_n.valeur, word);
                printf("TOKEN_NUMBER: \"%s\"\n", token_n.valeur);
                free(token_n.valeur);
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
                specifique = 1;
            }

            if (specifique == 0)
            {
                word[index_word] = buffer[index_buffer];
                word[index_word + 1] = '\0'; // Terminer le mot
                index_word++;

                if (strcmp(word, "#include") == 0 || strcmp(word, "#define") == 0) // Gestion #include
                {
                    index_word = 0;
                    Token token_pp; // crée le token préprocesseur
                    token_pp.type = TOKEN_PREPROCESSOR;
                    token_pp.valeur = malloc(strlen("#include") + 1);
                    strcpy(token_pp.valeur, "#include");
                    printf("TOKEN_PREPROCESSOR: \"%s\"\n", token_pp.valeur);
                    free(token_pp.valeur);
                    reset_word(word); // Réinitialiser le mot
                }
                else if (find_key_word(word) != -1)
                {
                    Token token_k;
                    token_k.type = TOKEN_KEYWORD;
                    token_k.valeur = malloc(strlen(word) + 1);
                    strcpy(token_k.valeur, word);
                    printf("TOKEN_KEYWORD: \"%s\"\n", token_k.valeur);
                    free(token_k.valeur);
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                }
                else if (find_operator(word) != -1)
                {
                    Token token_o;
                    token_o.type = TOKEN_OPERATOR;
                    token_o.valeur = malloc(strlen(word) + 1);
                    strcpy(token_o.valeur, word);
                    printf("TOKEN_OPERATOR: \"%s\"\n", token_o.valeur);
                    free(token_o.valeur);
                    index_word = 0;
                    reset_word(word); // Réinitialiser le mot
                }
            }
            index_buffer++;
        }
        if (buffer[index_buffer] != '\0')
        {
            if (word[0] != '\0' && word != NULL)
            {
                Token token_i;
                token_i.type = TOKEN_IDENTIFIER;
                token_i.valeur = malloc(strlen(word) + 1);
                strcpy(token_i.valeur, word);
                printf("TOKEN_IDENTIFIER: \"%s\"\n", token_i.valeur);
                free(token_i.valeur);
            }

            reset_word(word); // Réinitialiser le mot
            word[0] = buffer[index_buffer];
            word[1] = '\0'; // Terminer le mot
            if (find_puntuation(buffer[index_buffer]) != 0)
            {
                char temp[2];
                temp[0] = buffer[index_buffer];
                temp[1] = '\0';
                Token token_p;
                token_p.type = TOKEN_PUNCTUATION;
                token_p.valeur = malloc(strlen(temp) + 1);
                strcpy(token_p.valeur, temp);
                printf("TOKEN_PUNCTUATION: \"%s\"\n", token_p.valeur);
                free(token_p.valeur);
            }
            else if (find_operator(word) != -1)
            {
                Token token_o;
                token_o.type = TOKEN_OPERATOR;
                token_o.valeur = malloc(strlen(word) + 1);
                strcpy(token_o.valeur, word);
                printf("TOKEN_OPERATOR: \"%s\"\n", token_o.valeur);
                free(token_o.valeur);
                index_word = 0;
                reset_word(word); // Réinitialiser le mot
            }

            index_buffer++;
        }
    }
}
