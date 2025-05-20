#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

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
    buffer = Read_file(nom_fichier);
    printf("\n\n\n %s \n\n\n", buffer);

    if (buffer != NULL)
    {
        analyser_lexical(buffer);
        free(buffer); // Libérer la mémoire après utilisation
    }
    return 0;
}