#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"

int is_pointvirgule(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ";") == 0; }

int is_crochet(Token t) { return t.type == TOKEN_PUNCTUATION && (strcmp(t.valeur, "{") == 0 || strcmp(t.valeur, "}") == 0); }

int is_openingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "(") == 0; }

int is_closingParenthesis(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, ")") == 0; }

int is_openingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "{") == 0; }

int is_closingBrace(Token t) { return t.type == TOKEN_PUNCTUATION && strcmp(t.valeur, "}") == 0; }

int is_token_pointvirgule(TokenList *t, int *index)
{
    if (*index >= t->count)
        return 0;
    return is_pointvirgule(t->tokens[*index]);
}

int taille_liste(const char *liste[])
{
    int taille = 0;
    while (liste[taille] != NULL)
    {
        taille++;
    }
    return taille;
}

char find_separator(char word)
{
    const char separators[] = {
        ' ', '\n', '\t', ';', ',', '(', ')', '{', '}',
        '[', ']', '<', '>', '=', '!', '+', '-', '*', '/',
        '&', '|', '^', '%', '\0'};

    for (int i = 0; separators[i] != '\0'; i++)
    {
        if (separators[i] == word) // Ensure no stray characters
        {
            return separators[i];
        }
    }
    return 0; // Si aucun mot-clé n'est trouvé
}

char find_puntuation(char word)
{
    const char ponctuation[] = {';', ',', '(', ')', '{', '}',
                                '[', ']'};

    for (int i = 0; ponctuation[i] != '\0'; i++)
    {
        if (ponctuation[i] == word) // Ensure no stray characters
        {
            return ponctuation[i];
        }
    }
    return -1; // Si aucun mot-clé n'est trouvé
}

