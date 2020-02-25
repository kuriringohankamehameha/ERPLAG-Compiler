// Group 42:
// R Vijay Krishna 2017A7PS0183P
// Rohit K 2017A7PS0177P

#include "common.h"
#include "hash_table.h"
#include "lexer.h"
#include "parser.h"
#include "parserDef.h"

unsigned long hash_func (char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

// Populate the Hash Table with Keywords, indexed by a hash function pointer
HashTable* populate_hash_table(HashTable* ht, Keyword* keywords, unsigned long (*hash_fun)(char*)) {
    ht = create_table(CAPACITY, hash_fun);
    for (int i=0; i < NUM_KEYWORDS; i++) {
        ht_insert(ht, keywords[i].key, keywords[i].tid);
    }
    return ht;
}


TokenStream* create_token_stream() {
    // Creates a token stream element
    TokenStream* tk_str = (TokenStream*) calloc (1, sizeof(TokenStream));
    tk_str->next = tk_str->prev = NULL;
    return tk_str;
}

void insert_token_stream(Token token) {
    // Inserts an element into the tokenstream
    TokenStream* curr = last_tk;
    TokenStream* tk = create_token_stream();
    tk->token = token;
    if (!last_tk) {
        first_tk = tk;
        last_tk = tk;
        return;
    }
    curr->next = tk;
    tk->prev = last_tk;
    last_tk = tk;
}

void delete_from_token_stream() {
    // Removes the last element from the tokenstream
    if (!last_tk)
        return;
    
    TokenStream* temp = last_tk->prev;
    
    if (!temp) {
        free_token(last_tk);
        first_tk = last_tk = NULL;
        return;
    }

    temp->next = NULL;
    last_tk->prev = NULL;
    free_token(last_tk);
    last_tk = temp;
    return;
}

void free_token(TokenStream* tk) {
    // Frees a tokenstream element
    free((tk->token).lexeme);
    tk->prev = tk->next = NULL;
    free(tk);
}

void free_token_stream() {
    // Frees the complete chain of tokenstream elements
    TokenStream* curr = last_tk;
    if (!curr) {
        return;
    }
    
    if (curr->prev == NULL) {
        free_token(curr);
        first_tk = last_tk = NULL;
        return;
    }

    curr = curr->prev;
    TokenStream* temp = last_tk;
    free_token(temp);
    last_tk = curr;
    free_token_stream();
}

void init_tokenizer(char* filename) {
    // Initializes the tokenizer
    fp = fopen(filename, "r");
    if (!fp) {
        perror("ERPLAG Error");
        exit(EXIT_FAILURE);
    }
    line_no = 1;
    num_tokens = 0;
    buffer1 = (char*) calloc (BUF_SIZE+1, sizeof(char));
    buffer2 = (char*) calloc (BUF_SIZE+1, sizeof(char));
    curr_lexeme = look_ahead = NULL;
    lexeme_size = 0;
    reload_buffer1 = reload_buffer2 = true;
    first_tk = last_tk = NULL;
    hash_fun = hash_func;
    // Hash Table of Keywords
    ht = populate_hash_table(ht, keywords, hash_fun); 
    //print_hashtable(ht);
}

void close_tokenizer() {
    // Frees the buffers and closes
    // the file pointer
    // along with the token stream
    free_token_stream();
    free(buffer1);
    free(buffer2);
    free_table(ht);
    fclose(fp);
}

FILE* get_stream(FILE* fp, int buffer_no) {
    // Reads the file into memory using a
    // fixed buffer size
    if (buffer_no == 1) {
        size_t bytes_read = fread(buffer1, sizeof(char), BUF_SIZE, fp);
        buffer1[bytes_read] = '\0';
    }
    else if (buffer_no == 2) {
        size_t bytes_read = fread(buffer2, sizeof(char), BUF_SIZE, fp);
        buffer2[bytes_read] = '\0';
    }
    else {
        fprintf(stderr, "Invalid Buffer Number. Only 2 are available\n");
        exit(EXIT_FAILURE);
    }
    return fp;
}

char get_char() {
    // Gets the next character from the file
    // which is indexed by the lookahead pointer
    if (!curr_lexeme && !look_ahead) {
        // None of the buffers are loaded.
        // Read into the first buffer
        fp = get_stream(fp, 1);
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
            // Load into the second buffer
            fp = get_stream(fp, 2);
        }
    }

    else if (look_ahead == buffer2 + BUF_SIZE - 1) {
        // End of buffer2. Get from buffer1
        look_ahead = buffer1;
        if (reload_buffer1) {
            fp = get_stream(fp, 1);
            //size_t bytes_read = fread(buffer1, sizeof(char), BUF_SIZE, fp);
            //buffer1[bytes_read] = '\0';
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
    // Prints the buffer elements
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

void print_token_stream() {
    // Prints the token stream elements
    printf("\nToken Stream:\n");
    TokenStream* temp = first_tk;
    while (temp) {
        printf("Token: %s, ", (char*)(temp->token).lexeme);
        print_token_type(temp->token);
        temp = temp->next;
    }
    printf("\n");
}

void print_lexical_error(char ch, LexerError err) {
    // Prints an error to the console on a Lexical Error
    switch(err) {
        case LEX_UNRECOGNISED_CHAR:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognized character: '%c'\n", line_no, ch);
            break;

        case LEX_ID_OVERFLOW:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Identifier length exceeded 20 characters\n", line_no);
            break;

        case LEX_UNRECOGNISED_EXPONENT:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Exponent Symbol: '%c'\n", line_no, ch);
            break;

        case LEX_UNRECOGNISED_REAL_NUMBER:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Real Number: '%c'\n", line_no, ch);
            break;

        case LEX_UNRECOGNISED_EQUAL:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Character after equal to sign (=): '%c'\n", line_no, ch);
            break;

        case LEX_UNRECOGNISED_NOT:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Unrecognised Character after NOT sign (!): '%c'\n", line_no, ch);
            break;

        case LEX_INVALID_DECIMAL_POINT:
            fprintf(stderr, "ERPLAG Lexical Error: (Line No %d) Decimal point must have a prefix digit.\n", line_no);

        default:
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
    // by performing a Lookup on the Keyword Hash Table
    if (!lexeme) {
        fprintf(stderr, "ERPLAG Error: Lexeme Corrupted\n");
        exit(EXIT_FAILURE);
    }

    // Search the Hashtable
    term tok = ht_search(ht, lexeme);

    // Return an identifier if it is not a keyword
    if (tok == TK_NONE)
        return TK_ID;
    else
        return tok;
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

Token get_next_token() {
    // DFA to capture the tokenizer states
    
    // Do we need to capture both the lexeme
    // string and it's value for integers and
    // floating point numbers?
    int state = 0;

    Token token;
    token.token_type = TK_NONE;
    token.lexeme = NULL;

    int ch;
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
                else if (ch == EOF || ch == '\0') {
                    state = 45;
                }
                else {
                    err = LEX_UNRECOGNISED_CHAR;
                    state = -1;
                }
                break;
            case 1:
                // Get the next character
                ch = get_char();
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
                    // and classify the number as an integer
                    state = 42;
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
                unget_char(1);
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
                else {
                    if (ch == '\n')
                        line_no++;
                    state = 15;
                }
                break;
            case 16:
                ch = get_char();
                if (ch == '*')
                    state = 17;
                else {
                    if (ch == '\n')
                        line_no++;
                    state = 15;
                }
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
                ch = get_char();
                if (ch == '<') {
                    state = 44;
                }
                else {
                    unget_char(1);
                    token.lexeme = get_lexeme();
                    token.token_type = TK_DEF;
                    token.line_no = line_no;
                    dfa_signal();
                    return token;          
                }
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
                ch = get_char();
                if (ch == '>') {
                    state = 43;
                }
                else {
                    unget_char(1);
                    token.lexeme = get_lexeme();
                    token.token_type = TK_ENDDEF;
                    token.line_no = line_no;
                    dfa_signal();
                    return token;
                }
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
                    err = LEX_UNRECOGNISED_NOT;
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
                else if (ch >= '0' && ch <= '9') {
                    state = -1;
                    err = LEX_INVALID_DECIMAL_POINT;
                }
                else {
                    state = -1;
                    err = LEX_UNRECOGNISED_CHAR;
                }
                break;
            case 37:
                token.lexeme = get_lexeme();
                token.token_type = TK_RANGEOP;
                token.line_no = line_no;
                dfa_signal();
                return token;
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
            case 42:
                unget_char(2);
                token.lexeme = get_lexeme();
                token.token_type = TK_NUM;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            case 43:
                token.lexeme = get_lexeme();
                token.token_type = TK_DRIVERENDDEF;
                token.line_no = line_no;
                dfa_signal();
                return token;
            case 44:
                token.lexeme = get_lexeme();
                token.token_type = TK_DRIVERDEF;
                token.line_no = line_no;
                dfa_signal();
                return token;
            case 45:
                token.lexeme = NULL;
                token.token_type = TK_EOF;
                token.line_no = line_no;
                dfa_signal();
                return token;
                break;
            default:
                break;

        }
    }

    print_lexical_error(ch, err);
    token.token_type = TK_ERROR;
    token.line_no = line_no;
    dfa_signal();
    return token;
}

void print_token_type(Token t) {
    // Prints the token type
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
        case TK_EOF:
        printf("TK_EOF\n");
        break;
        case TK_ERROR:
        printf("TK_ERROR\n");
        break;
        case TK_NONE:
        printf("TK_NONE\n");
        break;
        case TK_INTEGER:
        printf("TK_INTEGER\n");
        break;
        case TK_REAL:
        printf("TK_REAL\n");
        break;
        case TK_DRIVERDEF:
        printf("TK_DRIVERDEF\n");
        break;
        case TK_DRIVERENDDEF:
        printf("TK_DRIVERENDDEF\n");
        break;
        default:
        break;
    }
}

void remove_comments(char* testcase_file, char* clean_file) {
    // Processes a testcase file and rewrites it
    // into an output file after removing comments
    
    FILE* ip = fopen(testcase_file, "r");
    if (!ip) {
        fprintf(stderr, "Error opening %s\n", testcase_file);
        exit(EXIT_FAILURE);
    }

    FILE* op = fopen(clean_file, "w");
    if (!op) {
        fprintf(stderr, "Error opening %s\n", clean_file);
        fclose(ip);
        exit(EXIT_FAILURE);
    }

    // Use a separate buffer for this function
    char* buffer = (char*) calloc (BUF_SIZE, sizeof(char));
    
    // Are we under a comment?
    bool under_comment = false;
    
    do {
        // Keep reading until EOF or empty buffer
        size_t bytes_read = fread(buffer, sizeof(char), BUF_SIZE, ip);
        if (bytes_read == 0)
            break;
        buffer[bytes_read] = '\0';

        // Maintain index of current character in the buffer
        size_t curr = 0;
        

        while (curr < bytes_read) {
            // Traverse the buffer and remove comments
            if (under_comment) {
                // Ignore all characters until the next **
                for ( ; curr < bytes_read; curr++) {
                    
                    if (buffer[curr] == '\n') {
                        fprintf(op, "\n");
                    }

                    if (buffer[curr] == '*' && buffer[curr+1] == '*') {
                        // Encountered a comment. Increment the position
                        // by 2 and invert the flag
                        under_comment = ~(under_comment);
                        curr += 2;
                        break;
                    }
                }
            }

            for ( ; curr < bytes_read; curr++) {
                if (buffer[curr] == '*' && buffer[curr+1] == '*') {
                    // Encountered a comment. Increment the position
                    // by 2 and invert the flag
                    under_comment = ~(under_comment);
                    curr += 2;
                    break;
                }

                else {
                    // Don't print any character under a comment
                    // provided that it's not a newline character
                    if (under_comment && buffer[curr] != '\n')
                        continue;
                    else
                        fprintf(op, "%c", buffer[curr]);
                }
            }
        }
    } while (!feof(ip) && buffer[0] != '\0');

    // Close stuff
    free(buffer);
    fclose(ip);
    fclose(op);
}

void remove_comments_stdout(char* testcase_file) {
    // Processes a testcase file and rewrites it
    // into stdout
    
    FILE* ip = fopen(testcase_file, "r");
    if (!ip) {
        fprintf(stderr, "Error opening %s\n", testcase_file);
        exit(EXIT_FAILURE);
    }

    // Use a separate buffer for this function
    char* buffer = (char*) calloc (BUF_SIZE, sizeof(char));
    
    // Are we under a comment?
    bool under_comment = false;
    
    do {
        // Keep reading until EOF or empty buffer
        size_t bytes_read = fread(buffer, sizeof(char), BUF_SIZE, ip);
        if (bytes_read == 0)
            break;
        buffer[bytes_read] = '\0';

        // Maintain index of current character in the buffer
        size_t curr = 0;
        

        while (curr < bytes_read) {
            // Traverse the buffer and remove comments
            if (under_comment) {
                // Ignore all characters until the next **
                for ( ; curr < bytes_read; curr++) {
                    
                    if (buffer[curr] == '\n') {
                        fprintf(stdout, "\n");
                    }

                    if (buffer[curr] == '*' && buffer[curr+1] == '*') {
                        // Encountered a comment. Increment the position
                        // by 2 and invert the flag
                        under_comment = ~(under_comment);
                        curr += 2;
                        break;
                    }
                }
            }

            for ( ; curr < bytes_read; curr++) {
                if (buffer[curr] == '*' && buffer[curr+1] == '*') {
                    // Encountered a comment. Increment the position
                    // by 2 and invert the flag
                    under_comment = ~(under_comment);
                    curr += 2;
                    break;
                }

                else {
                    // Don't print any character under a comment
                    // provided that it's not a newline character
                    if (under_comment && buffer[curr] != '\n')
                        continue;
                    else
                        fprintf(stdout, "%c", buffer[curr]);
                }
            }
        }
    } while (!feof(ip) && buffer[0] != '\0');

    // Close stuff
    free(buffer);
    fclose(ip);
}

void run_tokenizer(char* filename) {
    // Runs the tokenizer on the input file
    init_tokenizer(filename);

    // Process the tokens using get_next_token()
    printf("----------------------------------------------\n");
    printf("Line No\tLexeme\tToken Name\n");
    for (;;) {
        Token t = get_next_token();
        if (t.token_type == TK_EOF)
            break;
        // Process token only if the lexeme exists.
        // This means that any TK_COMMENTMARK is avoided
        if (t.lexeme) {
            printf("%d\t", t.line_no);
            printf("%s\t\t", (char*)t.lexeme);
            print_token_type(t);
            printf("\n");
            // Insert the token into the token stream
            // insert_token_stream(t);
        }
        else {
            printf("%d\t", t.line_no);
            printf("NULL\t\t");
            print_token_type(t);
            printf("\n");
        }
    }
    printf("----------------------------------------------\n");

    // Prints the token stream
    // print_token_stream();
    // Close the tokenizer
    close_tokenizer();
}

Rule make_rule(int left, int* right, int num_right) {
    Rule grule;
    grule.left = left;
    grule.right = right;
    grule.num_right = num_right;
    return grule;
}

TreeNode* make_tree_node(TreeNode* parent, Token token) {
    TreeNode* node = (TreeNode*) calloc (1, sizeof(TreeNode));
    node->token = token;
    node->parent = parent;
    node->children = NULL;
    node->right_sibling = NULL;
    node->num_children = 0;
    node->rule_no = -1;
    node->check_term = is_terminal(token.token_type);
    return node;
}

void free_parse_tree(TreeNode* root) {
    if (!root)
        return;
    for (int i=0; i<root->num_children; i++) {
        free_parse_tree(root->children[i]);
    }
    free(root->token.lexeme);
    free(root);
}

TreeNode* add_tree_node(TreeNode* parent, Token token) {
    // Adds the token to the tree under parent
    if (!parent) {
        parent = make_tree_node(NULL, token); 
    }
    else {
        if (parent->num_children == 0) {
            parent->children = (TreeNode**) calloc (1, sizeof(TreeNode*));
            parent->children[parent->num_children] = make_tree_node(parent, token);
        }
        else {
            parent->children = (TreeNode**) realloc (parent->children, (parent->num_children + 1) * sizeof(TreeNode*));
            TreeNode* node = make_tree_node(parent, token);
            parent->children[parent->num_children] = node;
            parent->children[parent->num_children - 1]->right_sibling = node;
        }
        parent->num_children++;
    }
    return parent;
}

void printTreeNode(TreeNode* root) {
    if (!root)
        return;
    for (int i=0; i<root->num_children; i++) {
        printTreeNode(root->children[i]);
    }
    printf("Node: %s => ", get_string_from_term(root->token.token_type));
}

StackNode* make_stack_node(TreeNode* data) {
    StackNode* node = (StackNode*) calloc (1, sizeof(StackNode));
    node->data = data;
    node->is_dollar = (data->token.token_type == TK_DOLLAR);
    node->next = NULL;
    return node;
}

StackNode* push(StackNode* stack, TreeNode* data) {
    // Pushes the element into the stack and returns the updated head
    StackNode* node = make_stack_node(data);
    node->next = stack;
    stack = node;
    return stack;
}

StackNode* pop(StackNode* stack) {
    // Pops the element from the stack and returns the updated head
    if (!stack)
        return NULL;
    StackNode* node = stack->next;
    if (!node) {
        free(stack);
        stack = NULL;
    }
    else {
        StackNode* temp = stack;
        stack = node;
        temp->next = NULL;
        free(temp);
    }
    return stack;
}

StackNode* top(StackNode* stack) {
    return stack;
}

bool is_empty(StackNode* stack) {
    return stack == NULL;
}

void free_stack(StackNode* stack) {
    if (!stack)
        return;
    StackNode* temp = stack;
    stack = stack->next;
    temp->next = NULL;
    free(temp);
    free_stack(stack);
}

void free_tree(TreeNode* root) {
    if (!root)
        return;
    if (root->num_children == 0) {
        if (root->parent)
            root->parent->num_children--;
        root->parent = NULL;
        if (root->token.lexeme) {
            free(root->token.lexeme);
        }
        free(root);
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        free_tree(root->children[i]);
    }
}

void free_grammar_rule(Rule grule) {
    // Frees a grammar tule
    free(grule.right);
}

void free_grammar(Grammar g) {
    // Free the grammar structure pointed
    // by g
    for (int i=0; i < g.num_rules; i++) {
        free_grammar_rule(g.rules[i]);
    }
    free(g.rules);

    // Free the HashTable
    if (hash_table)
        free_table(hash_table);
}

void free_first_and_follow(FirstAndFollow f) {
    if (f.first) {
        for (int i=0; i<f.num_tokens + f.num_symbols + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (f.first[i])
                free(f.first[i]);
        }
        free(f.first);
    }
    if (f.follow) {
        for (int i=0; i<f.num_symbols + f.num_tokens + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (f.follow[i])
                free(f.follow[i]);
        }
        free(f.follow);
    }
}

void free_parse_table(ParseTable p) {
    for (int i=0; i<p.num_symbols + p.num_tokens + 1; i++) {
        if (p.matrix[i])
            free(p.matrix[i]);
    }
    free(p.matrix);
}

void print_rule(Rule grule) {
    // Prints a grammar rule
    printf("LHS : %s , RHS : ", get_string_from_term((term)grule.left));
    for (int i=0; i<grule.num_right; i++) {
        printf("%s , ", get_string_from_term((term)grule.right[i]));
    }
    printf("\n");
}

void print_grammar(Grammar g) {
    // Prints the grammar
    if (g.num_rules <= 0)
        return;
    printf("Grammar retrieved from file.\n-----------------------------\nStatistics:\n");
    printf("Number of Rules: %d\n", g.num_rules);
    printf("Number of Non Terminals: %d\n", g.num_symbols);
    printf("Number of Tokens: %d\n", g.num_tokens);
    for (int i=0; i<g.num_rules; i++) {
        print_rule(g.rules[i]);
    }
    printf("-----------------------------\n");
}

void printFirstAndFollowSets(FirstAndFollow f) {
    // Prints the first and follow sets
    printf("-----------------------------------------------------\n");
    printf("First Sets\n");
    for (int i=0; i<f.num_symbols + f.num_tokens; i++) {
        printf("Symbols: %s => ", get_string_from_term(i));
        for (int j=0; j<f.num_tokens; j++) {
            printf(",%s: %d ", get_string_from_term(j), f.first[i][j]);
        }
        printf("\n\n");
    }
    printf("-----------------------------------------------------\n");
    
    printf("Follow Sets\n");
    for (int i=0; i<f.num_symbols + f.num_tokens; i++) {
        printf("Symbols: %s => ", get_string_from_term(i));
        for (int j=0; j<f.num_tokens; j++) {
            printf(",%s: %d ", get_string_from_term(j), f.follow[i][j]);
        }
        // Include TK_DOLLAR in follow set
        printf(",%s: %d ", get_string_from_term(TK_DOLLAR), f.follow[i][TK_DOLLAR]);
        printf("\n\n");
    }

    printf("-----------------------------------------------------\n");
}

void printParseTable(ParseTable p) {
    // Prints the parse table
    // [SYMBOLS * TERMINALS]
    printf("-----------------------------------------------------\n");
    printf("Parse Table\n");
    for (int i=p.num_tokens; i<p.num_tokens + p.num_symbols; i++) {
        printf("Symbol : %s=>\t", get_string_from_term(i));
        for (int j=0; j<p.num_tokens; j++) {
            printf("<%s, %d> |", get_string_from_term(j), p.matrix[i][j]);
        }
        printf("\n\n");
    }
    printf("-----------------------------------------------------\n");
}

char* get_parser_token(Token t) {
    switch(t.token_type) {
        case TK_NUM:
        return "Integer";
        break;
        case TK_RNUM:
        return "Floating point number";
        break;
        case TK_BOOLEAN:
        return "Boolean";
        break;
        case TK_OF:
        return "Of";
        break;
        case TK_ARRAY:
        return "Array";
        break;
        case TK_START:
        return "start";
        break;
        case TK_END:
        return "end";
        break;
        case TK_DECLARE:
        return "declare";
        break;
        case TK_MODULE:
        return "module";
        break;
        case TK_DRIVER:
        return "driver";
        break;
        case TK_PROGRAM:
        return "program";
        break;
        case TK_GET_VALUE:
        return "get_value";
        break;
        case TK_PRINT:
        return "print";
        break;
        case TK_USE:
        return "use";
        break;
        case TK_TRUE:
        return "true";
        break;
        case TK_FALSE:
        return "false";
        break;
        case TK_TAKES:
        return "takes";
        break;
        case TK_SWITCH:
        return "switch";
        break;
        case TK_CASE:
        return "case";
        break;
        case TK_BREAK:
        return "break";
        break;
        case TK_DEFAULT:
        return "default";
        break;
        case TK_WHILE:
        return "while";
        break;
        case TK_COLON:
        return "colon";
        break;
        case TK_RANGEOP:
        return "range operator";
        break;
        case TK_SEMICOL:
        return ";";
        break;
        case TK_COMMA:
        return ",";
        break;
        case TK_ASSIGNOP:
        return ":=";
        break;
        case TK_SQBO:
        return "[";
        break;
        case TK_SQBC:
        return "]";
        break;
        case TK_BO:
        return "(";
        break;
        case TK_BC:
        return ")";
        break;
        case TK_ID:
        return "Identifier";
        break;
        default:
        break;
    }
    return get_string_from_term(t.token_type);
}

Grammar populate_grammar(FILE* fp) {
    Grammar G;
    G.rules = (Rule*) calloc (1, sizeof(Rule));
    G.rules[0].right = NULL;
    G.num_rules = G.num_symbols = G.num_tokens = 0;
    
    int num;
    char ch = '\0';
    
    int curr_right = 0;
    int curr_rule = -1;
    
    bool is_lhs = false;
    
    char buffer[256];
    
    unsigned long (*hash_fun)(char*);
    
    hash_fun = &hash_func;
    
    size_t num_symbols = sizeof(symbols)/sizeof(Symbol);
    hash_table = create_table(CAPACITY*3, hash_fun);
    
    for (uint32_t i=0; i<num_symbols; i++) {
        hash_table = ht_insert(hash_table, symbols[i].key, symbols[i].tid);
    }
    
    int* temp = (int*) calloc (1, sizeof(int));
    int total = 0;
    
    do {
    while (fscanf(fp, " %s", buffer) == 1) {
        //printf("Buffer = %s\n", buffer);
        if (strcmp(buffer, "$") == 0) {
        is_lhs = true;
        curr_rule++;
        G.num_rules++;
        curr_right = 0;
        continue;
        }
        else if (strcmp(buffer, "->") == 0) {
        is_lhs = false;
        continue;
        }
        else {
        num = ht_search(hash_table, buffer);
        if ((term)num == TK_NONE) {
            fprintf(stderr, "Symbol %s not in hash table\n", buffer);
            exit(EXIT_FAILURE);
        }
        
        bool flag = false;
        for (int i=0; i<total; i++) {
            if (temp[i] == num) {
            flag = true;
            break;
            }
        }
        if (!flag) {
            temp = (int*) realloc (temp, (total + 2) * sizeof(int));
            temp[total] = num;
            total++;
        }
                
        if (is_lhs) {
            // Belongs to the lhs
            //printf("LHS: curr_rule = %d, curr_right = %d\n", curr_rule, curr_right);
            G.rules = (Rule*) realloc (G.rules, (curr_rule + 1) * sizeof(Rule));
            G.rules[curr_rule].right = (int*) calloc ((curr_right + 2),  sizeof(int));
            //G.rules[curr_rule].right[curr_right + 1] = num;
            G.rules[curr_rule].rule_no = curr_rule; // Useless variable
            G.rules[curr_rule].left = num;
        }
        else {
            // Go to RHS
            //printf("RHS: curr_left = %d, curr_right = %d\n", curr_left, curr_right);
            G.rules = (Rule*) realloc (G.rules, (curr_rule + 1) * sizeof(Rule));
            G.rules[curr_rule].right = (int*) realloc (G.rules[curr_rule].right, (curr_right + 2) * sizeof(int));
            G.rules[curr_rule].num_right++;
            G.rules[curr_rule].right[curr_right] = num;
            curr_right++;
        }
        // Update the number of terminals / non-terminals
        if (!flag) {
            if (is_terminal(num)) {
            G.num_tokens++;
            }
            else {
            G.num_symbols++;
            }
        }
        }
        is_lhs = false;
    }
    } while (fscanf(fp, " %c", &ch) == 1);


    //G.rules[curr_rule].num_right = curr_right;
    G.num_tokens++;
    free(temp);
    
    return G;
}

int Union(int* a ,int* b, int length) {
    // Performs a union operation (OR) between
    // two sets a and b, indexed using an array
    //leave 0 epsilon
    int change = 0;
    for(int i=1; i<length; i++) {
        if(b[i] == 1) {
            if(a[i] == 1) {
                a[i] = 1;
                continue;
            }
            else {
                a[i] = 1;
                change = 1;
            }
        }
    }
    return change;
}

FirstAndFollow ComputeFirstAndFollowSets(Grammar G){
    // Compute the first and follow sets for the grammar
    FirstAndFollow F;
    
    // Keep an extra index for $ (TK_DOLLAR)
    // although we don't care about that for the first sets
    F.first = (int **)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int *)); 
    for (int i=0; i<G.num_symbols + G.num_tokens + 2; i++) 
         F.first[i] = (int *)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int));
    F.num_symbols = G.num_symbols;
    F.num_tokens = G.num_tokens;
    
    // Keep an extra index for $ (TK_DOLLAR)
    F.follow = (int **)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int *)); 
    for (int i=0; i<G.num_symbols+G.num_tokens+2; i++) 
         F.follow[i] = (int *)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int));
    
    
    // F.first
    int change;

    // Base Cases for Terminals 
    for(int i=0; i<G.num_symbols; i++) {
        if(is_terminal(i))
            F.first[i][i] = 1;
    }
    
    // A -> Epsilon
    for (int i=0; i<G.num_rules; i++) {
        if ((term) G.rules[i].right[0] == TK_EPSILON) {
            int nt_idx = G.rules[i].left;
            F.first[nt_idx][0] = 1;
        }
    }

    do{
        change=0;

        for(int i=0; i<G.num_rules; i++) {
            for(int j=0; j<G.rules[i].num_right; j++) {
                // If the current right symbol is a terminal
                if( is_terminal(G.rules[i].right[j]) ) {
                    // Add it to the first set and go to the next rule
                    int t_index = G.rules[i].right[j];
                    F.first[G.rules[i].left][t_index] = 1; 
                    // We don't update change, since we reached a terminal.
                    // This belongs to a base case, so this is acceptable
                    //change = 1;   
                    break;
                }

                // Perform an OR operation between 2 first sets, and update the first set
                // with the mask of a right symbol
                int temp = Union(F.first[G.rules[i].left], F.first[G.rules[i].right[j]], G.num_tokens);
                
                if(change != 1)
                    change = temp;
                
                // If we're at the last right symbol
                if(j == (G.rules[i].num_right)-1) {
                    // And if it's set contains Epsilon
                    if(F.first[G.rules[i].right[j]][0] == 1) {
                        // Then Epsilon must also belong to first(left symbol)
                        F.first[G.rules[i].left][0] = 1;
                        change = 1;
                        break;  
                    }
                }
                
                // Otherwise, if the current right symbol has Epsilon in it's
                // first set, don't include it for the left symbol
                else if(F.first[G.rules[i].right[j]][0] != 1)
                    break;
            }
        }

    } while (change == 1);
    
    // Follow sets only for non terminals
    // Set the follow for program to be $
    // Find a way to separately include $ for the
    // follow set, since it's not a normal terminal
    F.follow[(int)program][(int)TK_DOLLAR] = 1;
    
    change = 0;
    do {
        change = 0;
        for(int i=0; i< G.num_rules; i++) {
            for(int j=0; j<G.rules[i].num_right ; j++) {
                if (is_terminal(G.rules[i].right[j]))
                    continue;
                // If we're at the last right symbol of i
                if(j == G.rules[i].num_right-1) {
                    // Then the follow of the rightmost symbol will be UNIONed with the follow of the leftmost!
                    // We also need to include TK_DOLLAR when updating, so the length of the segment mask is
                    // G.num_tokens + 1, and NOT G.num_tokens
                    int temp = Union(F.follow[G.rules[i].right[j]],  F.follow[G.rules[i].left], G.num_tokens);
                    if(change != 1) {
                        change = temp;
                    }
                    break;
                }   
                
                // Otherwise, the follow of the current right symbol will be ORed with the first of the next right symbol
                int temp = Union(F.follow[G.rules[i].right[j]] , F.first[G.rules[i].right[j+1]], G.num_tokens);
                if(change != 1)
                    change = temp;
                
                // Again, if Epsilon is there in the first of the next right symbol, consider the follow set of that
                // symbol instead, since Epsilon cannot appear on any follow set.
                if(F.first[G.rules[i].right[j+1]][0] == 1) {
                    int temp = Union(F.follow[G.rules[i].right[j]] , F.follow[G.rules[i].right[j+1]], G.num_tokens);
                    if(change != 1)
                        change = temp;
                }
            }
        }       
    } while(change==1);
    
    for (int i=0; i<G.num_tokens; i++)
        for (int j=0; j<G.num_tokens + G.num_symbols; j++)
            F.follow[i][j] = 0;
    
    return F;
}

