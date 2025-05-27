#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "../utils.h"

int is_assignment(TokenList *tokens, int *index); 
int is_declaration(TokenList *tokens, int *index);
int is_instruction(TokenList *tokens, int *index);

#endif // PARSER_H
