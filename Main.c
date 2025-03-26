#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TOKEN_KEYWORD,     // Mot-clé comme "if", "int", etc.
    TOKEN_IDENTIFIER,  // Identificateur (nom de variable, fonction)
    TOKEN_OPERATOR,    // Opérateurs comme +, -, *, /, etc.
    TOKEN_NUMBER,      // Nombre entier ou flottant
    TOKEN_STRING,      // Chaîne de caractères
    TOKEN_PUNCTUATION, // Ponctuation comme (, ), {, }, ;, etc.
    TOKEN_COMMENT,     // Commentaires
    TOKEN_END          // Fin de fichier
} TokenType;

typedef struct
{
    TokenType type;
    char valeur[100];
} Token;

int taille_liste(char *liste[])
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
        "&&", "||", "!", ",", ":"};

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
        ";", ",", "(", ")", "{", "}", "[", "]", ".", "->", "#", "#include", "/*", "*/", "//"};

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
        "struct", "switch", "typedef", "unsigned", "void", "while"};

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

    while (temp[index_buffer] != '\0') // Correction de la condition
    {
        while (buffer[index_buffer] == ' ' || buffer[index_buffer] == '\n' || buffer[index_buffer] == '\t')
        {
            index_buffer++;
        }

        index_word = 0;
        while (buffer[index_buffer] != ' ' && buffer[index_buffer] != '\0' && buffer[index_buffer] != '\n' && buffer[index_buffer] != '\t')
        {
            word[index_word] = temp[index_buffer];
            ++index_word;
            ++index_buffer;
        }
        word[index_word] = '\0';

       
        index_buffer++;
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
    char nom_fichier[] = "Test.txt";

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