#ifndef CONTROL_STRUCTURES_H
#define CONTROL_STRUCTURES_H
#include "type.h"
#include "expression.h"
#include "instruction.h"
#include "utils.h"

int is_if(TokenList *tokens, int *index);
int is_for(TokenList *tokens, int *index);
int is_while(TokenList *tokens, int *index);

#endif