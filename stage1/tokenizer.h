#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash_table.h"

#define BUF_SIZE 1 << 10
#define NUM_KEYWORDS 33


// Define the Terminals here
typedef enum {
    NUM, RNUM, BOOLEAN, OF, ARRAY, START, END, DECLARE, MODULE, DRIVER, PROGRAM, RECORD, TAGGED, UNION, GET_VALUE, PRINT, USE, WITH, PARAMETERS, TRUE, FALSE, TAKES, INPUT, RETURNS, AND, OR, FOR, IN, SWITCH, CASE, BREAK, DEFAULT, WHILE, PLUS, MINUS, MUL, DIV, LT, LE, GE, GT, EQ, NE, DEF, ENDDEF, COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, SQBO, SQBC, BO, BC, COMMENTMARK, ID, EPSILON, $, TK_EOF, TK_ERROR
}term;


// Define the Keyword here
typedef struct Keyword Keyword;

struct Keyword {
    char key[13];
    term tid;
};

// Complete list of keywords here
Keyword keywords[] = {
    {"and", AND},
    {"array", ARRAY},
    {"boolean", BOOLEAN},
    {"break", BREAK},
    {"case", CASE},
    {"declare", DECLARE},
    {"default", DEFAULT},
    {"driver", DRIVER},
    {"end", END},
    {"false", FALSE},
    {"for", FOR},
    {"get_value", GET_VALUE},
    {"in", IN},
    {"input", INPUT},
    {"integer", NUM},
    {"module", MODULE},
    {"of", OF},
    {"or", OR},
    {"parameters", PARAMETERS},
    {"print", PRINT},
    {"program", PROGRAM},
    {"real", RNUM},
    {"record", RECORD},
    {"returns", RETURNS},
    {"start", START},
    {"switch", SWITCH},
    {"tagged", TAGGED},
    {"takes", TAKES},
    {"true", TRUE},
    {"union", UNION},
    {"use", USE},
    {"while", WHILE},
    {"with", WITH},
};

typedef enum
{
    LEXEME_INT,
    LEXEME_FLOAT,
    LEXEME_CHAR,
    LEXEME_ERROR,
    LEXEME_NONE
}LexemeType;

typedef struct Token Token;

// Define the Token here
struct Token {
    // A Token consists of a token name (terminal), a lexeme,
    // and the length, followed by pointers to the previous
    // and next tokens in the line to be tokenized

    term t;
    void* lexeme;
    LexemeType lexeme_type;
    int line_no;
    Token* prev, *next;
};


// Declare all Global Variables here
int line_no = 1;
Token* first_token, *last_token;


Token* init_token() {
    // Initialize a token
    Token* token = (Token*) malloc (sizeof(token));
    token->lexeme = NULL;
    token->lexeme_type = LEXEME_NONE;
    token->line_no = line_no;

    token->prev = last_token;
    token->next = NULL;
    last_token->next = token;
    last_token = last_token->next;
    return token;
}

Token* insert_token(Token* stream, Token* token) {
    // Inserts the token into the stream
    if (stream == NULL) {
        first_token = token;
        last_token = token;
        return token;
    }

    token->prev = last_token;
    last_token->next = token;
    last_token = last_token->next;

    return stream;
}

void free_tokens(Token* token) {
    // Free the list of tokens
    Token* temp = first_token;
    if (!temp->next) {
        free(temp->lexeme);
        free(temp);
        first_token = last_token = NULL;
        return;
    }

    while (temp->next) {
        Token* node = temp;
        temp = temp->next;
        first_token = temp;
        node->next = NULL;
        free(node);
    }

    free(first_token);
    first_token = last_token = NULL;
    return;
}


char* read_line(FILE* file) {
    // Reads a line from the input file into a buffer
    // after removing whitespace characters
    if (!file) {
        perror("ERPLAG Error:");
        exit(EXIT_FAILURE);
    }

    char* buffer = NULL; // May need to free this??
    size_t bytes_read;
    size_t len = 0;

    // End of File reached
    if (feof(file))
        return "";

    if(!(bytes_read = getline(&buffer, &len, file))) {
        perror("ERPLAG Error:");
        exit(EXIT_FAILURE);
    }
    
    size_t curr = 0;

    if (bytes_read == 1) {
        return "\n";
    }

    // Parse out spaces, tabs
    while (curr < bytes_read-1 && !(buffer[curr] == ' ' || buffer[curr] == '\t')) {
        curr++;
    }

    if (buffer[curr] == ' ' || buffer[curr] == '\t') {
        buffer[curr] = '\0';
    }

    buffer[curr+1] = '\0';
    
    if (curr < bytes_read-1) {
        // Go back to the next token, since there is a space
        fseek(file, -(bytes_read - 1 - curr), SEEK_CUR);
    }

    return buffer;
}

Token* get_token_stream(Token* token_stream, char* buffer) {
    // Reads from the buffer into a token stream

    // Initialize the tokenstream head if NULL
    // and insert the tokens from the buffer
    if (!token_stream) {
        Token* head = init_token();

        return head;
    }

    Token* head = init_token();
    token->
    return NULL; 
}

void DFA(HashTable table, char* buffer) {

}
