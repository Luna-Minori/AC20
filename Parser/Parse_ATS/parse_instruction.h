#ifndef PARSE_INSTRUCTION_H
#define PARSE_INSTRUCTION_H

#include "utils.h"
#include "type.h"
#include "instruction.h"
#include "expression.h"
#include "control_structures.h"
#include "parse_expression.h"
#include "parse_controle_structures.h"

ASTNode *parse_instruction(TokenList *tokens, int *index, int *current_block_index);
ASTNode *parse_declaration(TokenList *tokens, int *index, int *current_block_index);
ASTNode *parse_assignment(TokenList *tokens, int *index);
ASTNode *parse_block(TokenList *tokens, int *index, int *current_block_index);

#endif