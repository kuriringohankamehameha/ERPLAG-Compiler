#include "common.h"
#include "symbol_table.h"

char* get_string_from_type(TypeName typename) {
    switch(typename) {
        case TYPE_INTEGER:
        return "TYPE_INTEGER";
        case TYPE_BOOLEAN:
        return "TYPE_BOOLEAN";
        case TYPE_REAL:
        return "TYPE_REAL";
        case TYPE_ARRAY:
        return "TYPE_ARRAY";
        case TYPE_NONE:
        return "TYPE_NONE";
        case TYPE_FUNCTION:
        return "TYPE_FUNCTION";
        case TYPE_MODULE:
        return "TYPE_MODULE";
        default:
        return "Not Yet Implemented";
    }
    return NULL;
}

unsigned long hash_function(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

SymbolHashTable* createSymbolTable(ASTNode* root) {
    // Creates a Symbol Table
    SymbolHashTable* symboltable = create_symtable(CAPACITY, hash_function);
    SymbolRecord* value = create_symbolrecord("num", NULL, NULL, TYPE_INTEGER, "10", 1, 4, 0);
    symboltable = st_insert(symboltable, "num", value);
    SymbolRecord* value2 = create_symbolrecord("num", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    symboltable = st_insert(symboltable, "num", value2);
    SymbolRecord* value3 = create_symbolrecord("abd", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    symboltable = st_insert(symboltable, "abd", value3);
    SymbolRecord* value4 = create_symbolrecord("bbc", NULL, NULL, TYPE_INTEGER, "20", 2, 4, 0);
    symboltable = st_insert(symboltable, "bbc", value4);
    print_search_symtable(symboltable, "bbc");
    print_search_symtable(symboltable, "abd");
    print_search_symtable(symboltable, "none");
    print_symtable(symboltable);
    return symboltable;
}

SymbolRecord* create_symbolrecord(char* var_name, char* fun_name, char* module_name, TypeName type_name, char* const_value, int scope_label, int total_size, int offset) {
    SymbolRecord* symbolrecord = (SymbolRecord*) calloc (1, sizeof(SymbolRecord));
    if (var_name) {
        symbolrecord->var_name = (char*) calloc (strlen(var_name) + 1, sizeof(char));
        strcpy(symbolrecord->var_name, var_name);
    }
    else
        symbolrecord->var_name = NULL;
    symbolrecord->type_name = type_name;
    if (fun_name) {
        symbolrecord->fun_name = (char*) calloc (strlen(fun_name) + 1, sizeof(char));
        strcpy(symbolrecord->fun_name, fun_name);
    }
    else
        symbolrecord->fun_name = NULL;
    if (module_name) {
        symbolrecord->module_name = (char*) calloc (strlen(fun_name) + 1, sizeof(char));
        strcpy(symbolrecord->module_name, fun_name);
    }
    else
        symbolrecord->module_name = NULL;
    if (const_value) {
        symbolrecord->const_value = (char*) calloc (strlen(const_value) + 1, sizeof(char));
        strcpy(symbolrecord->const_value, const_value);
    }
    else
        symbolrecord->const_value = NULL;
    symbolrecord->scope_label = scope_label;
    symbolrecord->total_size = total_size;
    symbolrecord->offset = offset;
    return symbolrecord;
}

static SymbolLinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    SymbolLinkedList* list = (SymbolLinkedList*) calloc (1, sizeof(SymbolLinkedList));
    return list;
}

static SymbolLinkedList* linkedlist_insert(SymbolLinkedList* list, St_item* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        SymbolLinkedList* head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
    
    else if (list->next == NULL) {
        SymbolLinkedList* node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    SymbolLinkedList* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }
    
    SymbolLinkedList* node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    
    return list;
}

static void free_linkedlist(SymbolLinkedList* list) {
    SymbolLinkedList* temp = list;
    if (!list)
        return;
    while (list) {
        temp = list;
        list = list->next;
        if (temp->item->key)
            free(temp->item->key);
        if (temp->item->value)
            free_symrecord(temp->item->value);
        //free(temp->item->value);
        if (temp->item)
            free(temp->item);
        free(temp);
    }
}

static SymbolLinkedList** create_overflow_buckets(SymbolHashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    SymbolLinkedList** buckets = (SymbolLinkedList**) calloc (table->size, sizeof(SymbolLinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(SymbolHashTable* table) {
    // Free all the overflow bucket lists
    SymbolLinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}


St_item* create_symitem(char* key, SymbolRecord* value) {
    // Creates a pointer to a new hash table item
    St_item* item = (St_item*) malloc (sizeof(St_item));
    item->key = (char*) calloc (strlen(key) + 1, sizeof(char));
    //item->value = (char*) calloc (strlen(value) + 1, sizeof(char));
    
    strcpy(item->key, key);
    item->value = value;
    //strcpy(item->value, value);

    return item;
}

SymbolHashTable* create_symtable(int size, unsigned long (*hash_fun)(char*)) {
    // Creates a new SymbolHashTable
    SymbolHashTable* table = (SymbolHashTable*) malloc (sizeof(SymbolHashTable));
    table->hash_function = hash_fun;
    table->size = size;
    table->count = 0;
    table->items = (St_item**) calloc (table->size, sizeof(St_item*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

void free_symitem(St_item* item) {
    // Frees an item
    if (!item)
        return;
    if (item->key)
        free(item->key);
    if (item->value) {
        if (item->value->var_name) free(item->value->var_name);
        if (item->value->fun_name) free(item->value->fun_name);
        if (item->value->module_name) free(item->value->module_name);
        if (item->value->const_value) free(item->value->const_value);
        free(item->value);
    }
    free(item);
}

void free_symrecord(SymbolRecord* record) {
    if (!record)
        return;
    if (record->var_name) free(record->var_name);
    if (record->fun_name) free(record->fun_name);
    if (record->module_name) free(record->module_name);
    if (record->const_value) free(record->const_value);
    free(record);
}

void free_symtable(SymbolHashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        St_item* item = table->items[i];
        if (item != NULL)
            free_symitem(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision(SymbolHashTable* table, unsigned long index, St_item* item) {
    SymbolLinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

SymbolHashTable* st_insert(SymbolHashTable* table, char* key, SymbolRecord* value) {
    //printf("Inserting %s : %s\n", item->key, get_string_from_SymbolRecord*(item->value));

    // Compute the index
    int index = table->hash_function(key);

    St_item* current_item = table->items[index];
    
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Avoid this free, since our value is now a pointer to a struct!!!
            // free_symitem(item);
            return table;
        }
        
        // Insert directly
        St_item* item = create_symitem(key, value);
        table->items[index] = item; 
        //printf("Inserting %s : %s\n", table->items[index]->key, get_string_from_SymbolRecord*(table->items[index]->value));
        table->count++;
    }

    else {
            // Scenario 1: We only need to update value
            if (strcmp(current_item->key, key) == 0) {
                //free(table->items[index]->value);
                //table->items[index]->value = (char*) calloc (strlen(value) + 1, sizeof(char));
                //strcpy(table->items[index]->value, value);
                if (table->items[index]->value)
                    free_symrecord(table->items[index]->value);
                table->items[index]->value = value;
                // Avoid this free, since our value is now a pointer to a struct!!!
                // free_symitem(item);
                return table;
            }
    
        else {
            // Scenario 2: Collision
            // Create the item
            St_item* item = create_symitem(key, value);
            handle_collision(table, index, item);
            return table;
        }
    }
    return table;
}

SymbolRecord* st_search(SymbolHashTable* table, char* key) {
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = table->hash_function(key);
    St_item* item = table->items[index];
    SymbolLinkedList* head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}

void st_delete(SymbolHashTable* table, char* key) {
    // Deletes an item from the table
    int index = table->hash_function(key);
    St_item* item = table->items[index];
    SymbolLinkedList* head = table->overflow_buckets[index];

    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && strcmp(item->key, key) == 0) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_symitem(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (strcmp(item->key, key) == 0) {
                // Remove this item and set the head of the list
                // as the new item
                
                free_symitem(item);
                SymbolLinkedList* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_symitem(node->item->key, node->item->value);
                node->item->key = NULL;
                node->item->value = NULL;
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            SymbolLinkedList* curr = head;
            SymbolLinkedList* prev = NULL;
            
            while (curr) {
                if (strcmp(curr->item->key, key) == 0) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }

        }
    }

}

void print_symrecord(SymbolRecord* t, char ch) {
    // Prints the SymbolRecord* type
    if (t->var_name)
        printf("var_name = %s , ", t->var_name);
    printf("Type Name = %s , ", get_string_from_type(t->type_name));
    if (t->fun_name)
        printf("fun_name = %s , ", t->fun_name);
    if (t->module_name)
        printf("module_name = %s , ", t->module_name);
    if (t->const_value) {
        switch(t->type_name) {
            case TYPE_INTEGER:
                printf("const_value = %d , ", atoi(t->const_value));
                break;
            case TYPE_BOOLEAN:
                printf("const_value = %s , ", t->const_value);
                break;
            case TYPE_REAL:
                printf("const_value = %.4f , ", atof(t->const_value));
            case TYPE_NONE:
                printf("const_value = %s , ", t->const_value);
            default:
                printf("Not Yet Implemented ");
                break;
        }       
    }
    printf("scope_label = %d , ", t->scope_label);
    printf("total_size = %d , ", t->total_size);
    printf("offset = %d%c", t->total_size, ch);
}

void print_search_symtable(SymbolHashTable* table, char* key) {
    SymbolRecord* val;
    if ((val = st_search(table, key)) == NULL) {
        printf("%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Value:", key);
        print_symrecord(val, '\n');
    }
}

void print_symtable(SymbolHashTable* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:", i, table->items[i]->key);
            print_symrecord(table->items[i]->value, '\0');
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                SymbolLinkedList* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%s, Value:" , head->item->key);
                    print_symrecord(head->item->value, ' ');
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}
