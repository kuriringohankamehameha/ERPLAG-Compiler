#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "stack.h"

typedef enum {
    TYPE_INTEGER,
    TYPE_BOOLEAN,
    TYPE_REAL,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_MODULE,
    TYPE_NONE,
    TYPE_ERROR
} TypeName;

struct SymbolRecord {
    // SymbolTable Structure
    // Reference: https://www.csie.ntu.edu.tw/~b93501005/slide5.pdf
    // Contains:
    // * Variable Name (Variable Length Name)
    // * Function Name
    // * Module Name
    // * Type Name
    // * Constant Value (if applicable)
    // * Scope Label
    // * Total Size
    // * Offset (for Arrays)
    Token token;
    TypeName type_name;
    int scope_label; // Defaults to 0 for global scope
    int end;
    int offset;
    term element_type;
    // For Dynamic Arrays
    char* offset_id;
    char* end_id;
    // Runtime address
    int addr;
};

typedef struct SymbolRecord SymbolRecord;

typedef struct St_item St_item;

// Define the Hash Table Item here
struct St_item {
    char* key;
    //char* value;
    SymbolRecord* value;
};


typedef struct SymbolLinkedList SymbolLinkedList;

// Define the SymbolLinkedlist here
struct SymbolLinkedList {
    St_item* item; 
    SymbolLinkedList* next;
};


typedef struct SymbolHashTable SymbolHashTable;

// Define the Hash Table here
struct SymbolHashTable {
    // Contains an array of pointers
    // to items
    St_item** items;
    SymbolLinkedList** overflow_buckets;
    int size;
    int count;
    unsigned long (*hash_function)(char*);
};


SymbolHashTable* create_symtable(int size, unsigned long (*hash_fun)(char*));
SymbolRecord* create_symbolrecord(Token token, TypeName type_name, int scope_label, int end, int offset, term element_type, char* offset_id, char* end_id);
void free_symtable(SymbolHashTable* table);
St_item* create_symitem(char* key, SymbolRecord* value);
void free_symitem(St_item* item);
void free_symrecord(SymbolRecord* record);
void free_symtable(SymbolHashTable* table);
SymbolHashTable* st_insert(SymbolHashTable* table, char* key, SymbolRecord* value);
SymbolRecord* st_search(SymbolHashTable* table, char* key);
void handle_collision(SymbolHashTable* table, unsigned long index, St_item* item);
void st_delete(SymbolHashTable* table, char* key);
void print_symrecord(SymbolRecord* symbolrecord, char ch);
void print_search_symtable(SymbolHashTable* table, char* key);
void print_symtable(SymbolHashTable* table);
void print_symtables(SymbolHashTable** tables, int num_tables);


char* get_string_from_type(TypeName);
TypeName get_typename_from_term(term token_type);
SymbolHashTable*** createSymbolTables(ASTNode* root);
void create_scope_table(SymbolHashTable*** symboltables_ptr, int index);
void insert_into_symbol_table(SymbolHashTable*** symboltables_ptr, char* key, SymbolRecord* record, int index);
void perform_semantic_analysis(SymbolHashTable*** symboltables_ptr, ASTNode* root);
void semantic_analyzer_wrapper(SymbolHashTable*** symboltables_ptr, ASTNode* root);
void free_symtables(SymbolHashTable** tables, int num_tables);

// Keep two variables to keep track of nested scopes
static int start_scope = 0;
static int end_scope = 0;

// Keep track of modulewise scopes
static int* modules = NULL;
static int module_index = -1;

static int error = 0;
static TypeName expression_type = TYPE_NONE;
static bool set_to_boolean = false;

static bool has_semantic_error = false;

// Stack for nested scopes
static Stack** scope_stacks = NULL;

static bool convert_to_bool = false;

// Temp array to hold array type expression parameters
// First parameter is the type name, while the second and third
// correspond to the offset and the end indices of the array
// The last parameter is for the line number
static int expression_array_params[4] = {(int)TYPE_NONE, -1, -1, -1};
static char* expression_array_name = NULL;

struct ArrayOffset {
    int offset;
    int end;
    Token offset_id;
    Token end_id;
}ArrayOffset;

static struct ArrayOffset array_offset = {0, 0, {TK_NONE, NULL, -1}, {TK_NONE, NULL, -1}};

static bool is_dynamic = false;

static int tmp_scope = -1;

#endif
