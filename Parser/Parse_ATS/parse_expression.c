#include "parse_expression.h"
#include <stdio.h>
#include <string.h>

ASTNode *parse_expression(TokenList *tokens, int *index);
ASTNode *parse_logical_or(TokenList *tokens, int *index);
ASTNode *parse_logical_and(TokenList *tokens, int *index);
ASTNode *parse_comparison(TokenList *tokens, int *index);
ASTNode *parse_addition(TokenList *tokens, int *index);
ASTNode *parse_multiplication(TokenList *tokens, int *index);
ASTNode *parse_factor(TokenList *tokens, int *index);

// Helper pour tester un opérateur sans avancer
static int peek_operator(TokenList *t, int index, const char *op)
{
    return index < t->count &&
           t->tokens[index].type == TOKEN_OPERATOR &&
           strcmp(t->tokens[index].valeur, op) == 0;
}

// parse_expression → logical_or
ASTNode *parse_expression(TokenList *tokens, int *index)
{
    int start = *index;
    printf("[parse_expression] enter idx=%d token=%s\n",
           start,
           start < tokens->count ? tokens->tokens[start].valeur : "EOF");

    ASTNode *node = parse_logical_or(tokens, index);

    if (!node)
    {
        *index = start;
        printf("[parse_expression] fail, reset idx=%d\n", start);
    }
    else
    {
        printf("[parse_expression] exit node=%s new_idx=%d\n",
               node->type == NODE_BINARY_EXPR ? "BinaryExpr" : node->type == NODE_LITERAL  ? "Literal"
                                                           : node->type == NODE_IDENTIFIER ? "Identifier"
                                                                                           :
                                                                                           /*…*/ "Other",
               *index);
    }
    return node;
}

ASTNode *parse_addition(TokenList *tokens, int *index)
{
    int start = *index;
    printf("  [parse_addition] enter idx=%d token=%s\n",
           start,
           start < tokens->count ? tokens->tokens[start].valeur : "EOF");

    ASTNode *left = parse_multiplication(tokens, index);
    if (!left)
    {
        *index = start;
        printf("  [parse_addition] no left → fail idx=%d\n", start);
        return NULL;
    }

    while (*index < tokens->count && is_add_op(tokens->tokens[*index]))
    {
        Token op = tokens->tokens[(*index)++];
        printf("  [parse_addition] found op '%s' at idx=%d\n", op.valeur, *index - 1);

        ASTNode *right = parse_multiplication(tokens, index);
        if (!right)
        {
            *index = start;
            free_AST(left);
            printf("  [parse_addition] right NULL → backtrack idx=%d\n", start);
            return NULL;
        }

        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
        printf("  [parse_addition] build BinaryExpr, now idx=%d\n", *index);
    }

    printf("  [parse_addition] exit, returning %s idx=%d\n",
           left->type == NODE_BINARY_EXPR ? "BinaryExpr" : "Identifier/Literal",
           *index);
    return left;
}

// logical_or → logical_and { "||" logical_and }*
ASTNode *parse_logical_or(TokenList *tokens, int *index)
{
    int start = *index;
    ASTNode *left = parse_logical_and(tokens, index);
    if (!left)
    {
        *index = start;
        return NULL;
    }
    while (peek_operator(tokens, *index, "||"))
    {
        Token op = tokens->tokens[(*index)++];
        ASTNode *right = parse_logical_and(tokens, index);
        if (!right)
        {
            free_AST(left);
            *index = start;
            return NULL;
        }
        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
    }
    return left;
}

// logical_and → comparaison { "&&" comparaison }*
ASTNode *parse_logical_and(TokenList *tokens, int *index)
{
    int start = *index;
    ASTNode *left = parse_comparison(tokens, index);
    if (!left)
    {
        *index = start;
        return NULL;
    }
    while (peek_operator(tokens, *index, "&&"))
    {
        Token op = tokens->tokens[(*index)++];
        ASTNode *right = parse_comparison(tokens, index);
        if (!right)
        {
            free_AST(left);
            *index = start;
            return NULL;
        }
        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
    }
    return left;
}

