#include <stdio.h>
#include <stdlib.h>
#include "activation_record.h"

ActivationRecord* activation_records = NULL;

bool search_activation_record_lexeme(char* lexeme, int index);
bool search_activation_record_input_parameter(char* input_parameter, int index);
bool search_activation_record_output_parameter(char* output_parameter, int index);
void add_variable_activation(char* variable, int index);
void add_input_parameter(char* input_parameter, int index);
void add_output_parameter(char* output_parameter, int index);
void free_activation_record(int num_modules);

ActivationRecord* create_activation_record(char** variables, int num_variables, int total_size, char** input_plist, int num_input_params, char** output_plist, int num_output_params, int base_address, int scope_number, Stack* scope_stack) {
    ActivationRecord* activation_record = calloc (1, sizeof(ActivationRecord));
    activation_record->variables = variables;
    activation_record->num_variables = num_variables;
    activation_record->total_size = total_size;
    activation_record->input_plist = input_plist;
    activation_record->num_input_params = num_input_params;
    activation_record->output_plist = output_plist;
    activation_record->num_output_params = num_output_params;
    activation_record->base_address = base_address;
    activation_record->scope_number = scope_number;
    activation_record->scope_stack = scope_stack;

    return activation_record;
}

void realloc_activation(int index) {
    activation_records = realloc(activation_records, (index + 1) * sizeof (ActivationRecord));
}

bool search_activation_record_lexeme(char* lexeme, int index) {
    for (int i=0; i<activation_records[index].num_variables;i++) {
        if (strcmp(activation_records[index].variables[i], lexeme) == 0)
            return true;
    }
    return false;
}

bool search_activation_record_input_parameter(char* input_parameter, int index) {
    for (int i=0; i<activation_records[index].num_input_params;i++) {
        if (strcmp(activation_records[index].input_plist[i], input_parameter) == 0)
            return true;
    }
    return false;
}

bool search_activation_record_output_parameter(char* output_parameter, int index) {
    for (int i=0; i<activation_records[index].num_output_params;i++) {
        if (strcmp(activation_records[index].output_plist[i], output_parameter) == 0)
            return true;
    }
    return false;
}

void add_variable_activation(char* variable, int index) {
    if (activation_records == NULL)
        activation_records = calloc (1, sizeof(ActivationRecord));
    if (activation_records[index].variables == NULL)
        activation_records[index].variables = calloc (1, sizeof(char*));
    else
        activation_records[index].variables = realloc(activation_records[index].variables, (activation_records[index].num_variables + 1) * sizeof(char*));
    activation_records[index].variables[activation_records[index].num_variables] = variable;
    activation_records[index].num_variables++;
}

void add_input_parameter(char* input_parameter, int index) {
    if (activation_records == NULL)
        activation_records = calloc (1, sizeof(ActivationRecord));
    if (activation_records[index].input_plist == NULL)
        activation_records[index].input_plist = calloc (1, sizeof(char*));
    else
        activation_records[index].input_plist = realloc(activation_records[index].input_plist, (activation_records[index].num_input_params + 1) * sizeof(char*));
    activation_records[index].input_plist[activation_records[index].num_input_params] = input_parameter;
    activation_records[index].num_input_params++;
}

void add_output_parameter(char* output_parameter, int index) {
    if (activation_records == NULL)
        activation_records = calloc (1, sizeof(ActivationRecord));
    if (activation_records[index].output_plist == NULL)
        activation_records[index].output_plist = calloc (1, sizeof(char*));
    else
        activation_records[index].output_plist = realloc(activation_records[index].output_plist, (activation_records[index].num_output_params + 1) * sizeof(char*));
    activation_records[index].output_plist[activation_records[index].num_output_params] = output_parameter;
    activation_records[index].num_output_params++;
}

void print_activation_record(int index) {
    printf("\nFor Index %d:\n", index);
    if (activation_records[index].variables) {
        printf("Variables: ");
        for (int i=0; i<activation_records[index].num_variables; i++) {
            printf("%s , ", activation_records[index].variables[i]);
        }
        printf("\n");
    }
    if (activation_records[index].input_plist) {
        printf("Input Parameters: ");
        for (int i=0; i<activation_records[index].num_input_params; i++) {
            printf("%s , ", activation_records[index].input_plist[i]);
        }
        printf("\n");
    }
    if (activation_records[index].output_plist) {
        printf("Output Parameters: ");
        for (int i=0; i<activation_records[index].num_output_params; i++) {
            printf("%s , ", activation_records[index].output_plist[i]);
        }
    }
    printf("\n");
}

void print_activation_records(int idxs) {
    for (int i=0; i<idxs; i++)
        print_activation_record(i);
}

void free_activation_record(int num_modules) {
    if (activation_records) {
        for (int index = 0; index < num_modules; index++) {
            if (activation_records[index].variables) {
                for (int i=0; i<activation_records[index].num_variables; i++) {
                    activation_records[index].variables[i] = NULL;
                }
                free(activation_records[index].variables);
            }
            if (activation_records[index].input_plist) {
                for (int i=0; i<activation_records[index].num_input_params; i++) {
                    activation_records[index].input_plist[i] = NULL;
                }
                free(activation_records[index].input_plist);
            }
            if (activation_records[index].output_plist) {
                for (int i=0; i<activation_records[index].num_output_params; i++) {
                    activation_records[index].output_plist[i] = NULL;
                }
                free(activation_records[index].output_plist);
            }
            activation_records[index].scope_stack = NULL;
        }
        free(activation_records);
    }
}
