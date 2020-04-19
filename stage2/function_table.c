#include <stdio.h>
#include <stdlib.h>
#include "function_table.h"

FunctionTable* function_tables = NULL;

bool search_function_table_lexeme(char* lexeme, int index);
bool search_function_table_input_parameter(char* input_parameter, int index);
bool search_function_table_output_parameter(char* output_parameter, int index);
void add_variable_activation(char* variable, int index);
void add_input_parameter(char* input_parameter, int index);
void add_output_parameter(char* output_parameter, int index);
void free_function_table(int num_modules);

FunctionTable* create_function_table(char** variables, int num_variables, int total_size, char** input_plist, int num_input_params, char** output_plist, int num_output_params, int base_address, int scope_number, Stack* scope_stack) {
    FunctionTable* function_table = calloc (1, sizeof(FunctionTable));
    function_table->variables = variables;
    function_table->num_variables = num_variables;
    function_table->total_size = total_size;
    function_table->input_plist = input_plist;
    function_table->num_input_params = num_input_params;
    function_table->output_plist = output_plist;
    function_table->num_output_params = num_output_params;
    function_table->base_address = base_address;
    function_table->scope_number = scope_number;
    function_table->scope_stack = scope_stack;

    return function_table;
}

void realloc_activation(int index) {
    function_tables = realloc(function_tables, (index + 1) * sizeof (FunctionTable));
}

bool search_function_table_lexeme(char* lexeme, int index) {
    for (int i=0; i<function_tables[index].num_variables;i++) {
        if (strcmp(function_tables[index].variables[i], lexeme) == 0)
            return true;
    }
    return false;
}

bool search_function_table_input_parameter(char* input_parameter, int index) {
    for (int i=0; i<function_tables[index].num_input_params;i++) {
        if (strcmp(function_tables[index].input_plist[i], input_parameter) == 0)
            return true;
    }
    return false;
}

bool search_function_table_output_parameter(char* output_parameter, int index) {
    for (int i=0; i<function_tables[index].num_output_params;i++) {
        if (strcmp(function_tables[index].output_plist[i], output_parameter) == 0)
            return true;
    }
    return false;
}

void add_variable_activation(char* variable, int index) {
    if (function_tables == NULL)
        function_tables = calloc (1, sizeof(FunctionTable));
    if (function_tables[index].variables == NULL)
        function_tables[index].variables = calloc (1, sizeof(char*));
    else
        function_tables[index].variables = realloc(function_tables[index].variables, (function_tables[index].num_variables + 1) * sizeof(char*));
    function_tables[index].variables[function_tables[index].num_variables] = variable;
    function_tables[index].num_variables++;
}

void add_input_parameter(char* input_parameter, int index) {
    if (function_tables == NULL)
        function_tables = calloc (1, sizeof(FunctionTable));
    if (function_tables[index].input_plist == NULL)
        function_tables[index].input_plist = calloc (1, sizeof(char*));
    else
        function_tables[index].input_plist = realloc(function_tables[index].input_plist, (function_tables[index].num_input_params + 1) * sizeof(char*));
    function_tables[index].input_plist[function_tables[index].num_input_params] = input_parameter;
    function_tables[index].num_input_params++;
}

void add_output_parameter(char* output_parameter, int index) {
    if (function_tables == NULL)
        function_tables = calloc (1, sizeof(FunctionTable));
    if (function_tables[index].output_plist == NULL)
        function_tables[index].output_plist = calloc (1, sizeof(char*));
    else
        function_tables[index].output_plist = realloc(function_tables[index].output_plist, (function_tables[index].num_output_params + 1) * sizeof(char*));
    function_tables[index].output_plist[function_tables[index].num_output_params] = output_parameter;
    function_tables[index].num_output_params++;
}

void print_function_table(int index) {
    printf("\nFor Index %d:\n", index);
    if (function_tables[index].variables) {
        printf("Variables: ");
        for (int i=0; i<function_tables[index].num_variables; i++) {
            printf("%s , ", function_tables[index].variables[i]);
        }
        printf("\n");
    }
    if (function_tables[index].input_plist) {
        printf("Input Parameters: ");
        for (int i=0; i<function_tables[index].num_input_params; i++) {
            printf("%s , ", function_tables[index].input_plist[i]);
        }
        printf("\n");
    }
    if (function_tables[index].output_plist) {
        printf("Output Parameters: ");
        for (int i=0; i<function_tables[index].num_output_params; i++) {
            printf("%s , ", function_tables[index].output_plist[i]);
        }
    }
    printf("\n");
}

void print_function_tables(int idxs) {
    for (int i=0; i<idxs; i++)
        print_function_table(i);
}

void free_function_table(int num_modules) {
    if (function_tables) {
        for (int index = 0; index < num_modules; index++) {
            if (function_tables[index].variables) {
                for (int i=0; i<function_tables[index].num_variables; i++) {
                    function_tables[index].variables[i] = NULL;
                }
                free(function_tables[index].variables);
            }
            if (function_tables[index].input_plist) {
                for (int i=0; i<function_tables[index].num_input_params; i++) {
                    function_tables[index].input_plist[i] = NULL;
                }
                free(function_tables[index].input_plist);
            }
            if (function_tables[index].output_plist) {
                for (int i=0; i<function_tables[index].num_output_params; i++) {
                    function_tables[index].output_plist[i] = NULL;
                }
                free(function_tables[index].output_plist);
            }
            function_tables[index].scope_stack = NULL;
        }
        free(function_tables);
    }
}
