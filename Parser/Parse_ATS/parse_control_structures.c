#include <stdio.h>
#include "utils.h"
#include "string.h"
#include "parse_controle_structures.h"

ASTNode *parse_if(TokenList *tokens, int *index, int *current_block_index)
{
    int start = *index;
    // 1) Vérification syntaxique (avance index) via is_if
    if (!is_if(tokens, index))
    {
        *index = start;
        return NULL;
    }

    // 2) Création du nœud IF
    Token ifTok = tokens->tokens[start];
    ASTNode *ifNode = new_ATS(NODE_IF, NULL, NULL, ifTok, ifTok.ligne);

    // 3a) On reprend la lecture juste après le mot-clé « if »
    int pos = start;
    pos++; // on passe « if »
    // 3b) Consomme une seule '('
    if (pos >= tokens->count || !is_openingParenthesis(tokens->tokens[pos]))
    {
        *index = start;
        free_AST(ifNode);
        return NULL;
    }
    pos++;

    // 3c) Laisse parse_expression avaler **toutes** les parentheses imbriquées
    ASTNode *cond = parse_expression(tokens, &pos);
    if (!cond)
    {
        *index = start;
        free_AST(ifNode);
        return NULL;
    }
    add_child(ifNode, cond);

    // 3d) Consomme une seule ')'
    if (pos >= tokens->count || !is_closingParenthesis(tokens->tokens[pos]))
    {
        fprintf(stderr, "parse_if : parenthèse fermante manquante après condition\n");
        *index = start;
        free_AST(ifNode);
        return NULL;
    }
    pos++;

    // 4) then-block
    ASTNode *thenBlock = parse_block(tokens, &pos, current_block_index);
    if (!thenBlock)
    {
        *index = start;
        free_AST(ifNode);
        return NULL;
    }
    add_child(ifNode, thenBlock);

    // 5) else / else if
    if (pos < tokens->count && tokens->tokens[pos].type == TOKEN_KEYWORD && strcmp(tokens->tokens[pos].valeur, "else") == 0)
    {
        pos++;
        ASTNode *elseNode;
        if (pos < tokens->count && tokens->tokens[pos].type == TOKEN_KEYWORD && strcmp(tokens->tokens[pos].valeur, "if") == 0)
        {
            elseNode = parse_if(tokens, &pos, current_block_index);
        }
        else
        {
            elseNode = parse_block(tokens, &pos, current_block_index);
        }
        if (!elseNode)
        {
            *index = start;
            free_AST(ifNode);
            return NULL;
        }
        add_child(ifNode, elseNode);
    }

    // 6) Met à jour l’index appelant et renvoie l’IF
    *index = pos;
    return ifNode;
}

ASTNode *parse_while(TokenList *tokens, int *index, int *current_block_index)
{
    int start = *index;

    // 1) Test syntaxique
    if (!is_while(tokens, index))
    {
        *index = start;
        return NULL;
    }

    // root
    Token whileTok = tokens->tokens[start];
    ASTNode *whileNode = new_ATS(NODE_WHILE, NULL, NULL, whileTok, whileTok.ligne);

    // 3) Reconstruire les enfants :
    //    a) condition
    //    (start+1) == '('
    int pos = start + 2; // skip "while" et "("
    ASTNode *cond = parse_expression(tokens, &pos);
    add_child(whileNode, cond);

    if (pos >= tokens->count || !is_closingParenthesis(tokens->tokens[pos]))
    {
        fprintf(stderr, "parse_while : parenthèse fermante manquante après condition\n");
        *index = start;
        free_AST(whileNode);
        return NULL;
    }
    pos++;

    //    b) then-block
    ASTNode *thenBlock = parse_block(tokens, &pos, current_block_index);
    add_child(whileNode, thenBlock);

    // 4) Synchroniser l’index et retourner
    *index = pos;
    return whileNode;
}

ASTNode *parse_for(TokenList *tokens, int *index, int *current_block_index)
{
    int start = *index;
    if (!is_for(tokens, index))
    {
        *index = start;
        return NULL;
    }

    Token forTok = tokens->tokens[start];
    ASTNode *forNode = new_ATS(NODE_FOR, NULL, NULL, forTok, forTok.ligne);

    int pos = start + 1; // skip "for"

    // Parenthèse ouvrante
    if (pos >= tokens->count || !is_openingParenthesis(tokens->tokens[pos]))
    {
        *index = start;
        free_AST(forNode);
        return NULL;
    }
    pos++; // consomme '('

    // INIT
    ASTNode *init = NULL;
    int tmp = pos;
    init = parse_declaration(tokens, &tmp, current_block_index);
    if (!init)
    {
        tmp = pos;
        init = parse_assignment(tokens, &tmp);
    }
    if (!init)
    {
        tmp = pos;
        init = parse_expression(tokens, &tmp);
    }

    if (init)
    {
        pos = tmp;
        add_child(forNode, init);
    }

    // Point-virgule après INIT
    if (pos >= tokens->count || !is_token_pointvirgule(tokens, &pos))
    {
        *index = start;
        free_AST(forNode);
        return NULL;
    }
    pos++; // consomme ';'

    // COND
    ASTNode *cond = NULL;
    tmp = pos;
    if (is_expression(tokens, &tmp))
    {
        tmp = pos;
        cond = parse_expression(tokens, &tmp);
        if (cond)
        {
            pos = tmp;
            add_child(forNode, cond);
        }
    }

    // Point-virgule après COND
    if (pos >= tokens->count || !is_token_pointvirgule(tokens, &pos))
    {
        *index = start;
        free_AST(forNode);
        return NULL;
    }
    pos++; // consomme ';'

    // POST
    ASTNode *post = NULL;
    tmp = pos;
    post = parse_assignment(tokens, &tmp);
    if (!post)
    {
        tmp = pos;
        post = parse_expression(tokens, &tmp);
    }

    if (post)
    {
        pos = tmp;
        add_child(forNode, post);
    }

    // Parenthèse fermante
    if (pos >= tokens->count || !is_closingParenthesis(tokens->tokens[pos]))
    {
        *index = start;
        free_AST(forNode);
        return NULL;
    }
    pos++; // consomme ')'

    // Bloc
    ASTNode *body = parse_block(tokens, &pos, current_block_index);
    if (!body)
    {
        *index = start;
        free_AST(forNode);
        return NULL;
    }

    add_child(forNode, body);
    *index = pos;
    return forNode;
}
