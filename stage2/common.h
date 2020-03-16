#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DEBUG 1

enum bool { false = 0, true = ~0 };

typedef enum bool bool;

#define CAPACITY 5000 // Size of the Hash Table

// Define the Terminals here
typedef enum {
    TK_EPSILON = 0,
    TK_DECLARE = 1,
    TK_MODULE = 2,
    TK_ID = 3,
    TK_SEMICOL = 4,
    TK_DRIVERDEF = 5,
    TK_DRIVER = 6,
    TK_PROGRAM = 7,
    TK_DRIVERENDDEF = 8,
    TK_DEF = 9,
    TK_ENDDEF = 10,
    TK_TAKES = 11,
    TK_INPUT = 12,
    TK_SQBO = 13,
    TK_SQBC = 14,
    TK_RETURNS = 15,
    TK_COLON = 16,
    TK_COMMA = 17,
    TK_INTEGER = 18,
    TK_REAL = 19,
    TK_BOOLEAN = 20,
    TK_ARRAY = 21,
    TK_OF = 22,
    TK_START = 23,
    TK_END = 24,
    TK_GET_VALUE = 25,
    TK_BO = 26,
    TK_BC = 27,
    TK_PRINT = 28,
    TK_NUM = 29,
    TK_RNUM = 30,
    TK_TRUE = 31,
    TK_FALSE = 32,
    TK_ASSIGNOP = 33,
    TK_USE = 34,
    TK_WITH = 35,
    TK_PARAMETERS = 36,
    TK_MINUS = 37,
    TK_PLUS = 38,
    TK_MUL = 39,
    TK_DIV = 40,
    TK_AND = 41,
    TK_OR = 42,
    TK_LT = 43,
    TK_LE = 44,
    TK_GT = 45,
    TK_GE = 46,
    TK_EQ = 47,
    TK_NE = 48,
    TK_SWITCH = 49,
    TK_CASE = 50,
    TK_BREAK = 51,
    TK_DEFAULT = 52,
    TK_FOR = 53,
    TK_IN = 54,
    TK_WHILE = 55,
    TK_RANGEOP = 56,
    // The below 8 tokens are not part of the
    // grammar structure, so it is separated from
    // it
        TK_EOF = 57,
        TK_ERROR = 580,
        TK_NONE = 590,
        TK_COMMENTMARK = 600,
        TK_RECORD = 610,
        TK_TAGGED = 620,
        TK_UNION = 630,
        // ASSUMPTION: TK_DOLLAR is the last symbol
        TK_DOLLAR = 57,
    // Grammar non terminals start from here
    program = 58,
    moduleDeclarations = 59,
    moduleDeclaration = 60,
    otherModules = 61,
    driverModule = 62,
    module = 63,
    ret = 64,
    input_plist = 65,
    N1 = 66,
    output_plist = 67,
    N2 = 68,
    dataType = 69,
    range_arrays = 70,
    type = 71,
    moduleDef = 72,
    statements = 73,
    statement = 74,
    ioStmt = 75,
    var = 76,
    boolConstt = 77,
    var_id_num = 78,
    whichId = 79,
    simpleStmt = 80,
    assignmentStmt = 81,
    whichStmt = 82,
    lvalueIDStmt = 83,
    lvalueARRStmt = 84,
    g_index = 85,
    moduleReuseStmt = 86,
    optional = 87,
    idList = 88,
    N3 = 89,
    expression = 90,
    U = 91,
    new_NT = 92,
    unary_op = 93,
    arithmeticOrBooleanExpr = 94,
    N7 = 95,
    AnyTerm = 96,
    N8 = 97,
    arithmeticExpr = 98,
    N4 = 99,
    g_term = 100,
    N5 = 101,
    factor = 102,
    op1 = 103,
    op2 = 104,
    logicalOp = 105,
    relationalOp = 106,
    declareStmt = 107,
    condionalStmt = 108,
    caseStmts = 109,
    N9 = 110,
    value = 111,
    g_default = 112,
    iterativeStmt = 113,
    range = 114,
}term;

typedef struct Symbol Symbol;

struct Symbol {
    char key[30];
    term tid;
};

bool is_terminal(int num);
char* get_string_from_term(term t);
term get_term_from_int(int term_int);
term get_term_from_string(char* str);
#endif
