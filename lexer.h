#ifndef LEXER_H
#define LEXER_H

typedef enum
{
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_PREPROCESSOR,
    TOKEN_PUNCTUATION,
    TOKEN_COMMENT,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    int ligne;
    char *valeur;
} Token;

typedef struct {
    Token *tokens;
    int count;
} TokenList;

void lexer(char *buffer, TokenList *list);
void free_token_list(TokenList *list);

#endif // LEXER_H