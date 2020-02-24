typedef struct Rule Rule;

struct Rule{
    // Grammar Rule Structure for a single Grammar nonterminal Symbol:
    // Has a left Grammar Symbol, and an array of
    // right Symbols, since each rule has multiple symbols, and
    // there can be multiple rules for each symbol
    int left;
    int* right;
    int num_right;
    int rule_no;
};

typedef struct FirstAndFollow FirstAndFollow;

struct FirstAndFollow {
    int** first;
    int** follow;
	int num_symbols;
	int num_tokens;
};

typedef struct Grammar Grammar;

struct Grammar{
    // Structure that represents the complete Grammar for the
    // language.
    // This consists of a set of Grammar Rules, and a first and follow
    // set for each rule. Since we encode the grammar to integer numbers,
    // we can also have the first and follow sets as a 2d-array of integers
    Rule* rules;
    FirstAndFollow F;
    int num_rules; // Number of distinct rules (Nonterminals on the left)
    int num_symbols; // Number of distinct Non Terminals
    int num_tokens; // Number of Terminals
};

typedef struct ParseTable ParseTable;
struct ParseTable {
    // The Parse Table structure is a 
    // 2-D array of symbols * tokens
    int** matrix;
    int num_symbols;
    int num_tokens;
};


// Declare all global variables here
