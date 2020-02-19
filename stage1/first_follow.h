#include "common.h"

typedef struct GrammarSymbol GrammarSymbol;

struct GrammarSymbol{
    // Grammar Symbol Structure:
    // Has the token ID and an is_terminal marker
    term token_id;
    bool is_terminal;
};

typedef struct GrammarRule GrammarRule;

struct GrammarRule{
    // Grammar Rule Structure for a single Grammar nonterminal Symbol:
    // Has a left Grammar Symbol, and an array of
    // right Symbols, since each rule has multiple symbols, and
    // there can be multiple rules for each symbol
    GrammarSymbol left;
    GrammarSymbol** right;
    int num_left;
    int* num_right;
};

typedef struct Grammar Grammar;

struct Grammar{
    // Structure that represents the complete Grammar for the
    // language.
    // This consists of a set of Grammar Rules, and a first and follow
    // set for each rule. Since we encode the grammar to integer numbers,
    // we can also have the first and follow sets as a 2d-array of integers
    GrammarRule* rules;
    int **first, **follow;
    int num_rules; // Number of distinct rules (Nonterminals on the left)
    int num_symbols; // Number of distinct symbols (Terminals + Non Terminals)
};

// Declare all global variables here


// Define all function Definitions here
GrammarSymbol make_grammar_symbol(term t) {
    // Construct the grammar Symbol from a token
    bool is_terminal = false;
    // ASSUMPTION: Terminals start from {54, }
    // and EPSILON = 0
    if (t >= 54 || t == 0)
        is_terminal = true;
    GrammarSymbol g = {t, is_terminal};
    return g;
}

GrammarRule make_grammar_rule(GrammarSymbol left, GrammarSymbol** right, int num_left, int* num_right) {
    // Construct the grammar rule from the current line of fp
    //GrammarSymbol* right = (GrammarSymbol*) calloc (num_right, sizeof(GrammarSymbol));
    GrammarRule grule;
    grule.left = left;
    grule.right = right;
    grule.num_left = num_left;
    grule.num_right = num_right;
    return grule;
}

GrammarRule add_symbol_to_rule(GrammarRule grule, int left_num, GrammarSymbol symbol, bool add_to_left) {
    // Adds the new symbol to the existing grule of left_num
    if (add_to_left) {
        // This symbol must be added to the left side
        grule.left = symbol;
        grule.num_left ++;
        return grule;
    }
    else {
        // Otherwise, add to the right hand side
        if (grule.num_left >= left_num) {
            // Get the number of right symbols for left_num
            int right_num = grule.num_right[left_num];
            // And add another symbol to it's end
            grule.right = (GrammarSymbol**) realloc (grule.right, (grule.num_right[left_num] + 1) * sizeof(GrammarSymbol*));
            grule.right[left_num][right_num] = symbol;
            grule.num_right[left_num] ++;
        }
        return grule;
    }
}

Grammar add_rule_to_grammar(Grammar g, GrammarRule rule) {
    // Adds rule to the set of grammar rules
    g.rules = (GrammarRule*) realloc (g.rules, (g.num_rules + 1) * sizeof (GrammarRule));
    g.rules[g.num_rules] = rule;
    g.num_rules ++;
    return g;
}

Grammar add_grammar_rule(Grammar g, GrammarRule rule) {
    // Adds the grammar rule to the set of grammar rules
    g.rules = (GrammarRule*) realloc (g.rules, g.num_rules + 1);
    g.rules[g.num_rules] = rule;
    g.num_rules ++; 
    return g;
}

void free_grammar_rule(GrammarRule grule) {
    for (int i=0; i < grule.num_left; i++) {
        free(grule.right[i]);
    }
    free(grule.right);
    free(grule.num_right);
}

void free_grammar(Grammar g) {
    for (int i=0; i < g.num_rules; i++) {
        free_grammar_rule(g.rules[i]);
    }
    free(g.rules);
    if (g.first) {
        for (int i=0; i<g.num_symbols; i++) {
            if (g.first[i])
                free(g.first[i]);
        }
        free(g.first);
    }
    if (g.follow) {
        for (int i=0; i<g.num_symbols; i++) {
            if (g.follow[i])
                free(g.follow[i]);
        }
        free(g.follow);
    }
}

void print_rule(GrammarRule grule) {
    // Prints a grammar rule
    for (int i=0; i<grule.num_left; i++) {
        printf("LHS : %s , RHS : ", get_string_from_term(grule.left.token_id));
        for (int j=0; j<grule.num_right[i]; j++) {
            printf("%s , ", get_string_from_term(grule.right[i][j].token_id));
        }
        printf("\n");
    }
}

void print_grammar(Grammar g) {
    // Prints the grammar
    if (g.num_rules <= 0)
        return;
    printf("Grammar retrieved from file.\n-----------------------------\nStatistics:\n");
    printf("Number of symbols: %d\n", g.num_symbols);
    printf("Number of Rules: %d\n", g.num_rules);
    for (int i=0; i<g.num_rules; i++) {
        print_rule(g.rules[i]);
    }
    printf("-----------------------------\n");
}

