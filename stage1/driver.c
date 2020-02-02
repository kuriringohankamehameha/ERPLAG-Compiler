#include "hash_table.h"

unsigned long hash_func (char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

int main() {
    // Declare a function pointer to the Hash function
    unsigned long (*hash_fun)(char*);

    // Point to hash_func()
    hash_fun = &hash_func;

    HashTable* ht = create_table(5000, hash_fun);

    ht_insert(ht, "One", "This is the first string");
    ht_insert(ht, "Two", "This is the second string");
    ht_insert(ht, "One", "This is the first string part 2");
    ht_insert(ht, "Hel", "String 1");
    ht_insert(ht, "Cau", "String 2");
    
    print_search(ht, "One");
    print_search(ht, "Two");
    print_search(ht, "Hel");
    print_search(ht, "Cau");
    
    printf("\nFirst Time\n");
    print_hashtable(ht);
    
    ht_delete(ht, "Two");
    ht_delete(ht, "Cau");
    ht_insert(ht, "Cau", "String 2 updated");
    ht_delete(ht, "Hel");
    
    printf("\nSecond Time after deleting {Two, Cau}, and updating Cau\n");
    print_hashtable(ht);
    
    free_table(ht);
}