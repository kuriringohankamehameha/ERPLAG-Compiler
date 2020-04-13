#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"

typedef enum {
    TYPE_INTEGER,
    TYPE_BOOLEAN,
    TYPE_REAL,
    TYPE_ARRAY,
    TYPE_NONE
} TypeName;

struct SymbolRecord {
    // SymbolTable Structure
    // Reference: https://www.csie.ntu.edu.tw/~b93501005/slide5.pdf
    // Contains:
    // * Variable Name (Variable Length Name)
    // * Type Name
    // * Function Name
    // * Constant Value (if applicable)
    // * Scope Label
    // * Total Size
    // * Offset (for Arrays)
    char* var_name;
    TypeName type_name;
    char* fun_name;
    char* const_value;
    int scope_label; // Defaults to 0 for global scope
    int total_size;
    int offset;
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
SymbolRecord* create_symbolrecord(char* var_name, TypeName type_name, char* fun_name, char* const_value, int scope_label, int total_size, int offset);
void free_symtable(SymbolHashTable* table);
St_item* create_symitem(char* key, SymbolRecord* value);
void free_symitem(St_item* item);
void free_symrecord(SymbolRecord* record);
void free_symtable(SymbolHashTable* table);
SymbolHashTable* st_insert(SymbolHashTable* table, char* key, SymbolRecord* value);
SymbolRecord* st_search(SymbolHashTable* table, char* key);
void handle_collision(SymbolHashTable* table, unsigned long index, St_item* item);
void st_delete(SymbolHashTable* table, char* key);
void print_record_symtable(SymbolRecord* symbolrecord, char ch);
void print_search_symtable(SymbolHashTable* table, char* key);
void print_symtable(SymbolHashTable* table);


char* get_string_from_type(TypeName);
SymbolHashTable* createSymbolTable(ASTNode*);

#endif
