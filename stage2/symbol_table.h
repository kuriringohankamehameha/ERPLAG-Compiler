#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"

typedef enum {
    TYPE_INTEGER,
    TYPE_BOOLEAN,
    TYPE_REAL,
    TYPE_ARRAY
} TypeName;

struct SymbolTable {
    // SymbolTable Structure
    // Reference: https://www.csie.ntu.edu.tw/~b93501005/slide5.pdf
    // Contains:
    // * Variable Name (Variable Length Name)
    // * Type Name
    // * Function Name
    // * Constant Value (if applicable)
    // * Scope Label
    // * Total Size
    // * Offset (for Arrays)
    char* var_name;
    TypeName type_name;
    char* fun_name;
    char* const_value;
    int scope_label; // Defaults to 0 for global scope
    int total_size;
    int offset;
};

typedef struct SymbolTable SymbolTable;

char* get_string_from_type(TypeName);
SymbolTable* createSymbolTable(ASTNode*);

#endif
