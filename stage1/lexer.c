#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

#define BUF_SIZE 4096
#define NUM_KEYWORDS 33


enum bool { false = 0, true = ~0 };

typedef enum bool bool;

// Define the Terminals here

typedef enum {
    TK_NUM,
    TK_RNUM,
    TK_BOOLEAN,
    TK_OF,
    TK_ARRAY,
    TK_START,
    TK_END,
    TK_DECLARE,
    TK_MODULE,
    TK_DRIVER,
    TK_PROGRAM,
    TK_RECORD,
    TK_TAGGED,
    TK_UNION,
    TK_GET_VALUE,
    TK_PRINT,
    TK_USE,
    TK_WITH,
    TK_PARAMETERS,
    TK_TRUE,
    TK_FALSE,
    TK_TAKES,
    TK_INPUT,
    TK_RETURNS,
    TK_AND,
    TK_OR,
    TK_FOR,
    TK_IN,
    TK_SWITCH,
    TK_CASE,
    TK_BREAK,
    TK_DEFAULT,
    TK_WHILE,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_LT,
    TK_LE,
    TK_GE,
    TK_GT,
    TK_EQ,
    TK_NE,
    TK_DEF,
    TK_ENDDEF,
    TK_COLON,
    TK_RANGEOP,
    TK_SEMICOL,
    TK_COMMA,
    TK_ASSIGNOP,
    TK_SQBO,
    TK_SQBC,
    TK_BO,
    TK_BC,
    TK_COMMENTMARK,
    TK_ID,
    TK_EPSILON,
    $,
    TK_EOF,
    TK_ERROR,
    TK_NONE,
    TK_NOT_KEYWORD,
}term;


// Define the Keyword here
typedef struct Keyword Keyword;

struct Keyword {
    char key[13];
    term tid;
};

// Complete list of keywords here
// ASSUMPTION: No Keyword has a length exceeding 20 characters
Keyword keywords[] = {
    {"and", TK_AND},
    {"array", TK_ARRAY},
    {"boolean", TK_BOOLEAN},
    {"break", TK_BREAK},
    {"case", TK_CASE},
    {"declare", TK_DECLARE},
    {"default", TK_DEFAULT},
    {"driver", TK_DRIVER},
    {"end", TK_END},
    {"false", TK_FALSE},
    {"for", TK_FOR},
    {"get_value", TK_GET_VALUE},
    {"in", TK_IN},
    {"input", TK_INPUT},
    {"integer", TK_NUM},
    {"module", TK_MODULE},
    {"of", TK_OF},
    {"or", TK_OR},
    {"parameters", TK_PARAMETERS},
    {"print", TK_PRINT},
    {"program", TK_PROGRAM},
    {"real", TK_RNUM},
    {"record", TK_RECORD},
    {"returns", TK_RETURNS},
    {"start", TK_START},
    {"switch", TK_SWITCH},
    {"tagged", TK_TAGGED},
    {"takes", TK_TAKES},
    {"true", TK_TRUE},
    {"union", TK_UNION},
    {"use", TK_USE},
    {"while", TK_WHILE},
    {"with", TK_WITH},
};

typedef enum {
    LEXEME_INT,
    LEXEME_FLOAT,
    LEXEME_CHAR,
    LEXEME_STRING,
    LEXEME_ERROR,
    LEXEME_NONE
}LexemeType;


typedef enum {
    LEX_UNRECOGNISED,
    LEX_ID_OVERFLOW,
    LEX_NONE,
    LEX_UNRECOGNISED_EXPONENT,
    LEX_UNRECOGNISED_REAL_NUMBER,
    LEX_UNRECOGNISED_EQUAL,
}LexerError;

typedef struct Token Token;

// Define the Token here
struct Token {
    // A Token consists of a token name (terminal), a lexeme,
    // and the length, followed by pointers to the previous
    // and next tokens in the line to be tokenized

    term token_type;
    void* lexeme;
    int line_no;
    Token* prev, *next;
};


