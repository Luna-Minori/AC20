#ifndef LEXER_H
#define LEXER_H
#include "type.h"
#include "utils.h"

void lexer(char *buffer, TokenList *list);
void free_token_list(TokenList *list);

#endif // LEXER_H