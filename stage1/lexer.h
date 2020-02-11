// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

// Declare all function prototypes here
TokenStream* create_token_stream();
void insert_token_stream(Token);
void free_token(TokenStream*);
void free_token_stream();
void init_tokenizer();
void close_tokenizer();
char get_char();
void unget_char(int);
void print_buffers();
void print_token_stream();
void print_token_type(Token);
void print_lexical_error(char, LexerError);
char* get_lexeme();
void dfa_signal();
Token get_next_token();
term is_keyword(char*);
void run_tokenizer(char*);
