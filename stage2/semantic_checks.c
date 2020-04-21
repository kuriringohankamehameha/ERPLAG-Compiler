#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "common.h"
#include "hash_table.h"
#include "lexer.h"
#include "time.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "function_table.h"

extern HashTable* keyword_table;
extern unsigned long (*hash_fun)(char*); // Function Pointer to the Hash Function
extern Keyword keywords[];
extern int total_scope;
extern int* start_num_scope;
extern int* end_num_scope;

int main(int argc, char* argv[]) {    
    FILE* fp = fopen("grammar_rules.txt", "r");
    Grammar g = populate_grammar(fp);
    fclose(fp);

    FirstAndFollow f = ComputeFirstAndFollowSets(g);
    ParseTable p = createParseTable(f, g);
    hash_fun = &hash_func;
    keyword_table = populate_hash_table(keyword_table, keywords, hash_fun);
    
    TreeNode* parseTree = generateParseTree("test/symboltable_test.txt", p, g);
    printf("-------------------------------------------------------------\n");
    printf("Parse Tree:\n");
    printf("Token\tLine No\tLexeme\t\tNum. Value\t\tParent\t\tIs Leaf\t\tSymbol Type\n");
    printParseTree(parseTree);
    printf("-------------------------------------------------------------\n");

    // AST Operations Here
    printf("Generating AST...\n");
    generate_AST(parseTree);
    print_AST(parseTree->node);

    // Symbol Tables
    SymbolHashTable*** tables_ptr = createSymbolTables(parseTree->node);
    printf("Printing Symbol Tables\n");
    SymbolHashTable** tables = *tables_ptr;
    print_symtables(tables, total_scope);
    if (start_num_scope) free(start_num_scope);
    if (end_num_scope) free(end_num_scope);
    free_stacks(30);
    free_symtables(tables, total_scope);
    free(tables_ptr);

    free_AST(parseTree->node);
    free_parse_tree(parseTree);
    free_parse_table(p);
    free_first_and_follow(f);
    free_grammar(g);
    free_table(keyword_table);
    return 0;
}