ParseTable createParseTable(FirstAndFollow F, Grammar G){
    // Construct the parse table using Grammar and
    // the First, Follow sets
    ParseTable P;
    P.matrix = (int **) calloc (F.num_symbols + F.num_tokens + 1, sizeof(int*)); 
    for (int i=0; i<F.num_tokens + F.num_symbols + 1; i++) 
         P.matrix[i] = (int *) calloc (F.num_tokens + 1, sizeof(int));
    
    for (int i=0; i<G.num_rules; i++)
        for (int j=0; j<G.num_symbols; j++)
        P.matrix[i][j] = -1;
    
    P.num_symbols = F.num_symbols;
    P.num_tokens = F.num_tokens;
    
    for(int i=0; i<G.num_rules; i++) {
        int A = G.rules[i].left;
        int B = G.rules[i].right[0];
        
        if (B == TK_EPSILON) {
            for(int j=1; j<G.num_tokens; j++) {
                if(F.follow[A][j] == 1)
                    P.matrix[A][j] = i;
            }
            continue;
        }
        
        for(int j=1; j<G.num_tokens; j++) {
            // Add nonterminal rules which don't have epsilon on it's
            // first set. We add the corresponding rule number
            if(F.first[B][j] == 1) {
                P.matrix[A][j] = i;
            }
        }
        
        // Otherwise, if there's an epsilon, simply add
        // it's follow set to the correct index
        if(F.first[B][0] == 1) {
            for(int j=1; j<G.num_tokens; j++) {
                if(F.follow[B][j] == 1)
                    P.matrix[A][j] = i;
            }   
        }
    }
    return P;
}

