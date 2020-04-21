#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "ast.h"
#include "symbol_table.h"

#define SIZE_INTEGER 4
#define SIZE_BOOLEAN 4
#define SIZE_REAL 4

void open_files();
void code_init();
void code_end();
void cleanup();

void generate_code(ASTNode* node, FunctionTable** function_tables, SymbolHashTable*** symboltables_ptr);
void io_handler(SymbolHashTable*** symboltables_ptr, ASTNode* ioStmtNode, int scope);
void expression_handler(SymbolHashTable*** symboltables_ptr, ASTNode* expNode, int scope);
void varidnum_handler(SymbolHashTable*** symboltables_ptr, ASTNode* varnode, int scope);
void index_handler(SymbolHashTable*** symboltables_ptr, ASTNode* indexnode, int scope);
void factor_handler(SymbolHashTable*** symboltables_ptr, ASTNode* factnode, int scope);
void term_handler(SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope);
void arithmetic_handler(SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope);
void anyterm_handler(SymbolHashTable*** symboltables_ptr, ASTNode* anynode, int scope);
void aob_handler(SymbolHashTable*** symboltables_ptr, ASTNode* aobnode, int scope);
void unary_handler(SymbolHashTable*** symboltables_ptr, ASTNode* unarynode, int scope);
void assign_handler(SymbolHashTable*** symboltables_ptr, ASTNode* assignmentStmtNode, int scope);
void assign_handler(SymbolHashTable*** symboltables_ptr, ASTNode* assignmentStmtNode, int scope);
void stmt_handler(SymbolHashTable*** symboltables_ptr, ASTNode* stmtNode, int scope);
void statements_handler(SymbolHashTable*** symboltables_ptr, ASTNode* statementsNode, int scope);
void write_code(ASTNode* root, FunctionTable** function_tables, SymbolHashTable*** symboltables_ptr);

#endif
