#include "common.h"
#include "symbol_table.h"

unsigned long hash_function_sym(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

int main() {
    SymbolHashTable* table = create_symtable(1000, hash_function_sym);
    SymbolRecord* value = create_symbolrecord("num", NULL, NULL, TYPE_INTEGER, "10", 1, 4, 0);
    table = st_insert(table, "num", value);
    SymbolRecord* value2 = create_symbolrecord("num", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    table = st_insert(table, "num", value2);
    SymbolRecord* value3 = create_symbolrecord("abd", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    table = st_insert(table, "abd", value3);
    SymbolRecord* value4 = create_symbolrecord("bbc", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    table = st_insert(table, "bbc", value4);
    print_search_symtable(table, "bbc");
    print_search_symtable(table, "abd");
    print_search_symtable(table, "none");
    print_symtable(table);
    st_delete(table, "abd");
    st_delete(table, "bbc");
    print_symtable(table);
    free_symtable(table);
    return 0;
}
