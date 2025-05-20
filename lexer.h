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

void analyser_lexical(char *buffer);

#endif // LEXER_H