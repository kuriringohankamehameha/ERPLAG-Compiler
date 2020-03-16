#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// Reduce Boilerplate code
#define AST_COND(root, gterm, num) if (root->token.token_type == gterm && root->num_lhs == num)

ASTNode* make_ASTNode(ASTNode* child, term token_type) {
    // Allocates memory for an AST Node, with a child
    ASTNode* node = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->token_type = token_type;
    node->parent = NULL;
    node->children = (ASTNode**) calloc (1, sizeof(ASTNode*));
    node->children[0] = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->children[0] = child;
    node->num_children = 1;
    return node;
}

void add_ASTChild(ASTNode* parent, ASTNode* child) {
    // Adds a child node to the parent AST Node
    if (parent->num_children <= 0) {
        parent->children = (ASTNode**) calloc (1, sizeof(ASTNode*));
        parent->children[0] = (ASTNode*) calloc (1, sizeof(ASTNode));
        parent->children[0] = child;
        parent->num_children = 1;
    }
    else {
        parent->children = (ASTNode**) realloc (parent->children, (parent->num_children + 1) * sizeof(ASTNode*));
        parent->children[parent->num_children] = child;
        parent->num_children++;
    }
}

ASTNode* make_ASTLeaf(ASTNode* parent, Token token) {
    // Creates a new leaf node, using the token from the Lexer
    ASTNode* node = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->token = token;
    node->token_type = token.token_type;
    node->parent = parent;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

ASTNode* generate_AST(TreeNode* root) {
    // Generates the AST from the Parse Tree
    AST_COND(root, program, 0) {
        // <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
    
    }
    else AST_COND(root, moduleDeclarations, 0) {
        // <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>
    
    }
    else AST_COND(root, moduleDeclarations, 1) {
        // <moduleDeclarations> -> E
    
    }
    else AST_COND(root, moduleDeclaration, 0) {
        // <moduleDeclaration> -> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL
    
    }
    else AST_COND(root, otherModules, 0) {
        // <otherModules> -> <module> <otherModules>
    
    }
    else AST_COND(root, otherModules, 1) {
        // <otherModules> -> E
    
    }
    else AST_COND(root, driverModule, 0) {
        // <driverModule> -> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>
    
    }
    else AST_COND(root, module, 0) {
        // <module> -> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>
    
    }
    else AST_COND(root, ret, 0) {
        // <ret> -> TK_RETURNS TK_SQBO <output_plist> TK_SQBC TK_SEMICOL
    
    }
    else AST_COND(root, ret, 1) {
        // <ret> -> E
    
    }
    else AST_COND(root, input_plist, 0) {
        // <input_plist> -> TK_ID TK_COLON <dataType> <N1>
    
    }
    else AST_COND(root, N1, 0) {
        // <N1> -> TK_COMMA TK_ID TK_COLON <dataType> <N1>
    
    }
    else AST_COND(root, N1, 1) {
        // <N1> -> E
    
    }
    else AST_COND(root, output_plist, 0) {
        // <output_plist> -> TK_ID TK_COLON <type> <N2>
    
    }
    else AST_COND(root, N2, 0) {
        // <N2> -> TK_COMMA TK_ID TK_COLON <type> <N2>
    
    }
    else AST_COND(root, N2, 1) {
        // <N2> -> E
    
    }
    else AST_COND(root, dataType, 0) {
        // <dataType> -> TK_INTEGER
    
    }
    else AST_COND(root, dataType, 1) {
        // <dataType> -> TK_REAL
    
    }
    else AST_COND(root, dataType, 2) {
        // <dataType> -> TK_BOOLEAN
    
    }
    else AST_COND(root, dataType, 3) {
        // <dataType> -> TK_ARRAY TK_SQBO <range_arrays> TK_SQBC TK_OF <type>
    
    }
    else AST_COND(root, range_arrays, 0) {
        // <range_arrays> -> <g_index> TK_RANGEOP <g_index>
    
    }
    else AST_COND(root, type, 0) {
        // <type> -> TK_INTEGER
    
    }
    else AST_COND(root, type, 1) {
        // <type> -> TK_REAL
    
    }
    else AST_COND(root, type, 2) {
        // <type> -> TK_BOOLEAN
    
    }
    else AST_COND(root, moduleDef, 0) {
        // <moduleDef> -> TK_START <statements> TK_END
    
    }
    else AST_COND(root, statements, 0) {
        // <statements> -> <statement> <statements>
    
    }
    else AST_COND(root, statements, 1) {
        // <statements> -> E
    
    }
    else AST_COND(root, statement, 0) {
        // <statement> -> <ioStmt>
    
    }
    else AST_COND(root, statement, 1) {
        // <statement> -> <simpleStmt>
    
    }
    else AST_COND(root, statement, 2) {
        // <statement> -> <declareStmt>
    
    }
    else AST_COND(root, statement, 3) {
        // <statement> -> <condionalStmt>
    
    }
    else AST_COND(root, statement, 4) {
        // <statement> -> <iterativeStmt>
    
    }
    else AST_COND(root, ioStmt, 0) {
        // <ioStmt> -> TK_GET_VALUE TK_BO TK_ID TK_BC TK_SEMICOL
    
    }
    else AST_COND(root, ioStmt, 1) {
        // <ioStmt> -> TK_PRINT TK_BO <var> TK_BC TK_SEMICOL
    
    }
    else AST_COND(root, var, 0) {
        // <var> -> <var_id_num>
    
    }
    else AST_COND(root, var, 1) {
        // <var> -> <boolConstt>
    
    }
    else AST_COND(root, boolConstt, 0) {
        // <boolConstt> -> TK_TRUE
    
    }
    else AST_COND(root, boolConstt, 1) {
        // <boolConstt> -> TK_FALSE
    
    }
    else AST_COND(root, var_id_num, 0) {
        // <var_id_num> -> TK_ID <whichId>
    
    }
    else AST_COND(root, var_id_num, 1) {
        // <var_id_num> -> TK_NUM
    
    }
    else AST_COND(root, var_id_num, 2) {
        // <var_id_num> -> TK_RNUM
    
    }
    else AST_COND(root, whichId, 0) {
        // <whichId> -> TK_SQBO <g_index> TK_SQBC
    
    }
    else AST_COND(root, whichId, 1) {
        // <whichId> -> E
    
    }
    else AST_COND(root, simpleStmt, 0) {
        // <simpleStmt> -> <assignmentStmt>
    
    }
    else AST_COND(root, simpleStmt, 1) {
        // <simpleStmt> -> <moduleReuseStmt>
    
    }
    else AST_COND(root, assignmentStmt, 0) {
        // <assignmentStmt> -> TK_ID <whichStmt>
    
    }
    else AST_COND(root, whichStmt, 0) {
        // <whichStmt> -> <lvalueIDStmt>
    
    }
    else AST_COND(root, whichStmt, 1) {
        // <whichStmt> -> <lvalueARRStmt>
    
    }
    else AST_COND(root, lvalueIDStmt, 0) {
        // <lvalueIDStmt> -> TK_ASSIGNOP <expression> TK_SEMICOL
    
    }
    else AST_COND(root, lvalueARRStmt, 0) {
        // <lvalueARRStmt> -> TK_SQBO <g_index> TK_SQBC TK_ASSIGNOP <expression> TK_SEMICOL
    
    }
    else AST_COND(root, g_index, 0) {
        // <g_index> -> TK_NUM
    
    }
    else AST_COND(root, g_index, 1) {
        // <g_index> -> TK_ID
    
    }
    else AST_COND(root, moduleReuseStmt, 0) {
        // <moduleReuseStmt> -> <optional> TK_USE TK_MODULE TK_ID TK_WITH TK_PARAMETERS <idList> TK_SEMICOL
    
    }
    else AST_COND(root, optional, 0) {
        // <optional> -> TK_SQBO <idList> TK_SQBC TK_ASSIGNOP
    
    }
    else AST_COND(root, optional, 1) {
        // <optional> -> E
    
    }
    else AST_COND(root, idList, 0) {
        // <idList> -> TK_ID <N3>
    
    }
    else AST_COND(root, N3, 0) {
        // <N3> -> TK_COMMA TK_ID <N3>
    
    }
    else AST_COND(root, N3, 1) {
        // <N3> -> E
    
    }
    else AST_COND(root, expression, 0) {
        // <expression> -> <arithmeticOrBooleanExpr>
    
    }
    else AST_COND(root, expression, 1) {
        // <expression> -> <U>
    
    }
    else AST_COND(root, U, 0) {
        // <U> -> <unary_op> <new_NT>
    
    }
    else AST_COND(root, new_NT, 0) {
        // <new_NT> -> TK_BO <arithmeticExpr> TK_BC
    
    }
    else AST_COND(root, new_NT, 1) {
        // <new_NT> -> <var_id_num>
    
    }
    else AST_COND(root, unary_op, 0) {
        // <unary_op> -> TK_PLUS
    
    }
    else AST_COND(root, unary_op, 1) {
        // <unary_op> -> TK_MINUS
    
    }
    else AST_COND(root, arithmeticOrBooleanExpr, 0) {
        // <arithmeticOrBooleanExpr> -> <AnyTerm> <N7>
    
    }
    else AST_COND(root, N7, 0) {
        // <N7> -> <logicalOp> <AnyTerm> <N7>
    
    }
    else AST_COND(root, N7, 1) {
        // <N7> -> E
    
    }
    else AST_COND(root, AnyTerm, 0) {
        // <AnyTerm> -> <arithmeticExpr> <N8>
    
    }
    else AST_COND(root, AnyTerm, 1) {
        // <AnyTerm> -> <boolConstt>
    
    }
    else AST_COND(root, N8, 0) {
        // <N8> -> <relationalOp> <arithmeticExpr>
    
    }
    else AST_COND(root, N8, 1) {
        // <N8> -> E
    
    }
    else AST_COND(root, arithmeticExpr, 0) {
        // <arithmeticExpr> -> <g_term> <N4>
    
    }
    else AST_COND(root, N4, 0) {
        // <N4> -> <op1> <g_term> <N4>
    
    }
    else AST_COND(root, N4, 1) {
        // <N4> -> E
    
    }
    else AST_COND(root, g_term, 0) {
        // <g_term> -> <factor> <N5>
    
    }
    else AST_COND(root, N5, 0) {
        // <N5> -> <op2> <factor> <N5>
    
    }
    else AST_COND(root, N5, 1) {
        // <N5> -> E
    
    }
    else AST_COND(root, factor, 0) {
        // <factor> -> TK_BO <arithmeticOrBooleanExpr> TK_BC
    
    }
    else AST_COND(root, factor, 1) {
        // <factor> -> <var_id_num>
    
    }
    else AST_COND(root, op1, 0) {
        // <op1> -> TK_PLUS
    
    }
    else AST_COND(root, op1, 1) {
        // <op1> -> TK_MINUS
    
    }
    else AST_COND(root, op2, 0) {
        // <op2> -> TK_MUL
    
    }
    else AST_COND(root, op2, 1) {
        // <op2> -> TK_DIV
    
    }
    else AST_COND(root, logicalOp, 0) {
        // <logicalOp> -> TK_AND
    
    }
    else AST_COND(root, logicalOp, 1) {
        // <logicalOp> -> TK_OR
    
    }
    else AST_COND(root, relationalOp, 0) {
        // <relationalOp> -> TK_LT
    
    }
    else AST_COND(root, relationalOp, 1) {
        // <relationalOp> -> TK_LE
    
    }
    else AST_COND(root, relationalOp, 2) {
        // <relationalOp> -> TK_GT
    
    }
    else AST_COND(root, relationalOp, 3) {
        // <relationalOp> -> TK_GE
    
    }
    else AST_COND(root, relationalOp, 4) {
        // <relationalOp> -> TK_EQ
    
    }
    else AST_COND(root, relationalOp, 5) {
        // <relationalOp> -> TK_NE
    
    }
    else AST_COND(root, declareStmt, 0) {
        // <declareStmt> -> TK_DECLARE <idList> TK_COLON <dataType> TK_SEMICOL
    
    }
    else AST_COND(root, condionalStmt, 0) {
        // <condionalStmt> -> TK_SWITCH TK_BO TK_ID TK_BC TK_START <caseStmts> <g_default> TK_END
    
    }
    else AST_COND(root, caseStmts, 0) {
        // <caseStmts> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
    
    }
    else AST_COND(root, N9, 0) {
        // <N9> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
    
    }
    else AST_COND(root, N9, 1) {
        // <N9> -> E
    
    }
    else AST_COND(root, value, 0) {
        // <value> -> TK_NUM
    
    }
    else AST_COND(root, value, 1) {
        // <value> -> TK_TRUE
    
    }
    else AST_COND(root, value, 2) {
        // <value> -> TK_FALSE
    
    }
    else AST_COND(root, g_default, 0) {
        // <g_default> -> TK_DEFAULT TK_COLON <statements> TK_BREAK TK_SEMICOL
    
    }
    else AST_COND(root, g_default, 1) {
        // <g_default> -> E
    
    }
    else AST_COND(root, iterativeStmt, 0) {
        // <iterativeStmt> -> TK_FOR TK_BO TK_ID TK_IN <range> TK_BC TK_START <statements> TK_END
    
    }
    else AST_COND(root, iterativeStmt, 1) {
        // <iterativeStmt> -> TK_WHILE TK_BO <arithmeticOrBooleanExpr> TK_BC TK_START <statements> TK_END
    
    }
    else AST_COND(root, range, 0) {
        // <range> -> TK_NUM TK_RANGEOP TK_NUM
    
    }
    else {

    }
}
