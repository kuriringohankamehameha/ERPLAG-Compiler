// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define BUF_SIZE 4096
#define NUM_KEYWORDS 33

enum bool { false = 0, true = ~0 };

typedef enum bool bool;


// Define the Keyword here
typedef struct Keyword Keyword;

struct Keyword {
    char key[13];
    term tid;
};

// Complete list of keywords here
// ASSUMPTION: No Keyword has a length exceeding 20 characters
Keyword keywords[] = {
    {"AND", TK_AND},
    {"FALSE", TK_FALSE},
    {"OR", TK_OR},
    {"TRUE", TK_TRUE},
    {"array", TK_ARRAY},
    {"boolean", TK_BOOLEAN},
    {"break", TK_BREAK},
    {"case", TK_CASE},
    {"declare", TK_DECLARE},
    {"default", TK_DEFAULT},
    {"driver", TK_DRIVER},
    {"end", TK_END},
    {"for", TK_FOR},
    {"get_value", TK_GET_VALUE},
    {"in", TK_IN},
    {"input", TK_INPUT},
    {"integer", TK_INTEGER},
    {"module", TK_MODULE},
    {"of", TK_OF},
    {"parameters", TK_PARAMETERS},
    {"print", TK_PRINT},
    {"program", TK_PROGRAM},
    {"real", TK_REAL},
    {"record", TK_RECORD},
    {"returns", TK_RETURNS},
    {"start", TK_START},
    {"switch", TK_SWITCH},
    {"tagged", TK_TAGGED},
    {"takes", TK_TAKES},
    {"union", TK_UNION},
    {"use", TK_USE},
    {"while", TK_WHILE},
    {"with", TK_WITH},
};


typedef enum {
    LEX_UNRECOGNISED_CHAR,
    LEX_ID_OVERFLOW,
    LEX_NONE,
    LEX_UNRECOGNISED_EXPONENT,
    LEX_UNRECOGNISED_REAL_NUMBER,
    LEX_UNRECOGNISED_EQUAL,
    LEX_UNRECOGNISED_NOT,
    LEX_INVALID_DECIMAL_POINT,
}LexerError;

typedef struct Token Token;

// Define the Token here
struct Token {
    // A Token consists of a token name (terminal), a lexeme,
    // followed by pointers to the previous
    // and next tokens in the line to be tokenized

    term token_type;
    void* lexeme;
    int line_no;
    //Token* prev, *next;
};


// Define the TokenStream Linked List here
typedef struct TokenStream TokenStream;

struct TokenStream {
    Token token;
    TokenStream* prev, *next;
};

// Declare all Global Variables here
FILE* fp; // The file pointer to the source code file
int line_no; // For Detecting errors
int num_tokens; // To keep track of the number of tokens tokenized
char* buffer1, *buffer2; // A Double Buffer scheme
int lexeme_size; // Keep track of the lexeme size
char* curr_lexeme; // Current lexeme
char* look_ahead; // Pointers to look at the lexemes
bool reload_buffer1 = false; // To signal reloading the buffer1
bool reload_buffer2 = false; // To signal reloadiing the buffer2
TokenStream* first_tk, *last_tk; // Pointers for the first and last tokenstream elements
HashTable* ht; // Pointer to the Keyword Hashtable
unsigned long (*hash_fun)(char*); // Function Pointer to the Hash Function