// Declare all Global Variables here
FILE* fp; // The file pointer to the source code file
int line_no; // For Detecting errors
char* buffer1, *buffer2; // A Double Buffer scheme
int lexeme_size; // Keep track of the lexeme size
char* curr_lexeme; // Current lexeme
char* look_ahead; // Pointers to look at the lexemes
bool reload_buffer1 = false; // To signal reloading the buffer1
bool reload_buffer2 = false; // To signal reloadiing the buffer2

void init_tokenizer(char* filename) {
    // Initializes the tokenizer
	fp = fopen(filename, "r");
	if (!fp) {
		perror("ERPLAG Error");
		exit(EXIT_FAILURE);
	}
    line_no = 1;
    buffer1 = (char*) calloc (BUF_SIZE+1, sizeof(char));
    buffer2 = (char*) calloc (BUF_SIZE+1, sizeof(char));
    curr_lexeme = look_ahead = NULL;
    lexeme_size = 0;
    reload_buffer1 = reload_buffer2 = true;
}

void close_tokenizer() {
    // Frees the buffers and closes
    // the file pointer
    free(buffer1);
    free(buffer2);
	fclose(fp);
}

char get_char() {
    // Gets the next character from the file
    // which is indexed by the lookahead pointer
    if (!curr_lexeme && !look_ahead) {
        // None of the buffers are loaded.
        // Read into the first buffer
        size_t bytes_read = fread(buffer1, sizeof(char), BUF_SIZE, fp);
        buffer1[bytes_read] = EOF;
        curr_lexeme = look_ahead = buffer1;   
    }

    char current_char = *look_ahead;

    if (look_ahead == buffer1 + BUF_SIZE - 1) {
        // End of buffer1. Get from buffer2

        // The problem with this is that since the lookahead pointer is
        // being pushed back, we cannot reload the buffers until the
        // unget_char() operation is complete. So we must wait for the
        // DFA to signal that the buffers are ready to be reloaded.
        look_ahead = buffer2;
        if (reload_buffer2) {
            size_t bytes_read = fread(buffer2, sizeof(char), BUF_SIZE, fp);
            buffer2[bytes_read] = EOF;
        }
    }

    else if (look_ahead == buffer2 + BUF_SIZE - 1) {
        // End of buffer2. Get from buffer1
        look_ahead = buffer1;
        if (reload_buffer1) {
            size_t bytes_read = fread(buffer1, sizeof(char), BUF_SIZE, fp);
            buffer1[bytes_read] = EOF;
        }
    }

    else {
        // Keep reading from the current buffer
        look_ahead ++;
    }

    lexeme_size ++;
    return current_char;
}

void unget_char(int num_characters) {
    // Shifts back the lookahead pointer on the input buffers

    if (look_ahead >= buffer1 && look_ahead <= buffer1 + BUF_SIZE -1) {
        // If the look_ahead pointer currently lies in the first buffer

        // And if all characters to be pushed still lies in the buffer
        if (buffer1 + num_characters <= look_ahead) {
            look_ahead = look_ahead - num_characters;
        }
        else {
            // The string also lies in buffer2
            // Shift by the difference in old position and buffer1
            char diff_position = look_ahead - buffer1;
            num_characters = num_characters - diff_position;

            // Move to the end of buffer2 and don't allow reloading on both buffers
            reload_buffer1 = false;
            reload_buffer2 = false;
            look_ahead = buffer2 + BUF_SIZE;
            look_ahead = look_ahead - num_characters;
        }
    }
    else {
        // Similar case for buffer2
        if (buffer2 + num_characters <= look_ahead) {
            look_ahead = look_ahead - num_characters;
        }
        else {
            // The string also lies in buffer1
            // Shift by the difference in old position and buffer2
            char diff_position = look_ahead - buffer2;
            num_characters = num_characters - diff_position;

            // Move to the end of buffer1 and don't allow reloading on both buffers
            reload_buffer1 = false;
            reload_buffer2 = false;
            look_ahead = buffer1 + BUF_SIZE;
            look_ahead = look_ahead - num_characters;
        }
    }

    lexeme_size -= num_characters;
}

