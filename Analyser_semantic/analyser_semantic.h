#ifndef ANALYSER_SEMANTIC_H
#define ANALYSER_SEMANTIC_H
#include "type.h"
#include "utils.h"

int analyser_semantique(ASTNode *node, Analyse_Table *table, Pile *pile, Analyse_Table *verif_table, int *index);

#endif