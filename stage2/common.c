#include "common.h"

bool is_terminal(int num) {
	if ((term)num == TK_DOLLAR || (num >= 0 && num <= 56)) {
		return true;
	}
	return false;
}

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
        case N1:
        return "N1";
        break;
        case output_plist:
        return "output_plist";
        break;
        case N2:
        return "N2";
        break;
        case dataType:
        return "dataType";
        break;
        case range_arrays:
        return "range_arrays";
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
        case boolConstt:
        return "boolConstt";
        break;
        case var_id_num:
        return "var_id_num";
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
        case g_index:
        return "g_index";
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
        case N3:
        return "N3";
        break;
        case expression:
        return "expression";
        break;
        case U:
        return "U";
        break;
        case new_NT:
        return "new_NT";
        break;
        case unary_op:
        return "unary_op";
        break;
        case arithmeticOrBooleanExpr:
        return "arithmeticOrBooleanExpr";
        break;
        case N7:
        return "N7";
        break;
        case AnyTerm:
        return "AnyTerm";
        break;
        case N8:
        return "N8";
        break;
        case arithmeticExpr:
        return "arithmeticExpr";
        break;
        case N4:
        return "N4";
        break;
        case g_term:
        return "g_term";
        break;
        case N5:
        return "N5";
        break;
        case factor:
        return "factor";
        break;
        case op1:
        return "op1";
        break;
        case op2:
        return "op2";
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
        case condionalStmt:
        return "condionalStmt";
        break;
        case caseStmts:
        return "caseStmts";
        break;
        case N9:
        return "N9";
        break;
        case value:
        return "value";
        break;
        case g_default:
        return "g_default";
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
        case TK_DOLLAR:
        return "TK_DOLLAR";
        break;
        default:
        return NULL;
    }
}

term get_term_from_int(int term_int) {
    if (term_int == -1) {
        return TK_ERROR;
    }
    return (term)term_int;
}

term get_term_from_string(char* str) {
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
    if (strcmp(str, "<N1>") == 0) {
        return N1;
    }
    if (strcmp(str, "<output_plist>") == 0) {
        return output_plist;
    }
    if (strcmp(str, "<N2>") == 0) {
        return N2;
    }
    if (strcmp(str, "<dataType>") == 0) {
        return dataType;
    }
    if (strcmp(str, "<range_arrays>") == 0) {
        return range_arrays;
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
    if (strcmp(str, "<boolConstt>") == 0) {
        return boolConstt;
    }
    if (strcmp(str, "<var_id_num>") == 0) {
        return var_id_num;
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
    if (strcmp(str, "<g_index>") == 0) {
        return g_index;
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
    if (strcmp(str, "<N3>") == 0) {
        return N3;
    }
    if (strcmp(str, "<expression>") == 0) {
        return expression;
    }
    if (strcmp(str, "<U>") == 0) {
        return U;
    }
    if (strcmp(str, "<new_NT>") == 0) {
        return new_NT;
    }
    if (strcmp(str, "<unary_op>") == 0) {
        return unary_op;
    }
    if (strcmp(str, "<arithmeticOrBooleanExpr>") == 0) {
        return arithmeticOrBooleanExpr;
    }
    if (strcmp(str, "<N7>") == 0) {
        return N7;
    }
    if (strcmp(str, "<AnyTerm>") == 0) {
        return AnyTerm;
    }
    if (strcmp(str, "<N8>") == 0) {
        return N8;
    }
    if (strcmp(str, "<arithmeticExpr>") == 0) {
        return arithmeticExpr;
    }
    if (strcmp(str, "<N4>") == 0) {
        return N4;
    }
    if (strcmp(str, "<g_term>") == 0) {
        return g_term;
    }
    if (strcmp(str, "<N5>") == 0) {
        return N5;
    }
    if (strcmp(str, "<factor>") == 0) {
        return factor;
    }
    if (strcmp(str, "<op1>") == 0) {
        return op1;
    }
    if (strcmp(str, "<op2>") == 0) {
        return op2;
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
    if (strcmp(str, "<condionalStmt>") == 0) {
        return condionalStmt;
    }
    if (strcmp(str, "<caseStmts>") == 0) {
        return caseStmts;
    }
    if (strcmp(str, "<N9>") == 0) {
        return N9;
    }
    if (strcmp(str, "<value>") == 0) {
        return value;
    }
    if (strcmp(str, "<g_default>") == 0) {
        return g_default;
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
    if (strcmp(str, "TK_DOLLAR") == 0) {
        return TK_DOLLAR;
    }
    return TK_ERROR;
}