TreeNode* generateParseTree (char* filename, ParseTable p, Grammar g) {
    Token t = {TK_DOLLAR, NULL, -1};
    TreeNode* dollar = make_tree_node(NULL, t);
    StackNode* stack = make_stack_node(dollar);
    
    t.token_type = program;
    TreeNode* root = make_tree_node(NULL, t);
    stack = push(stack, root);

    init_tokenizer(filename);
    
    t = get_next_token();
    
    bool is_complete = false;
    
    bool is_correct = true;
        
    while (!is_empty(stack)) {
        if (t.token_type == TK_ERROR) {
            fprintf(stderr, "Syntax Error: At line number: %d\n", t.line_no);
            t = get_next_token();
            continue;
        }
        else if (t.token_type == TK_DOLLAR) {
            if (is_correct) {
                printf("Parsed successfully!\n");
                break;
            }
            else {
                printf("Finished parsing with errors\n");
                break;
            }
        }
        else if (t.token_type == TK_COMMENTMARK) {
            t = get_next_token();
            continue;
        }
        if (stack->data->check_term == false) {
            // Non terminal
            TreeNode* curr = stack->data;
            // Look at the parse table
            int rule_no = p.matrix[curr->token.token_type][t.token_type];
            if (rule_no == -1) {
                if (t.token_type != TK_COMMENTMARK) {
                    fprintf(stderr, "Syntax Error at Line %d: Got Token: %s\n", t.line_no, get_parser_token(t));
                    is_correct = false;
                }
                t = get_next_token();
                continue;
            }
            // Pop the stack
            stack = pop(stack);
            
            Rule rule = g.rules[rule_no];
            curr->children = (TreeNode**) calloc (rule.num_right, sizeof(TreeNode*));
            curr->rule_no = rule_no;
            curr->num_children = rule.num_right;
            
            for (int i=0; i<rule.num_right; i++) {
                Token temp = {rule.right[i], NULL, -1};
                curr->children[i] = make_tree_node(curr, temp);
            }
            
            for (int i=rule.num_right-1; i>=0; i--) {
                stack = push(stack, curr->children[i]);
            }
            continue;
        }
        else {
            TreeNode* curr = stack->data;
            if (curr->token.token_type == TK_EPSILON) {
                // Pop from the stack and add to curr
                stack = pop(stack);
                curr->token.lexeme = NULL;
                curr->token.line_no = -1;
                continue;
            }
            if (curr->token.token_type == t.token_type) {
                if (t.token_type == TK_DOLLAR) {
                    // End of stack
                    is_complete = true;
                    break;
                }
                // Pop the stack and then go to the next token
                stack = pop(stack);
                curr->token.lexeme = t.lexeme;
                curr->token.line_no = t.line_no;
                t = get_next_token();
            }
            else {
                //continue;
                fprintf(stderr, "Syntax Error at Line %d: Expected: %s, but got: %s\n", t.line_no, get_parser_token(stack->data->token), get_parser_token(t));
                //fprintf(stderr, "Syntax Error at Line %d: Expected: %s, but got: %s\n", t.line_no, get_string_from_term(stack->data->token.token_type), get_string_from_term(t.token_type));
                // Pop from the stack
                stack = pop(stack);
                t = get_next_token();
                continue;
            }
        }
        if (is_complete)
            break;
    }
    //free_stack(stack);
    close_tokenizer();
    return root;
}

