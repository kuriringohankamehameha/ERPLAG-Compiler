#include "common.h"
#include "symbol_table.h"

unsigned long hash_function_sym(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

int main() {
    SymbolRecord* value = create_symbolrecord("num", TYPE_INTEGER, NULL, "10", 1, 4, 0);
    SymbolHashTable* table = create_symtable(1000, hash_function_sym);
    table = st_insert(table, "num", value);
    SymbolRecord* value2 = create_symbolrecord("num", TYPE_INTEGER, NULL, "20", 2, 4, 0);
    table = st_insert(table, "num", value2);
    SymbolRecord* value3 = create_symbolrecord("abd", TYPE_INTEGER, NULL, "20", 2, 4, 0);
    table = st_insert(table, "abd", value3);
    SymbolRecord* value4 = create_symbolrecord("bbc", TYPE_INTEGER, NULL, "20", 2, 4, 0);
    table = st_insert(table, "bbc", value4);
    print_symtable(table);
    free_symtable(table);
    return 0;
}
