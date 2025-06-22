#ifndef PARSE_CONTROL_STRUCTURES_H
#define PARSE_CONTROL_STRUCTURES_H
#include "type.h"
#include "expression.h"
#include "control_structures.h"
#include "instruction.h"
#include "parse_expression.h"
#include "parse_instruction.h"
#include "utils.h"

ASTNode *parse_if(TokenList *tokens, int *index, int *current_block_index);
ASTNode *parse_for(TokenList *tokens, int *index, int *current_block_index);
ASTNode *parse_while(TokenList *tokens, int *index, int *current_block_index);

#endif