void print_buffers() {
    printf("Buffer 1: \n");
    for (int i=0; i<BUF_SIZE; i++) {
        printf("%c", buffer1[i]);
    }

    printf("\nBuffer 2: \n");
    for (int i=0; i<BUF_SIZE; i++) {
        printf("%c", buffer2[i]);
    }
    printf("\n");
}

void print_error(char ch, LexerError err) {
    switch(err) {
        case LEX_UNRECOGNISED:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognized character: %c\n", line_no, ch);
            break;

        case LEX_ID_OVERFLOW:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Identifier length exceeded (>20)\n", line_no);
            break;

        case LEX_UNRECOGNISED_EXPONENT:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Exponent Symbol\n", line_no);
            break;

        case LEX_UNRECOGNISED_REAL_NUMBER:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Real Number\n", line_no);
            break;

        case LEX_UNRECOGNISED_EQUAL:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Character after equal to sign (=)\n", line_no);
            break;
    }
}

char* get_lexeme() {
    // Returns the lexeme corresponding to the current token
    char* lexeme = (char*) calloc (lexeme_size + 1, sizeof(char));
    char* temp = curr_lexeme;
    int curr = 0;
    while (temp != look_ahead) {
        // Set the lexeme value
        lexeme[curr++] = *temp;
        if (temp == buffer1 + BUF_SIZE - 1) {
            // End of buffer1. Start from buffer2
            temp = buffer2;
        }
        else if (temp == buffer2 + BUF_SIZE - 1) {
            temp = buffer1;
        }
        else
            temp ++;
    }
    // Null terminated lexeme
    lexeme[curr] = '\0';
    return lexeme;
}

term is_keyword(char* lexeme) {
    // Checks if the lexeme is a keyword or not
    // by performing a Binary Search on the Keyword Table
    if (!lexeme) {
        fprintf(stderr, "ERPLAG Error: Lexeme Corrupted\n");
        exit(EXIT_FAILURE);
    }

    int start = 0;
    int mid = NUM_KEYWORDS / 2;
    int end = NUM_KEYWORDS - 1;

    while (start <= end) {
        int val = strcmp(lexeme, keywords[mid].key);
        if (val < 0) {
            end = mid-1;
            mid = (start + end) / 2;
        }
        else if (val > 0) {
            start = mid+1;
            mid = (start + end) / 2;
        }
        else {
            return keywords[mid].tid;
        }
    }

    return TK_NOT_KEYWORD;
}

void dfa_signal() {
    // Gives a signal to reload both buffers
    // for the next get_char() and updates the current
    // lexeme pointer
    reload_buffer1 = true;
    reload_buffer2 = true;
    curr_lexeme = look_ahead;
    lexeme_size = 0;
}