void pretty_print(TreeNode* node) {
    // Prints only one node
    if (node->check_term == true) {
        // Terminal
        Token t = node->token;
        if (t.token_type == TK_EPSILON) {
            printf("-----\t");
            printf("-----\t");
        }
        else {
            printf("%s\t", node->token.lexeme);
            printf("%d\t", node->token.line_no);
        }
        printf("%s\t", get_string_from_term(node->token.token_type));
        if (t.token_type == TK_NUM)
            printf("%d\t", atoi(t.lexeme));
        else if (t.token_type == TK_RNUM) {
            for (int i=0; t.lexeme[i] != '\0'; i++)
                if (t.lexeme[i] == 'E')
                    t.lexeme[i] = 'e';
            printf("%.4f\t", atof(t.lexeme));
        }
        else
            printf("-----\t");
        printf("%s\t", get_string_from_term(node->parent->token.token_type));
        if (node->check_term)
            printf("Yes\t");
        else
            printf("No\t");
        printf("-----\t");
        printf("\n");
    }
    else {
        // Non Terminal
        printf("-----\t");
        printf("-----\t");
        printf("-----\t");
        printf("-----\t");
        printf("%s\t", get_string_from_term(node->parent->token.token_type));
        if (node->check_term)
            printf("Yes\t");
        else
            printf("No\t");
        printf("%s\t", get_string_from_term(node->token.token_type));
        printf("\n");
    }
}

void printParseTree(TreeNode* root) {
    if (root->num_children == 0) {
        pretty_print(root);
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        printParseTree(root->children[i]);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Format: %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
    FILE* fp = fopen("grammar_rules.txt", "r");
    if (!fp) {
        perror("File 'grammar_rules.txt' does not exist\n");
        exit(EXIT_FAILURE);
    }
    Grammar g = populate_grammar(fp);
    print_grammar(g);
    FirstAndFollow f = ComputeFirstAndFollowSets(g);
    printFirstAndFollowSets(f);
    ParseTable p = createParseTable(f, g);
    printParseTable(p);
    
    char* filename = argv[1];
    printf("Generating Parse Tree...\n");
    TreeNode* root = generateParseTree(filename, p, g);
    printParseTree(root);
    printTreeNode(root);
    free_first_and_follow(f);
    free_parse_table(p);
    fclose(fp);
    free_grammar(g);
    free_tree(root);
    return 0;
}
