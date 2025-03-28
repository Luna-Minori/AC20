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
    TOKEN_COMMENT,      // Commentaires
    TOKEN_END           // Fin de fichier
} TokenType;

typedef struct
{
    TokenType type;
    int index;
    char *valeur;
} Token;

typedef struct T
{
    TokenType type;
    char *value;
    struct T *lbranches;
} Tree;

int taille_liste(const char **liste)
{
    int taille = 0;
    while (liste[taille] != NULL)
    {
        taille++;
    }
    return taille;
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

int find_ponctuation(char *word)
{
    const char *punctuation[] = {
        ";", ",", "(", ")", "{", "}", "[", "]", ".", "->", "#include", NULL};

    for (int i = 0; i < taille_liste(punctuation); i++)
    {
        if (strcmp(word, punctuation[i]) == 0)
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
        "struct", "switch", "typedef", "unsigned", "void", "while", NULL};

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

void analyser_lexical(char *buffer)
{
    char *temp = buffer;
    char word[100];
    int index_word = 0;
    int index_buffer = 0;

    while (temp[index_buffer] != '\0') // Tant que on est pas à la fin du fichier
    {
        while (buffer[index_buffer] == ' ' || buffer[index_buffer] == '\n' || buffer[index_buffer] == '\t')
        {
            index_buffer++;
        }

        index_word = 0;
        while (buffer[index_buffer] != ' ' && buffer[index_buffer] != '\0' && buffer[index_buffer] != '\n' && buffer[index_buffer] != '\t')
        {

            if (buffer[index_buffer] == '"') // Gestion des String pour avoir "un seul token"
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
                }
                else
                {
                    printf("Erreur: Chaîne de caractères non terminée\n");
                    return;
                }
            }
            else if (buffer[index_buffer] != '\0' && buffer[index_buffer + 1] != '\0' && buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '/') // Gestion des commentaires une ligne
            {
                index_buffer += 2; // on saute le //
                index_buffer++;
                index_word = 0;

                while (buffer[index_buffer] != '\n' && buffer[index_buffer] != '\0') // lit toute la chaine
                {
                    word[index_word] = buffer[index_buffer];
                    index_buffer++;
                    index_word++;
                }

                if (buffer[index_buffer] != '\0' && buffer[index_buffer] == '\n') // si c'est fin de ligne alors token comment
                {
                    word[index_word] = '\0';
                    index_buffer++;

                    Token token_c; // crée le token commentaire
                    token_c.type = TOKEN_COMMENT;
                    token_c.valeur = malloc(strlen(word) + 1);
                    strcpy(token_c.valeur, word);
                    printf("TOKEN_COMMENT: \"%s\"\n", token_c.valeur);
                    free(token_c.valeur);
                }
                else
                { // pas de fin de ligne donc erreur
                    printf("Erreur: Commentaire non terminé\n");
                    return;
                }
            }
            else if (buffer[index_buffer] == '/' && buffer[index_buffer + 1] == '*') // Gestion des commentaires multi-lignes
            {
                index_buffer += 2; // on saute le /*
                index_word = 0;

                while (buffer[index_buffer] != '\0' && buffer[index_buffer + 1] != '\0' && buffer[index_buffer] != '*' && buffer[index_buffer + 1] != '/') // lit toute la chaine
                {
                    word[index_word] = buffer[index_buffer];
                    index_buffer++;
                    index_word++;
                }

                if (buffer[index_buffer] != '\0' && buffer[index_buffer + 1] != '\0' && buffer[index_buffer] == '*' && buffer[index_buffer + 1] == '/') // si c'est fin de commentaire alors token comment
                {
                    word[index_word] = '\0';
                    index_buffer += 2; // saute */

                    Token token_c;
                    token_c.type = TOKEN_COMMENT;
                    token_c.valeur = malloc(strlen(word) + 1);
                    strcpy(token_c.valeur, word);
                    printf("TOKEN_COMMENT: \"%s\"\n", token_c.valeur);
                    free(token_c.valeur);
                }
                else
                { // pas de fin de commentaire donc erreur
                    printf("Erreur: Commentaire non terminé\n");
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
                }
                else
                {
                    printf("Erreur: Caractère non terminé\n");
                    return;
                }
            }
            else if (buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // Gestion des nombres (entiers et flottants)
            {
                index_word = 0;
                while (buffer[index_buffer] != '\0' && buffer[index_buffer] >= '0' && buffer[index_buffer] <= '9') // Gestion des entiers
                {
                    word[index_word] = buffer[index_buffer];
                    index_word++;
                    index_buffer++;
                }

                if (buffer[index_buffer] == '.' || buffer[index_buffer] == ',') // Gestion des floats si c'est le cas
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

                        if (buffer[index_buffer] == '.' || buffer[index_buffer] == ',') // Gestion des exposants floats si c'est le cas
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

                    word[index_word] = '\0'; // Terminer le mot
                    Token token_n;           // Créer le token nombre
                    token_n.type = TOKEN_NUMBER;
                    token_n.valeur = malloc(strlen(word) + 1);
                    strcpy(token_n.valeur, word);
                    printf("TOKEN_NUMBER: \"%s\"\n", token_n.valeur);
                    free(token_n.valeur);
                }
                else if (buffer[index_buffer] == '#' && buffer[index_buffer + 1] != '\0' && buffer[index_buffer + 1] == 'i' && buffer[index_buffer + 2] != '\0' && buffer[index_buffer + 2] == 'n' && buffer[index_buffer + 3] != '\0' && buffer[index_buffer + 3] == 'c' && buffer[index_buffer + 4] != '\0' && buffer[index_buffer + 4] == 'l' && buffer[index_buffer + 5] != '\0' && buffer[index_buffer + 5] == 'u' && buffer[index_buffer + 6] != '\0' && buffer[index_buffer + 6] == 'd' && buffer[index_buffer + 7] != '\0' && buffer[index_buffer + 7] == 'e') // Gestion #include
                {
                    index_buffer += 8; // on saute le #include
                    Token token_pp;    // crée le token préprocesseur
                    token_pp.type = TOKEN_PREPROCESSOR;
                    token_pp.valeur = malloc(strlen("#include") + 1);
                    strcpy(token_pp.valeur, "#include");
                    printf("TOKEN_PREPROCESSOR: \"%s\"\n", token_pp.valeur);
                    free(token_pp.valeur);
                }
                word[index_word] = temp[index_buffer];
                ++index_word;
                ++index_buffer;
            }
            word[index_word] = '\0';

            int index_ponctuation = find_ponctuation(word);
            if (index_ponctuation != -1)
            {
                Token token_p;
                token_p.type = TOKEN_PUNCTUATION;
                token_p.valeur = malloc(strlen(word) + 1);
                strcpy(token_p.valeur, word);
                token_p.index = index_ponctuation;
            }
            else
            {
                int index_operator = find_operator(word);
                if (index_operator != -1)
                {
                    Token token_o;
                    token_o.type = TOKEN_OPERATOR;
                    token_o.index = index_operator;
                    token_o.valeur = malloc(strlen(word) + 1);
                    strcpy(token_o.valeur, word);
                }
                else
                {
                    int index_keyword = find_key_word(word);
                    if (index_keyword != -1)
                    {
                        Token token_k;
                        token_k.type = TOKEN_KEYWORD;
                        token_k.index = index_keyword;
                        token_k.valeur = malloc(strlen(word) + 1);
                        strcpy(token_k.valeur, word);
                    }
                    else
                    {
                        Token token_i;
                        token_i.type = TOKEN_IDENTIFIER;
                        token_i.valeur = malloc(strlen(word) + 1);
                        strcpy(token_i.valeur, word);
                    }
                }
            }
            index_buffer++;
        }
    }
}

char *Read_file(const char *nom_fichier)
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    // Aller à la fin du fichier pour connaître sa taille
    fseek(fichier, 0, SEEK_END);
    long taille = ftell(fichier);
    rewind(fichier); // Retour au début

    if (taille <= 0)
    {
        fclose(fichier);
        return NULL; // Fichier vide
    }

    // Allouer la mémoire
    char *buffer = malloc(taille + 1);
    if (buffer == NULL)
    {
        perror("Erreur d'allocation mémoire");
        fclose(fichier);
        return NULL;
    }

    // Lire le fichier en une seule fois
    size_t lu = fread(buffer, 1, taille, fichier);
    buffer[lu] = '\0'; // Assurer une fin de chaîne correcte

    fclose(fichier);
    return buffer;
}

int main()
{
    char *buffer = NULL;
    char nom_fichier[] = "Main.c";
    printf("Hello\n");
    buffer = Read_file(nom_fichier);
    printf("\n\n\n %s", buffer);
    if (buffer != NULL)
    {
        analyser_lexical(buffer);
        free(buffer); // Libérer la mémoire après utilisation
    }
    return 0;
}