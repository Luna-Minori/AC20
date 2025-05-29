#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "type.h"
#include "utils.h"

int is_expression(TokenList *tokens, int *index);
int is_logical_or(TokenList *tokens, int *index);
int is_logical_and(TokenList *tokens, int *index);
int is_comparaison(TokenList *tokens, int *index);
int is_addition(TokenList *tokens, int *index);
int is_multiplication(TokenList *tokens, int *index);
int is_factor(TokenList *tokens, int *index);
int is_literal(TokenList *tokens, int *index);

int is_valide_operande(Token t);

int is_add_op(Token t);

int is_mul_op(Token t);

#endif