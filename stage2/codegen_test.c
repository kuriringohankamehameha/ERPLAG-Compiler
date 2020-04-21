#include "common.h"
#include "hash_table.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "stack.h"
#include "function_table.h"
#include "symbol_table.h"
#include "codegen.h"

extern HashTable* keyword_table;
extern unsigned long (*hash_fun)(char*); // Function Pointer to the Hash Function
extern Keyword keywords[];
extern int total_scope;
extern int* start_num_scope;
extern int* end_num_scope;
extern FILE* fp;
extern int* modules;
extern int module_index;

int main() {
    FILE* gfp = fopen("grammar_rules.txt", "r");
    Grammar g = populate_grammar(gfp);
    fclose(gfp);

    FirstAndFollow f = ComputeFirstAndFollowSets(g);
    ParseTable p = createParseTable(f, g);
    hash_fun = &hash_func;
    keyword_table = populate_hash_table(keyword_table, keywords, hash_fun);
    
    TreeNode* parseTree = generateParseTree("test/codegen_1.txt", p, g);
    // AST Operations Here
    generate_AST(parseTree);

    printf("Generated AST\n");

    SymbolHashTable*** tables_ptr = createSymbolTables(parseTree->node);
    SymbolHashTable** tables = *tables_ptr;
    modules = NULL; module_index = 0; total_scope = 0; start_num_scope = NULL; end_num_scope = NULL; total_scope = 0;
    FunctionTable** function_tables = create_function_tables(100);
    SymbolHashTable*** dup = createSymbolTables(parseTree->node);
    function_tables = semantic_analysis(dup,parseTree->node);
    
    //print_function_tables(function_tables, start_scope + 1);

    // SymbolHashTable** tables = *tables_ptr;

    printf("Now performing Code Generation\n");

    fp = fopen("output.asm", "w");
    printf("Writing...\n");
    write_code(parseTree->node, function_tables, tables_ptr);
    printf("Done\n");
    fclose(fp);

    free(tables_ptr);

    free_AST(parseTree->node);
    free_parse_tree(parseTree);
    free_parse_table(p);
    free_first_and_follow(f);
    free_grammar(g);
    free_table(keyword_table);
    return 0;
}
