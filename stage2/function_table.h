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

FunctionTable* create_function_table(char** variables, int num_variables, int total_size, char** input_plist, int num_input_params, char** output_plist, int num_output_params, int base_address, int scope_number, Stack* scope_stack);
bool search_function_table_lexeme(char* lexeme, int index);
bool search_function_table_input_parameter(char* input_parameter, int index);
bool search_function_table_output_parameter(char* output_parameter, int index);
void add_variable_ft(char* variable, int index);
void add_input_parameter(char* input_parameter, int index);
void add_output_parameter(char* output_parameter, int index);
void free_function_table(int num_modules);
void realloc_ft(int index);
void print_function_tables(int idxs);
void print_function_table(int index);

#endif
