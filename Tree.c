#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"


typedef struct tree {
    struct tree **tree;
    TokenType type;
    int child_count;
}ParserTree;

int addChild(ParserTree *parent, ParserTree *child) {
    ParserTree **tmp = realloc(parent->tree, (parent->child_count + 1) * sizeof *tmp);
    if (!tmp) return -1;  // échec alloc
    parent->tree = tmp;
    parent->tree[parent->child_count++] = child;
    return 0;
}

ParserTree createTree(ParserTree **child) {
    ParserTree tree;
    tree.type = -1;
    tree.child_count = 0;
    tree.tree = child;
    return tree;
}