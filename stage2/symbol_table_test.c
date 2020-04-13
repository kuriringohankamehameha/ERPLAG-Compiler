#include "common.h"
#include "symbol_table.h"

unsigned long hash_function_sym(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

int main() {
    SymbolRecord* value = create_symbolrecord("num", TYPE_INTEGER, NULL, "10");
    SymbolHashTable* table = create_symtable(1000, hash_function_sym);
    table = st_insert(table, "global", value);
    print_symtable(table);
    free_symtable(table);
    return 0;
}