Token DFA() {
    // DFA to capture the tokenizer states
    
    // Do we need to capture both the lexeme
    // string and it's value for integers and
    // floating point numbers?
    int state = 0;

    Token token;
    token.token_type = TK_NONE;
    token.lexeme = NULL;

    char ch;
    LexerError err = LEX_NONE;

    // Identifier size must be < 20
    int id_size = 0;

    while (state >= 0) {
        switch(state) {
            case 0:
                ch = get_char();
                //printf("ch = %c\n", ch);
                if (ch == '\n') {
                    line_no ++;
                    // New Line. Update current lexeme to the lookahead
                    curr_lexeme = look_ahead;
                    // Set lexeme_size to 0
                    lexeme_size = 0;
                    state = 0;
                }
                
                else if (ch == ' ' || ch == '\t') {
                    // Move ahead to the look_ahead pointer
                    curr_lexeme = look_ahead;
                    // Set lexeme_size to 0
                    lexeme_size = 0;
                    state = 0;
                }

                else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
                    state = 1;
                else if (ch >= '0' && ch <= '9')
                    state = 3;
                else if (ch == '+')
                    state = 11;
                else if (ch == '-')
                    state = 12;
                else if (ch == '*')
                    state = 13;
                else if (ch == '<')
                    state = 18;
                else if (ch == '/')
                    state = 22;
                else if (ch == '[')
                    state = 23;
                else if (ch == ']')
                    state = 24;
                else if (ch == '>')
                    state = 25;
                else if (ch == '=')
                    state = 29;
                else if (ch == '!')
                    state = 31;
                else if (ch == ':')
                    state = 33;
                else if (ch == '.')
                    state = 36;
                else if (ch == '(')
                    state = 38;
                else if (ch == ')')
                    state = 39;
                else if (ch == ';')
                    state = 40;
                else if (ch == ',')
                    state = 41;
                else {
                    err = LEX_UNRECOGNISED;
                    state = -1;
                }
                break;
            case 1:
                // Get the next character
                ch = get_char();
                //printf("ch = %c\n", ch);
                if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_') {
                    id_size ++;
                    state = 1;
                }
                else {
                    // Some other character. We have found the identifier
                    // token. Go to state 2 and move the lookahead back by a character
                    state = 2;
                }

                if (id_size > 20) {
                    state = -1;
                    err = LEX_ID_OVERFLOW;
                }
                break;
            case 2:
                // Go back by one character and
                // get the lexeme for the identifier
                // to set the token value
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = is_keyword(token.lexeme);
                token.line_no = line_no;
                dfa_signal();
                return token;
                
                break;
            case 3:
                // Get the next character
                ch = get_char();
                if (ch >= '0' && ch <= '9') {
                    state = 3;
                }
                else if (ch == '.') {
                    state = 5;
                }
                else if (ch == 'e' || ch == 'E') {
                    state = 7;
                }
                else {
                    state = 4;
                }
                break;
            case 4:
                // Return the integer value
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = TK_NUM;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 5:
                ch = get_char();
                if (ch >= '0' && ch <= '9') {
                    state = 6;
                }
                else if (ch == '.'){
                    // For range op, we need to unget both the dots
                    unget_char(2);
                    token.lexeme = get_lexeme();
                    token.token_type = TK_NUM;
                    token.line_no = line_no;
                    dfa_signal();
                    return token;
                }
                else {
                    // Throw a lexical error
                    state = -1;
                    err = LEX_UNRECOGNISED_REAL_NUMBER;
                }
                break;
            case 6:
                ch = get_char();
                if (ch >= '0' && ch <= '9')
                    state = 6;
                else if (ch == 'e' || ch == 'E')
                    state = 7;
                else {
                    state = 10;
                }
                break;
            case 7:
                ch = get_char();
                if (ch >= '0' && ch <= '9') {
                    state = 9;
                }
                else if (ch == '+' || ch == '-') {
                    state = 8;
                }
                else {
                    // Are we simply supposed to throw a 
                    // lexical error, or must we unget_char() also ??
                    state = -1;
                    err = LEX_UNRECOGNISED_EXPONENT;
                }
                break;
            case 8:
                ch = get_char();
                if (ch >= '0' && ch <= '9')
                    state = 9;
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED_EXPONENT;
                }
                break;
            case 9:
                ch = get_char();
                if (ch >= '0' && ch <= '9')
                    state = 9;
                else {
                    state = 10;
                }
                break;
            case 10:
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = TK_RNUM;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 11:
                token.lexeme = get_lexeme();
                token.token_type = TK_PLUS;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 12:
                token.lexeme = get_lexeme();
                token.token_type = TK_MINUS;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 13:
                ch = get_char();
                if (ch == '*') {
                    state = 15;
                }
                else
                    state = 14;
                break;
            case 14:
                token.lexeme = get_lexeme();
                token.token_type = TK_MUL;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 15:
                ch = get_char();
                if (ch == '*')
                    state = 16;
                else
                    state = 15;
                break;
            case 16:
                ch = get_char();
                if (ch == '*')
                    state = 17;
                else
                    state = 16;
                break;
            case 17:
                // This is just a comment. Return nothing as a lexeme
                token.lexeme = NULL;
                token.token_type = TK_COMMENTMARK;
                token.line_no = -1; // -1 for comments
                dfa_signal();
                return token;
                break;
            case 18:
                ch = get_char();
                if (ch == '=')
                    state = 19;
                else if (ch == '<')
                    state = 20;
                else
                    state = 21;
                break;
            case 19:
                token.lexeme = get_lexeme();
                token.token_type = TK_LE;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 20:
                token.lexeme = get_lexeme();
                token.token_type = TK_DEF;
                token.line_no = line_no;
                dfa_signal();
                return token;          
                break;
            case 21:
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = TK_LT;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 22:
                token.lexeme = get_lexeme();
                token.token_type = TK_DIV;
                token.line_no = line_no;
                dfa_signal();
                return token;   
                break;
            case 23:
                token.lexeme = get_lexeme();
                token.token_type = TK_SQBO;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 24:
                token.lexeme = get_lexeme();
                token.token_type = TK_SQBC;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 25:
                ch = get_char();
                if (ch == '=')
                    state = 26;
                else if (ch == '>')
                    state = 27;
                else
                    state = 28;
                break;
            case 26:
                token.lexeme = get_lexeme();
                token.token_type = TK_GE;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 27:
                token.lexeme = get_lexeme();
                token.token_type = TK_ENDDEF;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 28:
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = TK_GT;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 29:
                ch = get_char();
                if (ch == '=')
                    state = 30;
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED_EQUAL;
                }
                break;
            case 30:
                token.lexeme = get_lexeme();
                token.token_type = TK_EQ;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 31:
                ch = get_char();
                if (ch == '=')
                    state = 32;
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED;
                }
                break;
            case 32:
                token.lexeme = get_lexeme();
                token.token_type = TK_NE;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 33:
                ch = get_char();
                if (ch == '=')
                    state = 35;
                else
                    state = 34;
                break;
            case 34:
                unget_char(1);
                token.lexeme = get_lexeme();
                token.token_type = TK_COLON;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 35:
                token.lexeme = get_lexeme();
                token.token_type = TK_ASSIGNOP;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 36:
                ch = get_char();
                if (ch == '.')
                    state = 37;
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED;
                }
                break;
            case 37:
                ch = get_char();
                if (ch == '.') {
                    token.lexeme = get_lexeme();
                    token.token_type = TK_RANGEOP;
                    token.line_no = line_no;
                    dfa_signal();
                    return token;
                }
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED;
                }
                break;
            case 38:
                token.lexeme = get_lexeme();
                token.token_type = TK_BO;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 39:
                token.lexeme = get_lexeme();
                token.token_type = TK_BC;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 40:
                token.lexeme = get_lexeme();
                token.token_type = TK_SEMICOL;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 41:
                token.lexeme = get_lexeme();
                token.token_type = TK_COMMA;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            default:
                break;

        }
    }

    print_error(ch, err);
    token.token_type = TK_ERROR;
    dfa_signal();
    return token;
}

