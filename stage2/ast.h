#ifndef AST_H
#define AST_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"

typedef struct Synthesized {
    term token_type;
    int scope_no;
}Synthesized;

typedef struct ASTNode {
    // The Abstract Syntax Tree Node
    term token_type; // The Terminal / NonTerminal from the corresponding Grammar Rule
    Token token; // Contains the token carried forward from the Lexer. Only Leaf nodes have this attribute
    struct ASTNode* parent, **children; // Pointers to the parent and children
    int num_children;
    // For synthesized attributes (type checking, etc)
    Synthesized syn_attribute;
    bool visited;
    bool check_term;
}ASTNode;

ASTNode* make_ASTNode(ASTNode* child, term token_type);
void add_ASTChild(ASTNode* parent, ASTNode* child);
ASTNode* make_ASTLeaf(ASTNode* parent, Token token);
void generate_AST(TreeNode* root);
void synthesize_attributes(ASTNode* root);
void print_AST(ASTNode* root);
void print_AST_without_parent(ASTNode* parent);
void pretty_print_AST(ASTNode* node);
void printAbstractSyntaxTree(ASTNode* root);
void free_AST(ASTNode* root);

#endif
