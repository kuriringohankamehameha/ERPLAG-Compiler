#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "function_table.h"

int main() {
    FunctionTable** function_tables = create_function_tables(10);
    function_tables = add_variable_ft(function_tables, "Lol", 0);
    function_tables = add_variable_ft(function_tables, "Hello", 1);
    function_tables = add_variable_ft(function_tables, "Boyo", 1);
    print_function_tables(function_tables, 2);
    free_function_table(function_tables, 10);
    return 0;
}
