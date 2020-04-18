#ifndef ACTIVATION_RECORD_H
#define ACTIVATION_RECORD_H

#include "common.h"
#include "stack.h"

typedef struct ActivationRecord {
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
}ActivationRecord;

ActivationRecord* create_activation_record(char** variables, int num_variables, int total_size, char** input_plist, int num_input_params, char** output_plist, int num_output_params, int base_address, int scope_number, Stack* scope_stack);
bool search_activation_record_lexeme(char* lexeme, int index);
bool search_activation_record_input_parameter(char* input_parameter, int index);
bool search_activation_record_output_parameter(char* output_parameter, int index);
void add_variable_activation(char* variable, int index);
void add_input_parameter(char* input_parameter, int index);
void add_output_parameter(char* output_parameter, int index);
void free_activation_record(int num_modules);
void realloc_activation(int index);
void print_activation_records(int idxs);
void print_activation_record(int index);

#endif
