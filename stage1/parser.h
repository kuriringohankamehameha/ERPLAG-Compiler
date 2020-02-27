// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#ifndef PARSER_H
#define PARSER_H
#include "common.h"
#include "hash_table.h"
#include "lexer.h"
#include "parserDef.h"

// Declare all global variables here
unsigned long hash_func (char* str);
void free_tree(TreeNode* root);
void free_parse_tree(TreeNode* root);
void printTreeNode(TreeNode* root);
void free_grammar(Grammar g);
void free_first_and_follow(FirstAndFollow f);
void free_parse_table(ParseTable p);
void print_grammar(Grammar g);
Grammar populate_grammar(FILE* fp);
FirstAndFollow ComputeFirstAndFollowSets(Grammar G);
ParseTable createParseTable(FirstAndFollow F, Grammar G);
TreeNode* generateParseTree (char* filename, ParseTable p, Grammar g);
void printParseTree(TreeNode* root);
#endif
