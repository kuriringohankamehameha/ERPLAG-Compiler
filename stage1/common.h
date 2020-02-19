#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

enum bool { false = 0, true = ~0 };

typedef enum bool bool;

#define CAPACITY 5000 // Size of the Hash Table

// Define the Terminals here
typedef enum {
    program = 1,
    moduleDeclarations = 2,
    moduleDeclaration = 3,
    otherModules = 4,
    driverModule = 5,
    module = 6,
    ret = 7,
    input_plist = 8,
    IPL = 9,
    output_plist = 10,
    OPL = 11,
    dataType = 12,
    type = 13,
    moduleDef = 14,
    statements = 15,
    statement = 16,
    ioStmt = 17,
    var = 18,
    whichId = 19,
    simpleStmt = 20,
    assignmentStmt = 21,
    whichStmt = 22,
    lvalueIDStmt = 23,
    lvalueARRStmt = 24,
    arrIndex = 25,
    moduleReuseStmt = 26,
    optional = 27,
    idList = 28,
    IDL = 29,
    Unary = 30,
    UX = 31,
    expression = 32,
    NX = 33,
    AorBExpr = 34,
    NY = 35,
    arithmeticExpr = 36,
    ARE = 37,
    AOP = 38,
    termExpr = 39,
    FAC = 40,
    MOP = 41,
    factor = 42,
    logicalOp = 43,
    relationalOp = 44,
    declareStmt = 45,
    conditionalStmt = 46,
    caseStmts = 47,
    cStmt = 48,
    value = 49,
    defaultRule = 50,
    iterativeStmt = 51,
    range = 52,
    TK_DECLARE = 54,
    TK_MODULE = 55,
    TK_ID = 56,
    TK_SEMICOL = 57,
    TK_DRIVERDEF = 58,
    TK_DRIVER = 59,
    TK_PROGRAM = 60,
    TK_DRIVERENDDEF = 61,
    TK_DEF = 62,
    TK_ENDDEF = 63,
    TK_TAKES = 64,
    TK_INPUT = 65,
    TK_SQBO = 66,
    TK_SQBC = 67,
    TK_RETURNS = 68,
    TK_COLON = 69,
    TK_COMMA = 70,
    TK_INTEGER = 71,
    TK_REAL = 72,
    TK_BOOLEAN = 73,
    TK_ARRAY = 74,
    TK_OF = 75,
    TK_START = 76,
    TK_END = 77,
    TK_GET_VALUE = 78,
    TK_BO = 79,
    TK_BC = 80,
    TK_PRINT = 81,
    TK_NUM = 82,
    TK_RNUM = 83,
    TK_TRUE = 84,
    TK_FALSE = 85,
    TK_ASSIGNOP = 86,
    TK_USE = 87,
    TK_WITH = 88,
    TK_PARAMETERS = 89,
    TK_MINUS = 90,
    TK_PLUS = 91,
    TK_MUL = 92,
    TK_DIV = 93,
    TK_AND = 94,
    TK_OR = 95,
    TK_LT = 96,
    TK_LE = 97,
    TK_GT = 98,
    TK_GE = 99,
    TK_EQ = 100,
    TK_NE = 101,
    TK_SWITCH = 102,
    TK_CASE = 103,
    TK_BREAK = 104,
    TK_DEFAULT = 105,
    TK_FOR = 106,
    TK_IN = 107,
    TK_WHILE = 108,
    TK_RANGEOP = 109,
    TK_EPSILON = 0,
    TK_EOF,
    TK_ERROR,
    TK_NONE,
    TK_COMMENTMARK,
    TK_RECORD,
    TK_TAGGED,
    TK_UNION,
}term;