Grammar get_grammar(FILE* fp) {
    // Reads the grammar from the file pointer
    // into memory and returns a grammar
    Grammar grammar;
    grammar.rules = (GrammarRule*) calloc (1, sizeof(GrammarRule));
    grammar.rules[0].right = (GrammarSymbol**) calloc (1, sizeof(GrammarSymbol*));
    grammar.rules[0].right[0] = (GrammarSymbol*) calloc (1, sizeof(GrammarSymbol));
    grammar.rules[0].num_right = (int*) calloc (1, sizeof(int));
    grammar.first = NULL;
    grammar.follow = NULL;
    grammar.num_rules = 0;
    grammar.num_symbols = 0;
    
    int num;
    char ch = '\0';

    int curr_rule = -1;
    int curr_left = 0;
    int curr_right = 0;

    // Flag to detect the left hand term
    bool is_lhs = false;
    // Flag to detect a pipe symbol
    bool is_pipe = false;

    // End of array marker
    //GrammarSymbol end_marker = {-2, true};

    do {
        if (ch == '|') {
            // Encode it as -1
            grammar.rules[curr_rule].right = (GrammarSymbol**) realloc (grammar.rules[curr_rule].right, (curr_left + 2) * sizeof(GrammarSymbol*));
            grammar.rules[curr_rule].right[curr_left + 1] = (GrammarSymbol*) calloc (1, sizeof(GrammarSymbol));
            grammar.rules[curr_rule].num_right = (int*) realloc (grammar.rules[curr_rule].num_right, (curr_left + 2) * sizeof(int));
            // Increment the left symbol count for the current rule
            curr_left ++;
            // And set the right count as 0
            curr_right = 0;
            // is_pipe flag when encountering a pipe symbol
            is_pipe = true;
        }
        else if (ch == 'F') {
            // Go to the next rule and end current rule
            if (curr_rule >= 0) {
                // Allocate memory for the new grammar rule and it's components
                // This tries to allocate for the next rule to be encountered after parsing this line,
                // so we allocate for grammar.rules[curr_rule + 1]
                // and increment the current rule since we go to the next rule
                grammar.rules = (GrammarRule*) realloc (grammar.rules, (curr_rule + 2) * sizeof(GrammarRule));
                grammar.rules[curr_rule + 1].right = (GrammarSymbol**) calloc (1, sizeof(GrammarSymbol*));
                grammar.rules[curr_rule + 1].right[0] = (GrammarSymbol*) calloc (1, sizeof(GrammarSymbol));
                grammar.rules[curr_rule + 1].num_right = (int*) calloc (1, sizeof(int));
                grammar.rules[curr_rule].num_left = curr_left + 1;
                curr_rule ++;
                grammar.num_rules ++;
                curr_right = 0;
                curr_left = 0;
            }
            else {
                // Start afresh. Initialize all to 0
                curr_rule = 0;
                curr_left = 0;
                curr_right = 0;
            }
            is_lhs = true;
        }
        while (fscanf(fp, " %d", &num) == 1) {
            // Add the number to the array
            // and update the number of symbols
            if (num > grammar.num_symbols) {
                grammar.num_symbols = num;
            }
            if (is_lhs) {
                // This is the left most symbol, so add it to the grammar rule
                grammar.rules[curr_rule].left = make_grammar_symbol(num);
                // Allocate the number of right symbols for this specific rule
                // under a left symbol
                grammar.rules[curr_rule].num_right = (int*) realloc (grammar.rules[curr_rule].num_right, (curr_left + 2) * sizeof(int));
                grammar.rules[curr_rule].num_right[curr_left] = curr_right + 1;
                // Right now, no right symbols are encountered
                curr_right = 0;
            }
            else if (is_pipe) {
                // For the left symbol, add rules to the right
                // Reallocate memory for the right symbol
                // and increase the count of right symbols for the left symbol
                grammar.rules[curr_rule].right[curr_left] = (GrammarSymbol*) realloc (grammar.rules[curr_rule].right[curr_left], (curr_right + 2) * sizeof(GrammarSymbol));
                grammar.rules[curr_rule].right[curr_left][curr_right] = make_grammar_symbol(num);
                grammar.rules[curr_rule].num_right = (int*) realloc (grammar.rules[curr_rule].num_right, (curr_left + 2) * sizeof(int));
                grammar.rules[curr_rule].num_right[curr_left] = curr_right + 1;
                curr_right ++;
                is_pipe = false;
            }
            else {
                // This belongs to the right side
                // Reallocate memory for the right symbol
                // and increase the count of right symbols for the left symbol
                grammar.rules[curr_rule].right[curr_left] = (GrammarSymbol*) realloc (grammar.rules[curr_rule].right[curr_left], (curr_right + 2) * sizeof(GrammarSymbol));
                grammar.rules[curr_rule].right[curr_left][curr_right] = make_grammar_symbol(num);
                grammar.rules[curr_rule].num_right = (int*) realloc (grammar.rules[curr_rule].num_right, (curr_left + 2) * sizeof(int));
                grammar.rules[curr_rule].num_right[curr_left] = curr_right + 1;
                curr_right ++;
            }
            is_lhs = false;
        }
    } while (fscanf(fp, " %c", &ch) == 1);

    // We need to take the last rule into account, since we reach EOF
    // before reading another 'F'
    grammar.num_rules++;
    grammar.rules[curr_rule].num_left = curr_left + 1;
    grammar.rules[curr_rule].num_right[curr_left] = curr_right;
    
    //grammar.rules[curr_rule].num_right[curr_left] = curr_right + 1;
    //grammar.rules[curr_rule].right[curr_left][curr_right] = end_marker;

    return grammar;
}

void compute_first_set(Grammar g) {
    // Computes the first set for the Grammar
    if (g.num_rules == 0)
        return;
}