#include "common.h"
#include "symbol_table.h"

char* get_string_from_type(TypeName typename) {
    switch(typename) {
        case TYPE_INTEGER:
        return "TYPE_INTEGER";
        case TYPE_BOOLEAN:
        return "TYPE_BOOLEAN";
        case TYPE_REAL:
        return "TYPE_REAL";
        case TYPE_ARRAY:
        return "TYPE_ARRAY";
        default:
        return "Not Yet Implemented";
    }
    return NULL;
}

SymbolTable* createSymbolTable(ASTNode* root) {
    // Creates a Symbol Table
    SymbolTable* symboltable = (SymbolTable*) calloc (1, sizeof(SymbolTable));
    return symboltable;
}
