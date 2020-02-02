#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_CAPACITY 31
#define WORD_SIZE 30

struct _HashBucket {
    char word[WORD_SIZE];
    struct _HashBucket* next;
};

struct _HashStore {
    char word[WORD_SIZE];
};

struct _HashtableEntry {
    char word[WORD_SIZE];
    bool is_occupied;
    struct _HashBucket* start;
};

typedef struct _HashBucket HashBucket;
typedef struct _HashStore HashStore;
typedef struct _HashtableEntry* Hashtable;

// Lexeme Definitions come here

typedef enum {
    INTEGER, REAL, BOOLEAN, OF, ARRAY, START, END, DECLARE, MODULE, DRIVER,
    PROGRAM, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TRUE_, FALSE_, TAKES,
    INPUT, RETURNS, AND, OR, FOR, IN, SWITCH, CASE, BREAK, DEFAULT, WHILE, 
    PLUS, MINUS, MUL, DIV, LT, LE, GE, GT, EQ, NE, DRIVERDEF, DRIVERENDDEF,
    DEF, ENDDEF, COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, SQBO, SQBC, BO, 
    BC, COMMENTMARK, ID, NUM, RNUM, $, e, null_point = -1    
}term;

struct tokenInfo {
    term t;
    // Value for each token could be an int, float or char
    union {
        int i;
        float f;
        char c[WORD_SIZE/2];
    }value;
    int line_no;
    struct tokenInfo* prev, *next;
};
