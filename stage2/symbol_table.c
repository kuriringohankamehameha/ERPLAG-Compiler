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
        default:
        return "Not Yet Implemented";
    }
    return NULL;
}

SymbolTable* createSymbolTable(ASTNode* root) {
    // Creates a Symbol Table
    SymbolTable* symboltable = (SymbolTable*) calloc (1, sizeof(SymbolTable));
    return symboltable;
}

unsigned long hash_function(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
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
        free(temp->item->key);
        //free(temp->item->value);
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


St_item* create_symitem(char* key, term value) {
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
    free(item->key);
    //free(item->value);
    free(item);
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

SymbolHashTable* st_insert(SymbolHashTable* table, char* key, term value) {
    // Create the item
    St_item* item = create_symitem(key, value);
    //printf("Inserting %s : %s\n", item->key, get_string_from_term(item->value));

    // Compute the index
    int index = table->hash_function(key);

    St_item* current_item = table->items[index];
    
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_symitem(item);
            return table;
        }
        
        // Insert directly
        table->items[index] = item; 
        //printf("Inserting %s : %s\n", table->items[index]->key, get_string_from_term(table->items[index]->value));
        table->count++;
    }

    else {
            // Scenario 1: We only need to update value
            if (strcmp(current_item->key, key) == 0) {
                //free(table->items[index]->value);
                //table->items[index]->value = (char*) calloc (strlen(value) + 1, sizeof(char));
                //strcpy(table->items[index]->value, value);
                table->items[index]->value = value;
                free_symitem(item);
                return table;
            }
    
        else {
            // Scenario 2: Collision
            handle_collision(table, index, item);
            return table;
        }
    }
    return table;
}

term st_search(SymbolHashTable* table, char* key) {
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
            return TK_NONE;
        item = head->item;
        head = head->next;
    }
    return TK_NONE;
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

void print_term_type_symtable(term t, char ch) {
    // Prints the term type
    switch(t) {
        case TK_NUM:
        printf("TK_NUM%c", ch);
        break;
        case TK_RNUM:
        printf("TK_RNUM%c", ch);
        break;
        case TK_BOOLEAN:
        printf("TK_BOOLEAN%c", ch);
        break;
        case TK_OF:
        printf("TK_OF%c", ch);
        break;
        case TK_ARRAY:
        printf("TK_ARRAY%c", ch);
        break;
        case TK_START:
        printf("TK_START%c", ch);
        break;
        case TK_END:
        printf("TK_END%c", ch);
        break;
        case TK_DECLARE:
        printf("TK_DECLARE%c", ch);
        break;
        case TK_MODULE:
        printf("TK_MODULE%c", ch);
        break;
        case TK_DRIVER:
        printf("TK_DRIVER%c", ch);
        break;
        case TK_PROGRAM:
        printf("TK_PROGRAM%c", ch);
        break;
        case TK_RECORD:
        printf("TK_RECORD%c", ch);
        break;
        case TK_TAGGED:
        printf("TK_TAGGED%c", ch);
        break;
        case TK_UNION:
        printf("TK_UNION%c", ch);
        break;
        case TK_GET_VALUE:
        printf("TK_GET_VALUE%c", ch);
        break;
        case TK_PRINT:
        printf("TK_PRINT%c", ch);
        break;
        case TK_USE:
        printf("TK_USE%c", ch);
        break;
        case TK_WITH:
        printf("TK_WITH%c", ch);
        break;
        case TK_PARAMETERS:
        printf("TK_PARAMETERS%c", ch);
        break;
        case TK_TRUE:
        printf("TK_TRUE%c", ch);
        break;
        case TK_FALSE:
        printf("TK_FALSE%c", ch);
        break;
        case TK_TAKES:
        printf("TK_TAKES%c", ch);
        break;
        case TK_INPUT:
        printf("TK_INPUT%c", ch);
        break;
        case TK_RETURNS:
        printf("TK_RETURNS%c", ch);
        break;
        case TK_AND:
        printf("TK_AND%c", ch);
        break;
        case TK_OR:
        printf("TK_OR%c", ch);
        break;
        case TK_FOR:
        printf("TK_FOR%c", ch);
        break;
        case TK_IN:
        printf("TK_IN%c", ch);
        break;
        case TK_SWITCH:
        printf("TK_SWITCH%c", ch);
        break;
        case TK_CASE:
        printf("TK_CASE%c", ch);
        break;
        case TK_BREAK:
        printf("TK_BREAK%c", ch);
        break;
        case TK_DEFAULT:
        printf("TK_DEFAULT%c", ch);
        break;
        case TK_WHILE:
        printf("TK_WHILE%c", ch);
        break;
        case TK_PLUS:
        printf("TK_PLUS%c", ch);
        break;
        case TK_MINUS:
        printf("TK_MINUS%c", ch);
        break;
        case TK_MUL:
        printf("TK_MUL%c", ch);
        break;
        case TK_DIV:
        printf("TK_DIV%c", ch);
        break;
        case TK_LT:
        printf("TK_LT%c", ch);
        break;
        case TK_LE:
        printf("TK_LE%c", ch);
        break;
        case TK_GE:
        printf("TK_GE%c", ch);
        break;
        case TK_GT:
        printf("TK_GT%c", ch);
        break;
        case TK_EQ:
        printf("TK_EQ%c", ch);
        break;
        case TK_NE:
        printf("TK_NE%c", ch);
        break;
        case TK_DEF:
        printf("TK_DEF%c", ch);
        break;
        case TK_ENDDEF:
        printf("TK_ENDDEF%c", ch);
        break;
        case TK_COLON:
        printf("TK_COLON%c", ch);
        break;
        case TK_RANGEOP:
        printf("TK_RANGEOP%c", ch);
        break;
        case TK_SEMICOL:
        printf("TK_SEMICOL%c", ch);
        break;
        case TK_COMMA:
        printf("TK_COMMA%c", ch);
        break;
        case TK_ASSIGNOP:
        printf("TK_ASSIGNOP%c", ch);
        break;
        case TK_SQBO:
        printf("TK_SQBO%c", ch);
        break;
        case TK_SQBC:
        printf("TK_SQBC%c", ch);
        break;
        case TK_BO:
        printf("TK_BO%c", ch);
        break;
        case TK_BC:
        printf("TK_BC%c", ch);
        break;
        case TK_COMMENTMARK:
        printf("TK_COMMENTMARK%c", ch);
        break;
        case TK_ID:
        printf("TK_ID%c", ch);
        break;
        case TK_EPSILON:
        printf("TK_EPSILON%c", ch);
        break;
        case TK_EOF:
        printf("TK_EOF%c", ch);
        break;
        case TK_ERROR:
        printf("TK_ERROR%c", ch);
        break;
        case TK_NONE:
        printf("TK_NONE%c", ch);
        break;
        case TK_INTEGER:
        printf("TK_INTEGER%c", ch);
        break;
        case TK_REAL:
        printf("TK_REAL%c", ch);
        break;
        default:
        break;
    }
}

void print_search_symtable(SymbolHashTable* table, char* key) {
    term val;
    if ((val = st_search(table, key)) == TK_NONE) {
        printf("%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Value:", key);
        print_term_type_symtable(val, '\n');
    }
}

void print_symtable(SymbolHashTable* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:", i, table->items[i]->key);
            print_term_type_symtable(table->items[i]->value, '\0');
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                SymbolLinkedList* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%s, Value:" , head->item->key);
                    print_term_type_symtable(head->item->value, ' ');
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}