int find_single_operator(char *word)
{
    const char *operators[] = {
        "+", "-", "*", "/", "%", "&", "^", "<", ">", "!", NULL};
    for (int i = 0; i < taille_liste(operators); i++)
    {
        if (strcmp(word, operators[i]) == 0)
        {
            return i;
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int find_duo_operator(char first, char second)
{
    char word[3];
    word[0] = first;
    word[1] = second;
    word[2] = '\0';
    const char *duo_operators[] = {
        "++", "--", "==", "!=", "<=", ">=", "&&", "||",
        "<<", ">>", "->", NULL};

    for (int i = 0; i < taille_liste(duo_operators); i++)
    {
        if (strcmp(word, duo_operators[i]) == 0)
        {
            return i;
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int find_duo_assigment(char first, char second)
{
    char word[3];
    word[0] = first;
    word[1] = second;
    word[2] = '\0';
    const char *duo_operators[] = {
        "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", NULL};

    for (int i = 0; i < taille_liste(duo_operators); i++)
    {
        if (strcmp(word, duo_operators[i]) == 0)
        {
            return i;
        }
    }
    return -1; // Si aucun mot-clé n'est trouvé
}

int find_key_word(char *word)
{
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "default", "do",
        "double", "else", "enum", "float", "for", "if",
        "int", "long", "return", "short", "signed", "sizeof",
        "struct", "switch", "typedef", "unsigned", "void", "while", "main", "free", "malloc", NULL};
    // Parcours du tableau de mots-clés
    for (int i = 0; i < taille_liste(keywords); i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return i; // Retourne l'indice correspondant
        }
    }

    return -1; // Si aucun mot-clé n'est trouvé
}

int is_type(char *word)
{
    const char *types[] = {
        "int", "float", "double", "char", "void", NULL};
    for (int i = 0; i < taille_liste(types); i++)
    {
        if (strcmp(word, types[i]) == 0)
        {
            return i; // Le mot est un type
        }
    }
    return -1; // Le mot n'est pas un type
}

int is_wordcondition(char *word)
{
    const char *conditions[] = {
        "if", "else", "switch", NULL};
    for (int i = 0; i < taille_liste(conditions); i++)
    {
        if (strcmp(word, conditions[i]) == 0)
        {
            return i; // Le mot est une condition
        }
    }
    return -1; // Le mot n'est pas une condition
}

int is_comparaison_op(char *word)
{
    const char *comparaison_ops[] = {
        "==", "!=", "<", "<=", ">", ">=", NULL};
    for (int i = 0; i < taille_liste(comparaison_ops); i++)
    {
        if (strcmp(word, comparaison_ops[i]) == 0)
        {
            return 1; // C'est un opérateur de comparaison
        }
    }
    return 0; // Ce n'est pas un opérateur de comparaison
}

ASTNode *new_ATS(NodeType type, ASTNode *l, ASTNode *r, Token tok, int line)
{
    ASTNode *n = malloc(sizeof *n);
    n->type = type;
    n->token = tok;
    n->line = line;
    n->pointer_level = 0;
    n->first_child = n->next_sibling = NULL;
    n->data.binary.left = l;
    n->data.binary.right = r;
    return n;
}

void add_child(ASTNode *parent, ASTNode *child)
{
    if (!parent->first_child)
    {
        parent->first_child = child;
    }
    else
    {
        ASTNode *s = parent->first_child;
        while (s->next_sibling)
            s = s->next_sibling;
        s->next_sibling = child;
    }
}

const char *nodetype_to_string(NodeType type)
{
    switch (type)
    {
    case NODE_BLOCK:
        return "Block";
    case NODE_DECLARATION:
        return "Declaration";
    case NODE_ASSIGNMENT:
        return "Assignment";
    case NODE_IF:
        return "If";
    case NODE_FOR:
        return "For";
    case NODE_WHILE:
        return "While";
    case NODE_BINARY_EXPR:
        return "Binary Expression";
    case NODE_UNARY_EXPR:
        return "Unary Expression";
    case NODE_LITERAL:
        return "Literal";
    case NODE_IDENTIFIER:
        return "Identifier";
    // Ajoute ici les autres types si tu en as
    default:
        return "Unknown NodeType";
    }
}

void print_ast(ASTNode *node, int indent)
{
    if (!node)
        return;
    // indentation
    for (int i = 0; i < indent; i++)
        printf("  ");

    switch (node->type)
    {
    case NODE_BLOCK:
        printf("Block\n");
        break;
    case NODE_DECLARATION:
        printf("Declaration\n");
        break;
    case NODE_ASSIGNMENT:
        printf("Assignment: %s\n", node->token.valeur);
        break;
    case NODE_IF:
        printf("If\n");
        break;
    case NODE_FOR:
        printf("For\n");
        break;
    case NODE_WHILE:
        printf("While\n");
        break;
    case NODE_BINARY_EXPR:
        printf("BinaryExpr: %s\n", node->token.valeur);
        break;
    case NODE_UNARY_EXPR:
        printf("UnaryExpr: %s\n", node->token.valeur);
        break;
    case NODE_LITERAL:
        // afficher selon type de literal (supposons token contient la string)
        printf("Literal: %s\n", node->token.valeur);
        break;
    case NODE_IDENTIFIER:
        printf("Identifier: %s\n", node->token.valeur);
        break;
    case NODE_KEYWORD:
        printf("Keyword: %s\n", node->token.valeur);
        break;
    default:
        printf("Unknown node\n");
    }

    // Afficher enfants selon type
    switch (node->type)
    {
    case NODE_BINARY_EXPR:
        print_ast(node->data.binary.left, indent + 1);
        print_ast(node->data.binary.right, indent + 1);
        break;
    case NODE_UNARY_EXPR:
        print_ast(node->data.unary.operande, indent + 1);
        break;
    default:
        // pour tous les n-aires (bloc, déclarations, instructions…), on utilise first_child/next_sibling
        for (ASTNode *child = node->first_child; child; child = child->next_sibling)
        {
            print_ast(child, indent + 1);
        }
        break;
    }
}

void free_AST(ASTNode *node)
{
    if (!node)
        return;

    // Libération récursive des enfants
    ASTNode *child = node->first_child;
    while (child)
    {
        ASTNode *next = child->next_sibling;
        free_AST(child);
        child = next;
    }

    // Libération du contenu spécifique selon le type
    switch (node->type)
    {
    case NODE_LITERAL:
        if (node->token.type == TOKEN_STRING && node->token.valeur)
        {
            free(node->token.valeur); // libérer la chaîne copiée
        }
        break;

    case NODE_IDENTIFIER:
        if (node->data.ident.name)
        {
            free(node->data.ident.name);
        }
        break;

    default:
        break; // les autres types ne nécessitent pas de libération spéciale
    }

    // Libération du nœud lui-même
    free(node);
}

void print_symbol_table(Analyse_Table *table)
{
    printf("\n===== Table des symboles =====\n");
    SymbolEntry *entry = table->tete;
    int count = 0;
    while (entry != NULL)
    {
        printf("Symbole %d :\n", ++count);
        printf("  Nom          : %s\n", entry->name);
        printf("  Type         : %s\n", entry->type);
        printf("  Ligne source : %d\n", entry->token.ligne);
        printf("-----------------------------\n");
        entry = entry->suivant;
    }
    if (count == 0)
    {
        printf("  (aucun symbole)\n");
    }
}

Pile *create_block(Pile *parent, int *index)
{
    Pile *b = malloc(sizeof(Pile));
    if (!b)
    {
        perror("malloc");
        exit(1);
    }
    printf("creation nouveau block pile \n");
    b->index = (*index++); // chaque appel reçoit un nouvel ID
    printf("debug index\n");
    b->parent = parent;
    return b;
}

int is_block_visible(Pile *current, int block_index)
{
    while (current)
    {
        if (current->index == block_index)
            return 1;
        current = current->parent;
    }
    return 0;
}
