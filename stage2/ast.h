#ifndef AST_H
#define AST_H

#include "common.h"
#include "lexerDef.h"

typedef struct ASTNode {
    // The Abstract Syntax Tree Node
    term token_type; // The Terminal / NonTerminal from the corresponding Grammar Rule
    Token token; // Contains the token carried forward from the Lexer. Only Leaf nodes have this attribute
    struct ASTNode* parent, **children; // Pointers to the parent and children
    int num_children;
}ASTNode;

#endif
