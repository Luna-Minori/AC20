#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "Analyser_Semantic/analyser_semantic.h"

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
    char *buffer = Read_file("test2.txt");

    if (buffer)
    {
        printf("hello");
        // LEXER
        TokenList *list = malloc(sizeof(TokenList));
        list->tokens = NULL;
        list->count = 0;
        lexer(buffer, list);
        for (int i = 0; i < list->count; ++i)
        {
            printf("TOKEN[%d] type=%d ligne=%d valeur=%s\n", i, list->tokens[i].type, list->tokens[i].ligne, list->tokens[i].valeur);
        }

        // PARSER
        ASTNode *root = new_ATS(NODE_BLOCK, NULL, NULL, list->tokens[0], list->tokens[0].ligne);
        Analyse_Table *table = malloc(sizeof(Analyse_Table));
        if (!table)
        {
            perror("malloc table");
            exit(EXIT_FAILURE);
        }
        table->count = 0;
        table->tete = NULL;
        int current_block_index = 0;
        parser(list, root, table, &current_block_index);

        // ANALYSER
        Analyse_Table *verif_table = malloc(sizeof(Analyse_Table));
        if (!table)
        {
            perror("malloc table");
            exit(EXIT_FAILURE);
        }
        verif_table->count = 0;
        verif_table->tete = NULL;
        int index = 0;
        Pile *pile = create_block(NULL, &index);
        analyser_semantique(root, table, pile, verif_table, &index);
        print_symbol_table(verif_table);
        printf("arbre valide \n");
        free_token_list(list);
        free(buffer);
    }

    return 0;
}