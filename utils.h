#ifndef UTILS_H
#define UTILS_H

int taille_liste(const char *liste[]);
char find_separator(char word);
char find_puntuation(char word);
int find_single_operator(char *word);
int find_duo_operator(char first, char second);
int find_key_word(char *word);
int is_wordcondition(char *word);
int is_type(char *word);

#endif // UTILS_H
