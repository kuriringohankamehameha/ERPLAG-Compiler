// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#include "lexer.h"

// Define all function definitions here
// A Hash Function for the hash table of keywords
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

term is_keyword_binary_search(char* lexeme) {
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

    return TK_ID;
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

void run_tokenizer(char* filename) {
    // Runs the tokenizer on the input file
    init_tokenizer(filename);

    // Process the tokens using get_next_token()
    for (;;) {
        Token t = get_next_token();
        if (t.token_type == TK_EOF)
            break;
        // Process token only if the lexeme exists.
        // This means that any TK_COMMENTMARK is avoided
        if (t.lexeme) {
            /*
            printf("Line no: %d\n", t.line_no);
            printf("Token: %s\n", (char*)t.lexeme);
            printf("Type: ");
            print_token_type(t);
            */

            // Insert the token into the token stream
            insert_token_stream(t);
        }
    }

    // Prints the token stream
    print_token_stream();
    // Close the tokenizer
    close_tokenizer();
}

int main(int argc, char* argv[]) {
    // Driver function to test the tokenizer
    //run_tokenizer("sample.txt");
    if (argc == 2) {
        // Run the tokenizer
    	run_tokenizer(argv[1]);
        // Remove comments and write to an output file
        //remove_comments(argv[1], "output.txt");
    }
    else {
    	fprintf(stderr, "Format: %s input.txt\n", argv[0]);
    	exit(EXIT_FAILURE);
    }
    return 0;
}