char* get_string_from_term(term t) {
    switch(t) {
        case program:
        return "program";
        break;
        case moduleDeclarations:
        return "moduleDeclarations";
        break;
        case moduleDeclaration:
        return "moduleDeclaration";
        break;
        case otherModules:
        return "otherModules";
        break;
        case driverModule:
        return "driverModule";
        break;
        case module:
        return "module";
        break;
        case ret:
        return "ret";
        break;
        case input_plist:
        return "input_plist";
        break;
        case IPL:
        return "IPL";
        break;
        case output_plist:
        return "output_plist";
        break;
        case OPL:
        return "OPL";
        break;
        case dataType:
        return "dataType";
        break;
        case type:
        return "type";
        break;
        case moduleDef:
        return "moduleDef";
        break;
        case statements:
        return "statements";
        break;
        case statement:
        return "statement";
        break;
        case ioStmt:
        return "ioStmt";
        break;
        case var:
        return "var";
        break;
        case whichId:
        return "whichId";
        break;
        case simpleStmt:
        return "simpleStmt";
        break;
        case assignmentStmt:
        return "assignmentStmt";
        break;
        case whichStmt:
        return "whichStmt";
        break;
        case lvalueIDStmt:
        return "lvalueIDStmt";
        break;
        case lvalueARRStmt:
        return "lvalueARRStmt";
        break;
        case arrIndex:
        return "arrIndex";
        break;
        case moduleReuseStmt:
        return "moduleReuseStmt";
        break;
        case optional:
        return "optional";
        break;
        case idList:
        return "idList";
        break;
        case IDL:
        return "IDL";
        break;
        case Unary:
        return "Unary";
        break;
        case UX:
        return "UX";
        break;
        case expression:
        return "expression";
        break;
        case NX:
        return "NX";
        break;
        case AorBExpr:
        return "AorBExpr";
        break;
        case NY:
        return "NY";
        break;
        case arithmeticExpr:
        return "arithmeticExpr";
        break;
        case ARE:
        return "ARE";
        break;
        case AOP:
        return "AOP";
        break;
        case termExpr:
        return "termExpr";
        break;
        case FAC:
        return "FAC";
        break;
        case MOP:
        return "MOP";
        break;
        case factor:
        return "factor";
        break;
        case logicalOp:
        return "logicalOp";
        break;
        case relationalOp:
        return "relationalOp";
        break;
        case declareStmt:
        return "declareStmt";
        break;
        case conditionalStmt:
        return "conditionalStmt";
        break;
        case caseStmts:
        return "caseStmts";
        break;
        case cStmt:
        return "cStmt";
        break;
        case value:
        return "value";
        break;
        case defaultRule:
        return "defaultRule";
        break;
        case iterativeStmt:
        return "iterativeStmt";
        break;
        case range:
        return "range";
        break;
        case TK_DECLARE:
        return "TK_DECLARE";
        break;
        case TK_MODULE:
        return "TK_MODULE";
        break;
        case TK_ID:
        return "TK_ID";
        break;
        case TK_SEMICOL:
        return "TK_SEMICOL";
        break;
        case TK_DRIVERDEF:
        return "TK_DRIVERDEF";
        break;
        case TK_DRIVER:
        return "TK_DRIVER";
        break;
        case TK_PROGRAM:
        return "TK_PROGRAM";
        break;
        case TK_DRIVERENDDEF:
        return "TK_DRIVERENDDEF";
        break;
        case TK_DEF:
        return "TK_DEF";
        break;
        case TK_ENDDEF:
        return "TK_ENDDEF";
        break;
        case TK_TAKES:
        return "TK_TAKES";
        break;
        case TK_INPUT:
        return "TK_INPUT";
        break;
        case TK_SQBO:
        return "TK_SQBO";
        break;
        case TK_SQBC:
        return "TK_SQBC";
        break;
        case TK_RETURNS:
        return "TK_RETURNS";
        break;
        case TK_COLON:
        return "TK_COLON";
        break;
        case TK_COMMA:
        return "TK_COMMA";
        break;
        case TK_INTEGER:
        return "TK_INTEGER";
        break;
        case TK_REAL:
        return "TK_REAL";
        break;
        case TK_BOOLEAN:
        return "TK_BOOLEAN";
        break;
        case TK_ARRAY:
        return "TK_ARRAY";
        break;
        case TK_OF:
        return "TK_OF";
        break;
        case TK_START:
        return "TK_START";
        break;
        case TK_END:
        return "TK_END";
        break;
        case TK_GET_VALUE:
        return "TK_GET_VALUE";
        break;
        case TK_BO:
        return "TK_BO";
        break;
        case TK_BC:
        return "TK_BC";
        break;
        case TK_PRINT:
        return "TK_PRINT";
        break;
        case TK_NUM:
        return "TK_NUM";
        break;
        case TK_RNUM:
        return "TK_RNUM";
        break;
        case TK_TRUE:
        return "TK_TRUE";
        break;
        case TK_FALSE:
        return "TK_FALSE";
        break;
        case TK_ASSIGNOP:
        return "TK_ASSIGNOP";
        break;
        case TK_USE:
        return "TK_USE";
        break;
        case TK_WITH:
        return "TK_WITH";
        break;
        case TK_PARAMETERS:
        return "TK_PARAMETERS";
        break;
        case TK_MINUS:
        return "TK_MINUS";
        break;
        case TK_PLUS:
        return "TK_PLUS";
        break;
        case TK_MUL:
        return "TK_MUL";
        break;
        case TK_DIV:
        return "TK_DIV";
        break;
        case TK_AND:
        return "TK_AND";
        break;
        case TK_OR:
        return "TK_OR";
        break;
        case TK_LT:
        return "TK_LT";
        break;
        case TK_LE:
        return "TK_LE";
        break;
        case TK_GT:
        return "TK_GT";
        break;
        case TK_GE:
        return "TK_GE";
        break;
        case TK_EQ:
        return "TK_EQ";
        break;
        case TK_NE:
        return "TK_NE";
        break;
        case TK_SWITCH:
        return "TK_SWITCH";
        break;
        case TK_CASE:
        return "TK_CASE";
        break;
        case TK_BREAK:
        return "TK_BREAK";
        break;
        case TK_DEFAULT:
        return "TK_DEFAULT";
        break;
        case TK_FOR:
        return "TK_FOR";
        break;
        case TK_IN:
        return "TK_IN";
        break;
        case TK_WHILE:
        return "TK_WHILE";
        break;
        case TK_RANGEOP:
        return "TK_RANGEOP";
        break;
        case TK_EPSILON:
        return "TK_EPSILON";
        break;
        default:
        return NULL;
    }
}

