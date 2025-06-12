#ifndef PARSER_H
#define PARSER_H
#include "type.h"
#include "utils.h"
#include "instruction.h"
#include "expression.h"
#include "control_structures.h"
#include "parse_instruction.h"

void parser(TokenList *tokens, ASTNode *root, Analyse_Table *table, int *current_block_index);

#endif