int scanner() {
    // Scanner
    char ch;
    while (true) {
        ch = get_char();
        if (ch == EOF)
            break;
        if (ch == '\0')
            continue;
        printf("Next character is: %c\n", ch);
        DFA();
        //print_buffers();
    }

    return -1;
}

int main() {
    init_tokenizer("sample.txt");
    //scanner();
    for (int i=0; i<8; i++) {
        Token t = DFA();
        printf("Line no: %d\n", t.line_no);
        printf("Token: %s\n", (char*)t.lexeme);
        printf("Type: ");
        switch(t.token_type) {
            case TK_NUM:
            printf("TK_NUM\n");
            break;
            case TK_RNUM:
            printf("TK_RNUM\n");
            break;
            case TK_BOOLEAN:
            printf("TK_BOOLEAN\n");
            break;
            case TK_OF:
            printf("TK_OF\n");
            break;
            case TK_ARRAY:
            printf("TK_ARRAY\n");
            break;
            case TK_START:
            printf("TK_START\n");
            break;
            case TK_END:
            printf("TK_END\n");
            break;
            case TK_DECLARE:
            printf("TK_DECLARE\n");
            break;
            case TK_MODULE:
            printf("TK_MODULE\n");
            break;
            case TK_DRIVER:
            printf("TK_DRIVER\n");
            break;
            case TK_PROGRAM:
            printf("TK_PROGRAM\n");
            break;
            case TK_RECORD:
            printf("TK_RECORD\n");
            break;
            case TK_TAGGED:
            printf("TK_TAGGED\n");
            break;
            case TK_UNION:
            printf("TK_UNION\n");
            break;
            case TK_GET_VALUE:
            printf("TK_GET_VALUE\n");
            break;
            case TK_PRINT:
            printf("TK_PRINT\n");
            break;
            case TK_USE:
            printf("TK_USE\n");
            break;
            case TK_WITH:
            printf("TK_WITH\n");
            break;
            case TK_PARAMETERS:
            printf("TK_PARAMETERS\n");
            break;
            case TK_TRUE:
            printf("TK_TRUE\n");
            break;
            case TK_FALSE:
            printf("TK_FALSE\n");
            break;
            case TK_TAKES:
            printf("TK_TAKES\n");
            break;
            case TK_INPUT:
            printf("TK_INPUT\n");
            break;
            case TK_RETURNS:
            printf("TK_RETURNS\n");
            break;
            case TK_AND:
            printf("TK_AND\n");
            break;
            case TK_OR:
            printf("TK_OR\n");
            break;
            case TK_FOR:
            printf("TK_FOR\n");
            break;
            case TK_IN:
            printf("TK_IN\n");
            break;
            case TK_SWITCH:
            printf("TK_SWITCH\n");
            break;
            case TK_CASE:
            printf("TK_CASE\n");
            break;
            case TK_BREAK:
            printf("TK_BREAK\n");
            break;
            case TK_DEFAULT:
            printf("TK_DEFAULT\n");
            break;
            case TK_WHILE:
            printf("TK_WHILE\n");
            break;
            case TK_PLUS:
            printf("TK_PLUS\n");
            break;
            case TK_MINUS:
            printf("TK_MINUS\n");
            break;
            case TK_MUL:
            printf("TK_MUL\n");
            break;
            case TK_DIV:
            printf("TK_DIV\n");
            break;
            case TK_LT:
            printf("TK_LT\n");
            break;
            case TK_LE:
            printf("TK_LE\n");
            break;
            case TK_GE:
            printf("TK_GE\n");
            break;
            case TK_GT:
            printf("TK_GT\n");
            break;
            case TK_EQ:
            printf("TK_EQ\n");
            break;
            case TK_NE:
            printf("TK_NE\n");
            break;
            case TK_DEF:
            printf("TK_DEF\n");
            break;
            case TK_ENDDEF:
            printf("TK_ENDDEF\n");
            break;
            case TK_COLON:
            printf("TK_COLON\n");
            break;
            case TK_RANGEOP:
            printf("TK_RANGEOP\n");
            break;
            case TK_SEMICOL:
            printf("TK_SEMICOL\n");
            break;
            case TK_COMMA:
            printf("TK_COMMA\n");
            break;
            case TK_ASSIGNOP:
            printf("TK_ASSIGNOP\n");
            break;
            case TK_SQBO:
            printf("TK_SQBO\n");
            break;
            case TK_SQBC:
            printf("TK_SQBC\n");
            break;
            case TK_BO:
            printf("TK_BO\n");
            break;
            case TK_BC:
            printf("TK_BC\n");
            break;
            case TK_COMMENTMARK:
            printf("TK_COMMENTMARK\n");
            break;
            case TK_ID:
            printf("TK_ID\n");
            break;
            case TK_EPSILON:
            printf("TK_EPSILON\n");
            break;
            case $:
            printf("$\n");
            break;
            case TK_EOF:
            printf("TK_EOF\n");
            break;
            case TK_ERROR:
            printf("TK_ERROR\n");
            break;
            case TK_NONE:
            printf("TK_NONE\n");
            break;
            case TK_NOT_KEYWORD:
            printf("TK_NOT_KEYWORD\n");
            break;
        }
    }
    close_tokenizer();
    return 0;
}