// comparaison → addition { compar_op addition }*
ASTNode *parse_comparison(TokenList *tokens, int *index)
{
    int start = *index;
    ASTNode *left = parse_addition(tokens, index);
    if (!left)
    {
        *index = start;
        return NULL;
    }
    static const char *ops[] = {"==", "!=", "<", "<=", ">", ">=", NULL};
    while (*index < tokens->count && tokens->tokens[*index].type == TOKEN_OPERATOR)
    {
        const char *val = tokens->tokens[*index].valeur;
        int i = 0;
        for (; ops[i]; ++i)
            if (strcmp(val, ops[i]) == 0)
                break;
        if (!ops[i])
            break;
        Token op = tokens->tokens[(*index)++];
        ASTNode *right = parse_addition(tokens, index);
        if (!right)
        {
            free_AST(left);
            *index = start;
            return NULL;
        }
        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
    }
    return left;
}
/*
// addition → multiplication { ("+"|"-") multiplication }*
ASTNode *parse_addition(TokenList *tokens, int *index)
{
    int start = *index;
    ASTNode *left = parse_multiplication(tokens, index);
    if (!left)
    {
        *index = start;
        return NULL;
    }
    while (*index < tokens->count && is_add_op(tokens->tokens[*index]))
    {
        Token op = tokens->tokens[(*index)++];
        ASTNode *right = parse_multiplication(tokens, index);
        if (!right)
        {
            free_AST(left);
            *index = start;
            return NULL;
        }
        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
    }
    return left;
}
*/
// multiplication → factor { ("*"|"/"|"%") factor }*
ASTNode *parse_multiplication(TokenList *tokens, int *index)
{
    int start = *index;
    ASTNode *left = parse_factor(tokens, index);
    if (!left)
    {
        *index = start;
        return NULL;
    }
    while (*index < tokens->count && is_mul_op(tokens->tokens[*index]))
    {
        Token op = tokens->tokens[(*index)++];
        ASTNode *right = parse_factor(tokens, index);
        if (!right)
        {
            free_AST(left);
            *index = start;
            return NULL;
        }
        left = new_ATS(NODE_BINARY_EXPR, left, right, op, op.ligne);
    }
    return left;
}

// factor → "++" factor | "--" factor | "(" expression ")" [postfix] | literal [postfix]
ASTNode *parse_factor(TokenList *tokens, int *index)
{
    int start = *index;
    Token t;
    if (*index < tokens->count)
        t = tokens->tokens[*index];

    // préfixe ++/--
    if (peek_operator(tokens, *index, "++") || peek_operator(tokens, *index, "--"))
    {
        Token op = tokens->tokens[(*index)++];
        ASTNode *child = parse_factor(tokens, index);
        if (!child)
        {
            *index = start;
            return NULL;
        }
        return new_ATS(NODE_UNARY_EXPR, child, NULL, op, op.ligne);
    }

    // parenthèses
    if (*index < tokens->count && is_openingParenthesis(tokens->tokens[*index]))
    {
        (*index)++;
        ASTNode *expr = parse_expression(tokens, index);
        if (!expr || *index >= tokens->count || !is_closingParenthesis(tokens->tokens[*index]))
        {
            *index = start;
            if (expr)
                free_AST(expr);
            return NULL;
        }
        (*index)++;
        // postfixe ++/--
        if (*index < tokens->count && (peek_operator(tokens, *index, "++") || peek_operator(tokens, *index, "--")))
        {
            Token op = tokens->tokens[(*index)++];
            expr = new_ATS(NODE_UNARY_EXPR, expr, NULL, op, op.ligne);
        }
        return expr;
    }

    // littéral
    if (*index < tokens->count && is_literal(tokens, index))
    {
        Token lit = tokens->tokens[(*index) - 1]; // is_literal a avancé index
        ASTNode *node = (lit.type == TOKEN_IDENTIFIER)
                            ? new_ATS(NODE_IDENTIFIER, NULL, NULL, lit, lit.ligne)
                            : new_ATS(NODE_LITERAL, NULL, NULL, lit, lit.ligne);
        // postfixe ++/--
        if (*index < tokens->count && (peek_operator(tokens, *index, "++") || peek_operator(tokens, *index, "--")))
        {
            Token op = tokens->tokens[(*index)++];
            node = new_ATS(NODE_UNARY_EXPR, node, NULL, op, op.ligne);
        }
        return node;
    }

    *index = start;
    return NULL;
}
