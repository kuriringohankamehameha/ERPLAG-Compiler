// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#ifndef LEXERDEF_H
#define LEXERDEF_H
#include "common.h"
#include "hash_table.h"

#define BUF_SIZE 4096
#define NUM_KEYWORDS 35

// Define the Keyword here
typedef struct Keyword Keyword;

struct Keyword {
    char key[13];
    term tid;
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
    char* lexeme;
    int line_no;
    //Token* prev, *next;
};


// Define the TokenStream Linked List here
typedef struct TokenStream TokenStream;

struct TokenStream {
    Token token;
    TokenStream* prev, *next;
};
#endif
