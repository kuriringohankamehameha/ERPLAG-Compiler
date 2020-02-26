// Group No 42
// R VIJAY KRISHNA 2017A7PS0183P
// ROHIT K 2017A7PS0177P

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "common.h"

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item {
    char* key;
    //char* value;
    term value;
};


typedef struct LinkedList LinkedList;

// Define the Linkedlist here
struct LinkedList {
    Ht_item* item; 
    LinkedList* next;
};


typedef struct HashTable HashTable;

// Define the Hash Table here
struct HashTable {
    // Contains an array of pointers
    // to items
    Ht_item** items;
    LinkedList** overflow_buckets;
    int size;
    int count;
    unsigned long (*hash_function)(char*);
};


HashTable* create_table(int size, unsigned long (*hash_fun)(char*));
void free_table(HashTable* table);
HashTable* ht_insert(HashTable* table, char* key, term value);
term ht_search(HashTable* table, char* key);
void ht_delete(HashTable* table, char* key);
void print_term_type(term t, char ch);
void print_search(HashTable* table, char* key);
void print_hashtable(HashTable* table);
#endif
