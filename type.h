#ifndef TYPE_H
#define TYPE_H

typedef enum
{
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_ASSIGNMENT,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_PREPROCESSOR,
    TOKEN_PUNCTUATION,
} TokenType;

typedef struct
{
    TokenType type;
    int ligne;
    char *valeur;
} Token;

typedef struct
{
    Token *tokens;
    int count;
} TokenList;

// Tous les types de nœud que tu vas rencontrer
typedef enum
{
    NODE_BLOCK,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_FOR,
    NODE_KEYWORD,
    NODE_WHILE,
    NODE_BINARY_EXPR, // + - * / % == != < <= > >= && ||
    NODE_UNARY_EXPR,  // ++ -- - !
    NODE_LITERAL,     // entier, flottant, char, string
    NODE_IDENTIFIER,
    NODE_POINTER // **
    /* … ajoute tes autres types ici … */
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    Token token; // optionnel : copie du token (utile pour littéraux ou identifiants)
    int line;    // numéro de ligne pour les erreurs
    int pointer_level;
    // enfant et frère pour naviguer dans l’arbre
    struct ASTNode *first_child;
    struct ASTNode *next_sibling;

    // payload spécifique selon type de nœud
    union
    {
        struct
        { // pour NARY (ex: programme ou bloc)
          // aucun champ, on utilisera first_child / next_sibling
        } nary;

        struct
        { // pour binaire
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;

        struct
        { // pour unaire
            struct ASTNode *operande;
        } unary;

        struct
        { // pour littéral
            union
            {
                int int_val;
                double float_val;
                char char_val;
                char *str_val;
            };
        } literal;

        struct
        { // pour identifiant
            char *name;
        } ident;

        // tu peux ajouter d’autres payloads spécifiques…
    } data;

} ASTNode;

typedef struct Pile
{
    int index;           // ID unique du bloc (utile pour debug ou table de symboles)
    struct Pile *parent; // Pointeur vers le bloc parent
} Pile;

typedef struct T
{
    char *type;
    Token token;
    char *name;
    int index_block;
    struct T *suivant;
} SymbolEntry;

typedef struct
{
    SymbolEntry *tete;
    int count;
} Analyse_Table;

#endif