term get_term_from_int(int term_int) {
    if (term_int == -1) {
        return TK_ERROR;
    }
    return term_int;
}

term get_term_from_string(char* str) {
    // Get a term from a string
    if (strcmp(str, "<program>") == 0) {
        return program;
    }
    if (strcmp(str, "<moduleDeclarations>") == 0) {
        return moduleDeclarations;
    }
    if (strcmp(str, "<moduleDeclaration>") == 0) {
        return moduleDeclaration;
    }
    if (strcmp(str, "<otherModules>") == 0) {
        return otherModules;
    }
    if (strcmp(str, "<driverModule>") == 0) {
        return driverModule;
    }
    if (strcmp(str, "<module>") == 0) {
        return module;
    }
    if (strcmp(str, "<ret>") == 0) {
        return ret;
    }
    if (strcmp(str, "<input_plist>") == 0) {
        return input_plist;
    }
    if (strcmp(str, "<IPL>") == 0) {
        return IPL;
    }
    if (strcmp(str, "<output_plist>") == 0) {
        return output_plist;
    }
    if (strcmp(str, "<OPL>") == 0) {
        return OPL;
    }
    if (strcmp(str, "<dataType>") == 0) {
        return dataType;
    }
    if (strcmp(str, "<type>") == 0) {
        return type;
    }
    if (strcmp(str, "<moduleDef>") == 0) {
        return moduleDef;
    }
    if (strcmp(str, "<statements>") == 0) {
        return statements;
    }
    if (strcmp(str, "<statement>") == 0) {
        return statement;
    }
    if (strcmp(str, "<ioStmt>") == 0) {
        return ioStmt;
    }
    if (strcmp(str, "<var>") == 0) {
        return var;
    }
    if (strcmp(str, "<whichId>") == 0) {
        return whichId;
    }
    if (strcmp(str, "<simpleStmt>") == 0) {
        return simpleStmt;
    }
    if (strcmp(str, "<assignmentStmt>") == 0) {
        return assignmentStmt;
    }
    if (strcmp(str, "<whichStmt>") == 0) {
        return whichStmt;
    }
    if (strcmp(str, "<lvalueIDStmt>") == 0) {
        return lvalueIDStmt;
    }
    if (strcmp(str, "<lvalueARRStmt>") == 0) {
        return lvalueARRStmt;
    }
    if (strcmp(str, "<arrIndex>") == 0) {
        return arrIndex;
    }
    if (strcmp(str, "<moduleReuseStmt>") == 0) {
        return moduleReuseStmt;
    }
    if (strcmp(str, "<optional>") == 0) {
        return optional;
    }
    if (strcmp(str, "<idList>") == 0) {
        return idList;
    }
    if (strcmp(str, "<IDL>") == 0) {
        return IDL;
    }
    if (strcmp(str, "<Unary>") == 0) {
        return Unary;
    }
    if (strcmp(str, "<UX>") == 0) {
        return UX;
    }
    if (strcmp(str, "<expression>") == 0) {
        return expression;
    }
    if (strcmp(str, "<NX>") == 0) {
        return NX;
    }
    if (strcmp(str, "<AorBExpr>") == 0) {
        return AorBExpr;
    }
    if (strcmp(str, "<NY>") == 0) {
        return NY;
    }
    if (strcmp(str, "<arithmeticExpr>") == 0) {
        return arithmeticExpr;
    }
    if (strcmp(str, "<ARE>") == 0) {
        return ARE;
    }
    if (strcmp(str, "<AOP>") == 0) {
        return AOP;
    }
    if (strcmp(str, "<termExpr>") == 0) {
        return termExpr;
    }
    if (strcmp(str, "<FAC>") == 0) {
        return FAC;
    }
    if (strcmp(str, "<MOP>") == 0) {
        return MOP;
    }
    if (strcmp(str, "<factor>") == 0) {
        return factor;
    }
    if (strcmp(str, "<logicalOp>") == 0) {
        return logicalOp;
    }
    if (strcmp(str, "<relationalOp>") == 0) {
        return relationalOp;
    }
    if (strcmp(str, "<declareStmt>") == 0) {
        return declareStmt;
    }
    if (strcmp(str, "<conditionalStmt>") == 0) {
        return conditionalStmt;
    }
    if (strcmp(str, "<caseStmts>") == 0) {
        return caseStmts;
    }
    if (strcmp(str, "<cStmt>") == 0) {
        return cStmt;
    }
    if (strcmp(str, "<value>") == 0) {
        return value;
    }
    if (strcmp(str, "<defaultRule>") == 0) {
        return defaultRule;
    }
    if (strcmp(str, "<iterativeStmt>") == 0) {
        return iterativeStmt;
    }
    if (strcmp(str, "<range>") == 0) {
        return range;
    }
    if (strcmp(str, "TK_DECLARE") == 0) {
        return TK_DECLARE;
    }
    if (strcmp(str, "TK_MODULE") == 0) {
        return TK_MODULE;
    }
    if (strcmp(str, "TK_ID") == 0) {
        return TK_ID;
    }
    if (strcmp(str, "TK_SEMICOL") == 0) {
        return TK_SEMICOL;
    }
    if (strcmp(str, "TK_DRIVERDEF") == 0) {
        return TK_DRIVERDEF;
    }
    if (strcmp(str, "TK_DRIVER") == 0) {
        return TK_DRIVER;
    }
    if (strcmp(str, "TK_PROGRAM") == 0) {
        return TK_PROGRAM;
    }
    if (strcmp(str, "TK_DRIVERENDDEF") == 0) {
        return TK_DRIVERENDDEF;
    }
    if (strcmp(str, "TK_DEF") == 0) {
        return TK_DEF;
    }
    if (strcmp(str, "TK_ENDDEF") == 0) {
        return TK_ENDDEF;
    }
    if (strcmp(str, "TK_TAKES") == 0) {
        return TK_TAKES;
    }
    if (strcmp(str, "TK_INPUT") == 0) {
        return TK_INPUT;
    }
    if (strcmp(str, "TK_SQBO") == 0) {
        return TK_SQBO;
    }
    if (strcmp(str, "TK_SQBC") == 0) {
        return TK_SQBC;
    }
    if (strcmp(str, "TK_RETURNS") == 0) {
        return TK_RETURNS;
    }
    if (strcmp(str, "TK_COLON") == 0) {
        return TK_COLON;
    }
    if (strcmp(str, "TK_COMMA") == 0) {
        return TK_COMMA;
    }
    if (strcmp(str, "TK_INTEGER") == 0) {
        return TK_INTEGER;
    }
    if (strcmp(str, "TK_REAL") == 0) {
        return TK_REAL;
    }
    if (strcmp(str, "TK_BOOLEAN") == 0) {
        return TK_BOOLEAN;
    }
    if (strcmp(str, "TK_ARRAY") == 0) {
        return TK_ARRAY;
    }
    if (strcmp(str, "TK_OF") == 0) {
        return TK_OF;
    }
    if (strcmp(str, "TK_START") == 0) {
        return TK_START;
    }
    if (strcmp(str, "TK_END") == 0) {
        return TK_END;
    }
    if (strcmp(str, "TK_GET_VALUE") == 0) {
        return TK_GET_VALUE;
    }
    if (strcmp(str, "TK_BO") == 0) {
        return TK_BO;
    }
    if (strcmp(str, "TK_BC") == 0) {
        return TK_BC;
    }
    if (strcmp(str, "TK_PRINT") == 0) {
        return TK_PRINT;
    }
    if (strcmp(str, "TK_NUM") == 0) {
        return TK_NUM;
    }
    if (strcmp(str, "TK_RNUM") == 0) {
        return TK_RNUM;
    }
    if (strcmp(str, "TK_TRUE") == 0) {
        return TK_TRUE;
    }
    if (strcmp(str, "TK_FALSE") == 0) {
        return TK_FALSE;
    }
    if (strcmp(str, "TK_ASSIGNOP") == 0) {
        return TK_ASSIGNOP;
    }
    if (strcmp(str, "TK_USE") == 0) {
        return TK_USE;
    }
    if (strcmp(str, "TK_WITH") == 0) {
        return TK_WITH;
    }
    if (strcmp(str, "TK_PARAMETERS") == 0) {
        return TK_PARAMETERS;
    }
    if (strcmp(str, "TK_MINUS") == 0) {
        return TK_MINUS;
    }
    if (strcmp(str, "TK_PLUS") == 0) {
        return TK_PLUS;
    }
    if (strcmp(str, "TK_MUL") == 0) {
        return TK_MUL;
    }
    if (strcmp(str, "TK_DIV") == 0) {
        return TK_DIV;
    }
    if (strcmp(str, "TK_AND") == 0) {
        return TK_AND;
    }
    if (strcmp(str, "TK_OR") == 0) {
        return TK_OR;
    }
    if (strcmp(str, "TK_LT") == 0) {
        return TK_LT;
    }
    if (strcmp(str, "TK_LE") == 0) {
        return TK_LE;
    }
    if (strcmp(str, "TK_GT") == 0) {
        return TK_GT;
    }
    if (strcmp(str, "TK_GE") == 0) {
        return TK_GE;
    }
    if (strcmp(str, "TK_EQ") == 0) {
        return TK_EQ;
    }
    if (strcmp(str, "TK_NE") == 0) {
        return TK_NE;
    }
    if (strcmp(str, "TK_SWITCH") == 0) {
        return TK_SWITCH;
    }
    if (strcmp(str, "TK_CASE") == 0) {
        return TK_CASE;
    }
    if (strcmp(str, "TK_BREAK") == 0) {
        return TK_BREAK;
    }
    if (strcmp(str, "TK_DEFAULT") == 0) {
        return TK_DEFAULT;
    }
    if (strcmp(str, "TK_FOR") == 0) {
        return TK_FOR;
    }
    if (strcmp(str, "TK_IN") == 0) {
        return TK_IN;
    }
    if (strcmp(str, "TK_WHILE") == 0) {
        return TK_WHILE;
    }
    if (strcmp(str, "TK_RANGEOP") == 0) {
        return TK_RANGEOP;
    }
    return TK_ERROR;
}

/*
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
    TK_DRIVERDEF,
    TK_DRIVERENDDEF,
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
    TK_INTEGER,
    TK_REAL,
    TK_EPSILON,
    TK_EOF,
    TK_ERROR,
    TK_NONE,
}term;
*/