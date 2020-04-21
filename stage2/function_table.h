#ifndef FUNCTION_TABLE_H
#define FUNCTION_TABLE_H

#include "common.h"
#include "stack.h"

typedef struct FunctionTable {
    // Structure for the Activation Record
    // of a module
    char** variables;
    int num_variables;
    int total_size;
    char** input_plist;
    int num_input_params;
    char** output_plist;
    int num_output_params;
    int base_address;
    int scope_number;
    Stack* scope_stack;
}FunctionTable;

FunctionTable** create_function_tables(int capacity);
bool search_function_table_lexeme(FunctionTable** function_tables, char* lexeme, int index);
bool search_function_table_input_parameter(FunctionTable** function_tables, char* input_parameter, int index);
bool search_function_table_output_parameter(FunctionTable** function_tables, char* output_parameter, int index);
FunctionTable** add_variable_ft(FunctionTable** function_tables, char* variable, int index);
FunctionTable** add_input_parameter(FunctionTable** function_tables, char* input_parameter, int index);
FunctionTable** add_output_parameter(FunctionTable** function_tables, char* output_parameter, int index);
void free_function_table(FunctionTable** function_tables, int num_modules);
FunctionTable** realloc_ft(FunctionTable** function_tables, int index);
void print_function_tables(FunctionTable** function_tables, int idxs);
void print_function_table(FunctionTable** function_tables, int index);

#endif
