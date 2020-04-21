#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "function_table.h"

extern int* modules;
extern int module_index;
extern char** module_names;

bool search_function_table_lexeme(FunctionTable** function_tables, char* lexeme, int index);
bool search_function_table_input_parameter(FunctionTable** function_tables, char* input_parameter, int index);
bool search_function_table_output_parameter(FunctionTable** function_tables, char* output_parameter, int index);
FunctionTable** add_variable_ft(FunctionTable** function_tables, char* variable, int index);
FunctionTable** add_input_parameter(FunctionTable** function_tables, char* input_parameter, int index);
FunctionTable** add_output_parameter(FunctionTable** function_tables, char* output_parameter, int index);

FunctionTable** create_function_tables(int capacity) {
    FunctionTable** tables = calloc (capacity, sizeof(FunctionTable*));
    for (int i=0; i<capacity; i++)
        tables[i] = NULL;
    return tables;
}

FunctionTable** realloc_ft(FunctionTable** function_tables, int index) {
    function_tables = realloc(function_tables, (index + 2) * sizeof (FunctionTable*));
    function_tables[index] = calloc(1, sizeof(FunctionTable)); 
    return function_tables;
}

bool search_function_table_lexeme(FunctionTable** function_tables, char* lexeme, int index) {
    if (!function_tables[index]) return false;
    for (int i=0; i<function_tables[index]->num_variables;i++) {
        if (strcmp(function_tables[index]->variables[i], lexeme) == 0)
            return true;
    }
    return false;
}

bool search_function_table_input_parameter(FunctionTable** function_tables, char* input_parameter, int index) {
    if (!function_tables[index]) return false;
    for (int i=0; i<function_tables[index]->num_input_params;i++) {
        if (strcmp(function_tables[index]->input_plist[i], input_parameter) == 0)
            return true;
    }
    return false;
}

bool search_function_table_output_parameter(FunctionTable** function_tables, char* output_parameter, int index) {
    if (!function_tables[index]) return false;
    for (int i=0; i<function_tables[index]->num_output_params;i++) {
        if (strcmp(function_tables[index]->output_plist[i], output_parameter) == 0)
            return true;
    }
    return false;
}

FunctionTable** add_variable_ft(FunctionTable** function_tables, char* variable, int index) {
    if (function_tables[index] == NULL) {
        function_tables[index] = calloc (1, sizeof(FunctionTable));
    }
    if (function_tables[index]->variables == NULL) {
        function_tables[index]->variables = calloc(1, sizeof(char*));
    }
    else
        function_tables[index]->variables = realloc(function_tables[index]->variables, (function_tables[index]->num_variables + 1) * sizeof(char*));
    function_tables[index]->variables[function_tables[index]->num_variables] = variable;
    function_tables[index]->num_variables++;
    return function_tables;
}

FunctionTable** add_input_parameter(FunctionTable** function_tables, char* input_parameter, int index) {
    if (function_tables[index] == NULL) {
        function_tables[index] = calloc (1, sizeof(FunctionTable));
    }
    if (function_tables[index]->input_plist == NULL)
        function_tables[index]->input_plist = calloc (1, sizeof(char*));
    else
        function_tables[index]->input_plist = realloc(function_tables[index]->input_plist, (function_tables[index]->num_input_params + 1) * sizeof(char*));
    function_tables[index]->input_plist[function_tables[index]->num_input_params] = input_parameter;
    function_tables[index]->num_input_params++;
    return function_tables;
}

FunctionTable** add_output_parameter(FunctionTable** function_tables, char* output_parameter, int index) {
    if (function_tables[index] == NULL) {
        function_tables[index] = calloc (1, sizeof(FunctionTable));
    }
    if (function_tables[index]->output_plist == NULL)
        function_tables[index]->output_plist = calloc (1, sizeof(char*));
    else
        function_tables[index]->output_plist = realloc(function_tables[index]->output_plist, (function_tables[index]->num_output_params + 1) * sizeof(char*));
    function_tables[index]->output_plist[function_tables[index]->num_output_params] = output_parameter;
    function_tables[index]->num_output_params++;
    return function_tables;
}

void print_function_table(FunctionTable** function_tables, int index) {
    if (!function_tables[index]) return;
    if (function_tables[index] && function_tables[index]->num_variables == 0 && function_tables[index]->num_input_params == 0 && function_tables[index]->num_output_params == 0) return;
    // printf("\nModule %s:\n", get_module_name(index));
    printf("\nModule %d:\n", index);
    if (function_tables[index]->variables) {
        printf("Variables: ");
        for (int i=0; i<function_tables[index]->num_variables; i++) {
            printf("%s , ", function_tables[index]->variables[i]);
        }
        printf("\n");
    }
    if (function_tables[index]->input_plist) {
        printf("Input Parameters: ");
        for (int i=0; i<function_tables[index]->num_input_params; i++) {
            printf("%s , ", function_tables[index]->input_plist[i]);
        }
        printf("\n");
    }
    if (function_tables[index]->output_plist) {
        printf("Output Parameters: ");
        for (int i=0; i<function_tables[index]->num_output_params; i++) {
            printf("%s , ", function_tables[index]->output_plist[i]);
        }
    }
    printf("\n");
    printf("Total Activation Record Size = %d\n", function_tables[index]->total_size);
}

void print_function_tables(FunctionTable** function_tables, int idxs) {
    for (int i=0; i<idxs; i++)
        print_function_table(function_tables, i);
}

void free_function_table(FunctionTable** function_tables, int capacity) {
    if (function_tables) {
        for (int index = 0; index < capacity; index++) {
            if (function_tables[index]) {
                if (function_tables[index]->variables) {
                    for (int i=0; i<function_tables[index]->num_variables; i++) {
                        function_tables[index]->variables[i] = NULL;
                    }
                    free(function_tables[index]->variables);
                }
                if (function_tables[index]->input_plist) {
                    for (int i=0; i<function_tables[index]->num_input_params; i++) {
                        function_tables[index]->input_plist[i] = NULL;
                    }
                    free(function_tables[index]->input_plist);
                }
                if (function_tables[index]->output_plist) {
                    for (int i=0; i<function_tables[index]->num_output_params; i++) {
                        function_tables[index]->output_plist[i] = NULL;
                    }
                    free(function_tables[index]->output_plist);
                }
                function_tables[index]->scope_stack = NULL;
                free(function_tables[index]);
            }
        }
        free(function_tables);
    }
}
