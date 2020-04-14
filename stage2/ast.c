#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// Reduce Boilerplate code
#define AST_COND(root, gterm, tk_type) (root->token.token_type == gterm && root->children[0]->token.token_type == tk_type)

ASTNode* make_ASTNode(ASTNode* child, term token_type)
{
    // Allocates memory for an AST Node, with a child
    ASTNode* node = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->token_type = token_type;
    node->parent = NULL;
    node->children = (ASTNode**) calloc (1, sizeof(ASTNode*));
    node->children[0] = child;
    child->parent = node;
    node->num_children = 1;
    return node;
}

void add_ASTChild(ASTNode* parent, ASTNode* child)
{
    // Adds a child node to the parent AST Node
    if (parent->num_children <= 0)
    {
        parent->children = (ASTNode**) calloc (1, sizeof(ASTNode*));
        parent->children[0] = child;
        parent->num_children = 1;
        child->parent = parent;
    }
    else
    {
        parent->children = (ASTNode**) realloc (parent->children, (parent->num_children + 1) * sizeof(ASTNode*));
        parent->children[parent->num_children] = child;
        parent->num_children++;
        child->parent = parent;
    }
}

ASTNode* make_ASTLeaf(ASTNode* parent, Token token)
{
    // Creates a new leaf node, using the token from the Lexer
    ASTNode* node = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->token = token;
    node->token_type = token.token_type;
    node->parent = parent;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

void generate_AST(TreeNode* root)
{
    if (!root || !(root->children)) {
        return;
    }
    if AST_COND(root, program, moduleDeclarations)
    {
        // <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
        // printf("<program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>\n");
        TreeNode *moduleDeclarationsNode = root->children[0];
        TreeNode *otherModulesNode = root->children[1];
        TreeNode *driverModuleNode = root->children[2];
        TreeNode *otherModules2Node = root->children[3];
        generate_AST(moduleDeclarationsNode);
        generate_AST(otherModulesNode);
        generate_AST(driverModuleNode);
        generate_AST(otherModules2Node);
        if (moduleDeclarationsNode->node) {
            root->node = make_ASTNode(moduleDeclarationsNode->node, program);
            if (otherModulesNode->node)
                add_ASTChild(root->node, otherModulesNode->node);
            if (driverModuleNode->node)
                add_ASTChild(root->node, driverModuleNode->node);
            if (otherModules2Node->node)
                add_ASTChild(root->node, otherModules2Node->node);
        }
        else {
            if (otherModulesNode->node) {
                root->node = make_ASTNode(otherModulesNode->node, program);
                if (driverModuleNode->node)
                    add_ASTChild(root->node, driverModuleNode->node);
                if (otherModules2Node->node)
                    add_ASTChild(root->node, otherModules2Node->node);
            }
            else {
                root->node = make_ASTNode(driverModuleNode->node, program);
                if (otherModules2Node->node)
                    add_ASTChild(root->node, otherModules2Node->node);
            }
        }
    }
    else if AST_COND(root, moduleDeclarations, moduleDeclaration)
    {
        // <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>
        // printf("<moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>\n");
        TreeNode *moduleDeclarationNode = root->children[0];
        TreeNode *moduleDeclarationsNode = root->children[1];
        generate_AST(moduleDeclarationNode);
        generate_AST(moduleDeclarationsNode);
        root->node = make_ASTNode(moduleDeclarationNode->node, moduleDeclarations);
        if (moduleDeclarationsNode->node)
            add_ASTChild(root->node, moduleDeclarationsNode->node);
    }
    else if AST_COND(root, moduleDeclarations, TK_EPSILON)
    {
        // <moduleDeclarations> -> E
        // printf("<moduleDeclarations> -> E\n");
        root->node = NULL;
    }
    else if AST_COND(root, moduleDeclaration, TK_DECLARE)
    {
        // <moduleDeclaration> -> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL
        // printf("<moduleDeclaration> -> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL\n");
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), moduleDeclaration);
    }
    else if AST_COND(root, otherModules, module)
    {
        // <otherModules> -> <module> <otherModules>
        // printf("<otherModules> -> <module> <otherModules>\n");
        TreeNode *moduleNode = root->children[0];
        TreeNode *otherModulesNode = root->children[1];
        generate_AST(moduleNode);
        generate_AST(otherModulesNode);
        if (otherModulesNode->node) {
            root->node = make_ASTNode(moduleNode->node, otherModules);
            add_ASTChild(root->node, otherModulesNode->node);
        }
        else {
            root->node = moduleNode->node;
        }
    }
    else if AST_COND(root, otherModules, TK_EPSILON)
    {
        // <otherModules> -> E
        // printf("<otherModules> -> E\n");
        root->node = NULL;
    }
    else if AST_COND(root, driverModule, TK_DRIVERDEF)
    {
        // <driverModule> -> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>
        // printf("<driverModule> -> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>\n");
        TreeNode *moduleDefNode = root->children[4];
        generate_AST(moduleDefNode);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[1]->token), driverModule);
        add_ASTChild(root->node, moduleDefNode->node);
    }
    else if AST_COND(root, module, TK_DEF)
    {
        // <module> -> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>
        // printf("<module> -> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>\n");
        TreeNode *input_plistNode = root->children[7];
        TreeNode *retNode = root->children[10];
        TreeNode *moduleDefNode = root->children[11];
        generate_AST(input_plistNode);
        generate_AST(retNode);
        generate_AST(moduleDefNode);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), module);
        if (input_plistNode->node)
            add_ASTChild(root->node, input_plistNode->node);
        if (retNode->node)
            add_ASTChild(root->node, retNode->node);
        if (moduleDefNode->node)
            add_ASTChild(root->node, moduleDefNode->node);
    }
    else if AST_COND(root, ret, TK_RETURNS)
    {
        // <ret> -> TK_RETURNS TK_SQBO <output_plist> TK_SQBC TK_SEMICOL
        TreeNode *output_plistNode = root->children[2];
        generate_AST(output_plistNode);
        root->node = output_plistNode->node;
    }
    else if AST_COND(root, ret, TK_EPSILON)
    {
        // <ret> -> E
        root->node = NULL;
    }
    else if AST_COND(root, input_plist, TK_ID)
    {
        // <input_plist> -> TK_ID TK_COLON <dataType> <N1>
        TreeNode *dataTypeNode = root->children[2];
        TreeNode *N1Node = root->children[3];
        generate_AST(dataTypeNode);
        generate_AST(N1Node);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), input_plist);
        if (dataTypeNode->node)
            add_ASTChild(root->node, dataTypeNode->node);
        if (N1Node->node)
            add_ASTChild(root->node, N1Node->node);
    }
    else if AST_COND(root, N1, TK_COMMA)
    {
        // <N1> -> TK_COMMA TK_ID TK_COLON <dataType> <N1>
        TreeNode *dataTypeNode = root->children[3];
        TreeNode *N1Node = root->children[4];
        generate_AST(dataTypeNode);
        generate_AST(N1Node);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[1]->token), N1);
        if (dataTypeNode->node)
            add_ASTChild(root->node, dataTypeNode->node);
        if (N1Node->node)
            add_ASTChild(root->node, N1Node->node);
    }
    else if AST_COND(root, N1, TK_EPSILON)
    {
        // <N1> -> E
        root->node = NULL;
    }
    else if AST_COND(root, output_plist, TK_ID)
    {
        // <output_plist> -> TK_ID TK_COLON <type> <N2>
        TreeNode *typeNode = root->children[2];
        TreeNode *N2Node = root->children[3];
        generate_AST(typeNode);
        generate_AST(N2Node);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), output_plist);
        if (typeNode->node)
            add_ASTChild(root->node, typeNode->node);
        if (N2Node->node)
            add_ASTChild(root->node, N2Node->node);
    }
    else if AST_COND(root, N2, TK_COMMA)
    {
        // <N2> -> TK_COMMA TK_ID TK_COLON <type> <N2>
        TreeNode *typeNode = root->children[3];
        TreeNode *N2Node = root->children[4];
        generate_AST(typeNode);
        generate_AST(N2Node);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[1]->token), N2);
        if (typeNode->node)
            add_ASTChild(root->node, typeNode->node);
        if (N2Node->node)
            add_ASTChild(root->node, N2Node->node);
    }
    else if AST_COND(root, N2, TK_EPSILON)
    {
        // <N2> -> E
        root->node = NULL;
    }
    else if AST_COND(root, dataType, TK_INTEGER)
    {
        // <dataType> -> TK_INTEGER
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
    }
    else if AST_COND(root, dataType, TK_REAL)
    {
        // <dataType> -> TK_REAL
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
    }
    else if AST_COND(root, dataType, TK_BOOLEAN)
    {
        // <dataType> -> TK_BOOLEAN
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
    }
    else if AST_COND(root, dataType, TK_ARRAY)
    {
        // <dataType> -> TK_ARRAY TK_SQBO <range_arrays> TK_SQBC TK_OF <type>
        TreeNode *range_arraysNode = root->children[2];
        TreeNode *typeNode = root->children[5];
        generate_AST(range_arraysNode);
        generate_AST(typeNode);
        // For TK_ARRAY
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
        add_ASTChild(root->node, range_arraysNode->node);
        if (typeNode->node)
            add_ASTChild(root->node, typeNode->node);
    }
    else if AST_COND(root, range_arrays, g_index)
    {
        // <range_arrays> -> <g_index> TK_RANGEOP <g_index>
        TreeNode *g_indexNode = root->children[0];
        TreeNode *g_index2Node = root->children[2];
        generate_AST(g_indexNode);
        generate_AST(g_index2Node);
        root->node = make_ASTNode(g_indexNode->node, range_arrays);
        add_ASTChild(root->node, g_index2Node->node);
    }
    else if AST_COND(root, type, TK_INTEGER)
    {
        // <type> -> TK_INTEGER
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, type, TK_REAL)
    {
        // <type> -> TK_REAL
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, type, TK_BOOLEAN)
    {
        // <type> -> TK_BOOLEAN
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, moduleDef, TK_START)
    {
        // printf("<moduleDef> -> TK_START <statements> TK_END\n");
        // <moduleDef> -> TK_START <statements> TK_END
        TreeNode *statementsNode = root->children[1];
        generate_AST(statementsNode);
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
        if (statementsNode->node)
            add_ASTChild(root->node, statementsNode->node);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[2]->token));
    }
    else if AST_COND(root, statements, statement)
    {
        // <statements> -> <statement> <statements>
        TreeNode *statementNode = root->children[0];
        TreeNode *statementsNode = root->children[1];
        generate_AST(statementNode);
        generate_AST(statementsNode);
        root->node = statementNode->node;
        if (statementsNode->node)
            add_ASTChild(root->node, statementsNode->node);
    }
    else if AST_COND(root, statements, TK_EPSILON)
    {
        // <statements> -> E
        root->node = NULL;
    }
    else if AST_COND(root, statement, ioStmt)
    {
        // <statement> -> <ioStmt>
        TreeNode *ioStmtNode = root->children[0];
        generate_AST(ioStmtNode);
        root->node = ioStmtNode->node;
    }
    else if AST_COND(root, statement, simpleStmt)
    {
        // <statement> -> <simpleStmt>
        TreeNode *simpleStmtNode = root->children[0];
        generate_AST(simpleStmtNode);
        root->node = simpleStmtNode->node;
    }
    else if AST_COND(root, statement, declareStmt)
    {
        // <statement> -> <declareStmt>
        TreeNode *declareStmtNode = root->children[0];
        generate_AST(declareStmtNode);
        root->node = declareStmtNode->node;
    }
    else if AST_COND(root, statement, condionalStmt)
    {
        // <statement> -> <condionalStmt>
        TreeNode *condionalStmtNode = root->children[0];
        generate_AST(condionalStmtNode);
        root->node = condionalStmtNode->node;
    }
    else if AST_COND(root, statement, iterativeStmt)
    {
        // <statement> -> <iterativeStmt>
        TreeNode *iterativeStmtNode = root->children[0];
        generate_AST(iterativeStmtNode);
        root->node = iterativeStmtNode->node;
    }
    else if AST_COND(root, ioStmt, TK_GET_VALUE)
    {
        // <ioStmt> -> TK_GET_VALUE TK_BO TK_ID TK_BC TK_SEMICOL
        root->node = make_ASTLeaf(NULL, root->children[2]->token);
    }
    else if AST_COND(root, ioStmt, TK_PRINT)
    {
        // <ioStmt> -> TK_PRINT TK_BO <var> TK_BC TK_SEMICOL
        TreeNode *varNode = root->children[2];
        generate_AST(varNode);
        // TK_PRINT is included in the AST
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), ioStmt);
        add_ASTChild(root->node, varNode->node);
        // root->node = make_ASTNode(varNode->node, ioStmt);
    }
    else if AST_COND(root, var, var_id_num)
    {
        // <var> -> <var_id_num>
        TreeNode *var_id_numNode = root->children[0];
        generate_AST(var_id_numNode);
        root->node = var_id_numNode->node;
    }
    else if AST_COND(root, var, boolConstt)
    {
        // <var> -> <boolConstt>
        TreeNode *boolConsttNode = root->children[0];
        generate_AST(boolConsttNode);
        root->node = boolConsttNode->node;
    }
    else if AST_COND(root, boolConstt, TK_TRUE)
    {
        // <boolConstt> -> TK_TRUE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, boolConstt, TK_FALSE)
    {
        // <boolConstt> -> TK_FALSE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, var_id_num, TK_ID)
    {
        // <var_id_num> -> TK_ID <whichId>
        TreeNode *whichIdNode = root->children[1];
        generate_AST(whichIdNode);
        if (whichIdNode->node) {
            root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), var_id_num);
            add_ASTChild(root->node, whichIdNode->node);
        }
        else {
            root->node = make_ASTLeaf(NULL, root->children[0]->token);
        }
    }
    else if AST_COND(root, var_id_num, TK_NUM)
    {
        // <var_id_num> -> TK_NUM
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, var_id_num, TK_RNUM)
    {
        // <var_id_num> -> TK_RNUM
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, whichId, TK_SQBO)
    {
        // <whichId> -> TK_SQBO <g_index> TK_SQBC
        TreeNode *g_indexNode = root->children[1];
        generate_AST(g_indexNode);
        root->node = g_indexNode->node;
    }
    else if AST_COND(root, whichId, TK_EPSILON)
    {
        // <whichId> -> E
        root->node = NULL;
    }
    else if AST_COND(root, simpleStmt, assignmentStmt)
    {
        // <simpleStmt> -> <assignmentStmt>
        TreeNode *assignmentStmtNode = root->children[0];
        generate_AST(assignmentStmtNode);
        root->node = assignmentStmtNode->node;
    }
    else if AST_COND(root, simpleStmt, moduleReuseStmt)
    {
        // <simpleStmt> -> <moduleReuseStmt>
        TreeNode *moduleReuseStmtNode = root->children[0];
        generate_AST(moduleReuseStmtNode);
        root->node = moduleReuseStmtNode->node;
    }
    else if AST_COND(root, assignmentStmt, TK_ID)
    {
        // <assignmentStmt> -> TK_ID <whichStmt>
        TreeNode *whichStmtNode = root->children[1];
        generate_AST(whichStmtNode);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), assignmentStmt);
        if (whichStmtNode->node)
            add_ASTChild(root->node, whichStmtNode->node);
    }
    else if AST_COND(root, whichStmt, lvalueIDStmt)
    {
        // <whichStmt> -> <lvalueIDStmt>
        TreeNode *lvalueIDStmtNode = root->children[0];
        generate_AST(lvalueIDStmtNode);
        root->node = lvalueIDStmtNode->node;
    }
    else if AST_COND(root, whichStmt, lvalueARRStmt)
    {
        // <whichStmt> -> <lvalueARRStmt>
        TreeNode *lvalueARRStmtNode = root->children[0];
        generate_AST(lvalueARRStmtNode);
        root->node = lvalueARRStmtNode->node;
    }
    else if AST_COND(root, lvalueIDStmt, TK_ASSIGNOP)
    {
        // <lvalueIDStmt> -> TK_ASSIGNOP <expression> TK_SEMICOL
        TreeNode *expressionNode = root->children[1];
        generate_AST(expressionNode);
        root->node = expressionNode->node;
    }
    else if AST_COND(root, lvalueARRStmt, TK_SQBO)
    {
        // <lvalueARRStmt> -> TK_SQBO <g_index> TK_SQBC TK_ASSIGNOP <expression> TK_SEMICOL
        TreeNode *g_indexNode = root->children[1];
        TreeNode *expressionNode = root->children[4];
        generate_AST(g_indexNode);
        generate_AST(expressionNode);
        root->node = make_ASTNode(g_indexNode->node, lvalueARRStmt);
        if (expressionNode->node)
            add_ASTChild(root->node, expressionNode->node);
    }
    else if AST_COND(root, g_index, TK_NUM)
    {
        // <g_index> -> TK_NUM
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, g_index, TK_ID)
    {
        // <g_index> -> TK_ID
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, moduleReuseStmt, optional)
    {
        // <moduleReuseStmt> -> <optional> TK_USE TK_MODULE TK_ID TK_WITH TK_PARAMETERS <idList> TK_SEMICOL
        TreeNode *optionalNode = root->children[0];
        TreeNode *idListNode = root->children[6];
        generate_AST(optionalNode);
        generate_AST(idListNode);
        if (optionalNode->node) {
            root->node = make_ASTNode(optionalNode->node, moduleReuseStmt);
            add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[3]->token));
        }
        else {
            root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[3]->token), moduleReuseStmt);
        }
        if (idListNode->node)
            add_ASTChild(root->node, idListNode->node);
    }
    else if AST_COND(root, optional, TK_SQBO)
    {
        // <optional> -> TK_SQBO <idList> TK_SQBC TK_ASSIGNOP
        TreeNode *idListNode = root->children[1];
        generate_AST(idListNode);
        root->node = idListNode->node;
    }
    else if AST_COND(root, optional, TK_EPSILON)
    {
        // <optional> -> E
        root->node = NULL;
    }
    else if AST_COND(root, idList, TK_ID)
    {
        // <idList> -> TK_ID <N3>
        TreeNode *N3Node = root->children[1];
        generate_AST(N3Node);
        if (N3Node->node) {
            root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), idList);
            add_ASTChild(root->node, N3Node->node);
        }
        else {
            root->node = make_ASTLeaf(NULL, root->children[0]->token);
        }
    }
    else if AST_COND(root, N3, TK_COMMA)
    {
        // <N3> -> TK_COMMA TK_ID <N3>
        TreeNode *N3Node = root->children[2];
        generate_AST(N3Node);
        if (N3Node->node) {
            root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[1]->token), N3);
            add_ASTChild(root->node, N3Node->node);
        }
        else {
            root->node = make_ASTLeaf(NULL, root->children[1]->token);
        }
    }
    else if AST_COND(root, N3, TK_EPSILON)
    {
        // <N3> -> E
        root->node = NULL;
    }
    else if AST_COND(root, expression, arithmeticOrBooleanExpr)
    {
        // <expression> -> <arithmeticOrBooleanExpr>
        TreeNode *arithmeticOrBooleanExprNode = root->children[0];
        generate_AST(arithmeticOrBooleanExprNode);
        root->node = arithmeticOrBooleanExprNode->node;
    }
    else if AST_COND(root, expression, U)
    {
        // <expression> -> <U>
        TreeNode *UNode = root->children[0];
        generate_AST(UNode);
        root->node = UNode->node;
    }
    else if AST_COND(root, U, unary_op)
    {
        // <U> -> <unary_op> <new_NT>
        TreeNode *unary_opNode = root->children[0];
        TreeNode *new_NTNode = root->children[1];
        generate_AST(unary_opNode);
        generate_AST(new_NTNode);
        root->node = make_ASTNode(unary_opNode->node, U);
        if (new_NTNode->node)
            add_ASTChild(root->node, new_NTNode->node);
    }
    else if AST_COND(root, new_NT, TK_BO)
    {
        // <new_NT> -> TK_BO <arithmeticExpr> TK_BC
        TreeNode *arithmeticExprNode = root->children[1];
        generate_AST(arithmeticExprNode);
        root->node = arithmeticExprNode->node;
    }
    else if AST_COND(root, new_NT, var_id_num)
    {
        // <new_NT> -> <var_id_num>
        TreeNode *var_id_numNode = root->children[0];
        generate_AST(var_id_numNode);
        root->node = var_id_numNode->node;
    }
    else if AST_COND(root, unary_op, TK_PLUS)
    {
        // <unary_op> -> TK_PLUS
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, unary_op, TK_MINUS)
    {
        // <unary_op> -> TK_MINUS
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, arithmeticOrBooleanExpr, AnyTerm)
    {
        // <arithmeticOrBooleanExpr> -> <AnyTerm> <N7>
        TreeNode *AnyTermNode = root->children[0];
        TreeNode *N7Node = root->children[1];
        generate_AST(AnyTermNode);
        generate_AST(N7Node);
        if (N7Node->node) {
            root->node = make_ASTNode(AnyTermNode->node, arithmeticOrBooleanExpr);
            add_ASTChild(root->node, N7Node->node);
        }
        else {
            root->node = AnyTermNode->node;
        }
    }
    else if AST_COND(root, N7, logicalOp)
    {
        // <N7> -> <logicalOp> <AnyTerm> <N7>
        TreeNode *logicalOpNode = root->children[0];
        TreeNode *AnyTermNode = root->children[1];
        TreeNode *N7Node = root->children[2];
        generate_AST(logicalOpNode);
        generate_AST(AnyTermNode);
        generate_AST(N7Node);
        root->node = make_ASTNode(logicalOpNode->node, N7);
        if (AnyTermNode->node)
            add_ASTChild(root->node, AnyTermNode->node);
        if (N7Node->node)
            add_ASTChild(root->node, N7Node->node);
    }
    else if AST_COND(root, N7, TK_EPSILON)
    {
        // <N7> -> E
        root->node = NULL;
    }
    else if AST_COND(root, AnyTerm, arithmeticExpr)
    {
        // <AnyTerm> -> <arithmeticExpr> <N8>
        TreeNode *arithmeticExprNode = root->children[0];
        TreeNode *N8Node = root->children[1];
        generate_AST(arithmeticExprNode);
        generate_AST(N8Node);
        if (N8Node->node) {
            root->node = make_ASTNode(arithmeticExprNode->node, AnyTerm);
            add_ASTChild(root->node, N8Node->node);
        }
        else {
            root->node = arithmeticExprNode->node;
        }
    }
    else if AST_COND(root, AnyTerm, boolConstt)
    {
        // <AnyTerm> -> <boolConstt>
        TreeNode *boolConsttNode = root->children[0];
        generate_AST(boolConsttNode);
        root->node = boolConsttNode->node;
    }
    else if AST_COND(root, N8, relationalOp)
    {
        // <N8> -> <relationalOp> <arithmeticExpr>
        TreeNode *relationalOpNode = root->children[0];
        TreeNode *arithmeticExprNode = root->children[1];
        generate_AST(relationalOpNode);
        generate_AST(arithmeticExprNode);
        if (arithmeticExprNode->node) {
            root->node = make_ASTNode(relationalOpNode->node, N8);
            add_ASTChild(root->node, arithmeticExprNode->node);
        }
        else {
            root->node = relationalOpNode->node;
        }
    }
    else if AST_COND(root, N8, TK_EPSILON)
    {
        // <N8> -> E
        root->node = NULL;
    }
    else if AST_COND(root, arithmeticExpr, g_term)
    {
        // <arithmeticExpr> -> <g_term> <N4>
        TreeNode *g_termNode = root->children[0];
        TreeNode *N4Node = root->children[1];
        generate_AST(g_termNode);
        generate_AST(N4Node);
        if (N4Node->node) {
            root->node = make_ASTNode(g_termNode->node, arithmeticExpr);
            add_ASTChild(root->node, N4Node->node);
        }
        else {
            root->node = g_termNode->node;
        }
    }
    else if AST_COND(root, N4, op1)
    {
        // <N4> -> <op1> <g_term> <N4>
        TreeNode *op1Node = root->children[0];
        TreeNode *g_termNode = root->children[1];
        TreeNode *N4Node = root->children[2];
        //generate_AST(op1Node);
        generate_AST(g_termNode);
        generate_AST(N4Node);
        //root->node = make_ASTNode(op1Node->node, N4);
        term syn_token = op1Node->children[0]->token.token_type;
        root->node = make_ASTNode(g_termNode->node, N4);
        root->node->syn_attribute.token_type = syn_token;
        //add_ASTChild(root->node, g_termNode->node);
        if (N4Node->node)
            add_ASTChild(root->node, N4Node->node);
    }
    else if AST_COND(root, N4, TK_EPSILON)
    {
        // <N4> -> E
        root->node = NULL;
    }
    else if AST_COND(root, g_term, factor)
    {
        // <g_term> -> <factor> <N5>
        TreeNode *factorNode = root->children[0];
        TreeNode *N5Node = root->children[1];
        generate_AST(factorNode);
        generate_AST(N5Node);
        if (N5Node->node) {
            root->node = make_ASTNode(factorNode->node, g_term);
            add_ASTChild(root->node, N5Node->node);
        }
        else {
            root->node = factorNode->node;
        }
    }
    else if AST_COND(root, N5, op2)
    {
        // <N5> -> <op2> <factor> <N5>
        TreeNode *op2Node = root->children[0];
        TreeNode *factorNode = root->children[1];
        TreeNode *N5Node = root->children[2];
        //generate_AST(op2Node);
        generate_AST(factorNode);
        generate_AST(N5Node);
        //root->node = make_ASTNode(op2Node->node, N5);
        term syn_token = op2Node->children[0]->token.token_type;
        root->node = make_ASTNode(factorNode->node, N5);
        root->node->syn_attribute.token_type = syn_token;
        if (N5Node->node)
            add_ASTChild(root->node, N5Node->node);
    }
    else if AST_COND(root, N5, TK_EPSILON)
    {
        // <N5> -> E
        root->node = NULL;
    }
    else if AST_COND(root, factor, TK_BO)
    {
        // <factor> -> TK_BO <arithmeticOrBooleanExpr> TK_BC
        TreeNode *arithmeticOrBooleanExprNode = root->children[1];
        generate_AST(arithmeticOrBooleanExprNode);
        root->node = arithmeticOrBooleanExprNode->node;
    }
    else if AST_COND(root, factor, var_id_num)
    {
        // <factor> -> <var_id_num>
        TreeNode *var_id_numNode = root->children[0];
        generate_AST(var_id_numNode);
        root->node = var_id_numNode->node;
    }
    else if AST_COND(root, op1, TK_PLUS)
    {
        // <op1> -> TK_PLUS
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, op1, TK_MINUS)
    {
        // <op1> -> TK_MINUS
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, op2, TK_MUL)
    {
        // <op2> -> TK_MUL
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, op2, TK_DIV)
    {
        // <op2> -> TK_DIV
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, logicalOp, TK_AND)
    {
        // <logicalOp> -> TK_AND
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, logicalOp, TK_OR)
    {
        // <logicalOp> -> TK_OR
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_LT)
    {
        // <relationalOp> -> TK_LT
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_LE)
    {
        // <relationalOp> -> TK_LE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_GT)
    {
        // <relationalOp> -> TK_GT
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_GE)
    {
        // <relationalOp> -> TK_GE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_EQ)
    {
        // <relationalOp> -> TK_EQ
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, relationalOp, TK_NE)
    {
        // <relationalOp> -> TK_NE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, declareStmt, TK_DECLARE)
    {
        // <declareStmt> -> TK_DECLARE <idList> TK_COLON <dataType> TK_SEMICOL
        TreeNode *idListNode = root->children[1];
        TreeNode *dataTypeNode = root->children[3];
        generate_AST(idListNode);
        generate_AST(dataTypeNode);
        root->node = make_ASTNode(idListNode->node, declareStmt);
        if (dataTypeNode->node)
            add_ASTChild(root->node, dataTypeNode->node);
    }
    else if AST_COND(root, condionalStmt, TK_SWITCH)
    {
        // <condionalStmt> -> TK_SWITCH TK_BO TK_ID TK_BC TK_START <caseStmts> <g_default> TK_END
        TreeNode *caseStmtsNode = root->children[5];
        TreeNode *g_defaultNode = root->children[6];
        generate_AST(caseStmtsNode);
        generate_AST(g_defaultNode);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), condionalStmt);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[4]->token));
        if (caseStmtsNode->node)
            add_ASTChild(root->node, caseStmtsNode->node);
        if (g_defaultNode->node)
            add_ASTChild(root->node, g_defaultNode->node);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[7]->token));
    }
    else if AST_COND(root, caseStmts, TK_CASE)
    {
        // <caseStmts> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
        TreeNode *valueNode = root->children[1];
        TreeNode *statementsNode = root->children[3];
        TreeNode *N9Node = root->children[6];
        generate_AST(valueNode);
        generate_AST(statementsNode);
        generate_AST(N9Node);
        root->node = make_ASTNode(valueNode->node, caseStmts);
        if (statementsNode->node)
            add_ASTChild(root->node, statementsNode->node);
        if (N9Node->node)
            add_ASTChild(root->node, N9Node->node);
    }
    else if AST_COND(root, N9, TK_CASE)
    {
        // <N9> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
        TreeNode *valueNode = root->children[1];
        TreeNode *statementsNode = root->children[3];
        TreeNode *N9Node = root->children[6];
        generate_AST(valueNode);
        generate_AST(statementsNode);
        generate_AST(N9Node);
        if (statementsNode->node) {
            root->node = make_ASTNode(valueNode->node, N9);
            add_ASTChild(root->node, statementsNode->node);
            if (N9Node->node) {
                add_ASTChild(root->node, N9Node->node);
            }
        }
        else if (N9Node->node) {
            root->node = make_ASTNode(valueNode->node, N9);
            add_ASTChild(root->node, N9Node->node);
        }
        else if (statementsNode->node == NULL && N9Node->node == NULL) {
            root->node = valueNode->node;
        }
    }
    else if AST_COND(root, N9, TK_EPSILON)
    {
        // <N9> -> E
        root->node = NULL;
    }
    else if AST_COND(root, value, TK_NUM)
    {
        // <value> -> TK_NUM
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, value, TK_TRUE)
    {
        // <value> -> TK_TRUE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, value, TK_FALSE)
    {
        // <value> -> TK_FALSE
        root->node = make_ASTLeaf(NULL, root->children[0]->token);
    }
    else if AST_COND(root, g_default, TK_DEFAULT)
    {
        // <g_default> -> TK_DEFAULT TK_COLON <statements> TK_BREAK TK_SEMICOL
        TreeNode *statementsNode = root->children[2];
        generate_AST(statementsNode);
        root->node = statementsNode->node;
    }
    else if AST_COND(root, g_default, TK_EPSILON)
    {
        // <g_default> -> E
        root->node = NULL;
    }
    else if AST_COND(root, iterativeStmt, TK_FOR)
    {
        // <iterativeStmt> -> TK_FOR TK_BO TK_ID TK_IN <range> TK_BC TK_START <statements> TK_END
        TreeNode *rangeNode = root->children[4];
        TreeNode *statementsNode = root->children[7];
        generate_AST(rangeNode);
        generate_AST(statementsNode);
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), iterativeStmt);
        if (rangeNode->node)
            add_ASTChild(root->node, rangeNode->node);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[6]->token));
        if (statementsNode->node)
            add_ASTChild(root->node, statementsNode->node);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[8]->token));
    }
    else if AST_COND(root, iterativeStmt, TK_WHILE)
    {
        // <iterativeStmt> -> TK_WHILE TK_BO <arithmeticOrBooleanExpr> TK_BC TK_START <statements> TK_END
        TreeNode *arithmeticOrBooleanExprNode = root->children[2];
        TreeNode *statementsNode = root->children[5];
        generate_AST(arithmeticOrBooleanExprNode);
        generate_AST(statementsNode);
        root->node = make_ASTNode(arithmeticOrBooleanExprNode->node, iterativeStmt);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[4]->token));
        if (statementsNode->node)
            add_ASTChild(root->node, statementsNode->node);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[6]->token));
    }
    else if AST_COND(root, range, TK_NUM)
    {
        // <range> -> TK_NUM TK_RANGEOP TK_NUM
        root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), range);
        add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[2]->token));
    }
}

void synthesize_attributes(ASTNode* root) {
    // Performs attribute synthesis via a post order traversal
    ASTNode* temp = root;
    if (!temp)
        return;
    if (temp->children == NULL) {
        // Leaf Node. Pointless
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        synthesize_attributes(root->children[i]);
    }
    if (root->token_type == range) {
        // Synthesize attributes for <range>
        if (atoi(root->children[0]->token.lexeme) > atoi(root->children[1]->token.lexeme)) {
            fprintf(stderr, "Semantic Error (Line No: %d) : Range Operator must be of the form (lower..higher)\n", root->children[0]->token.line_no);
        }
    }
}

void print_AST(ASTNode* root) {
    // Prints the AST in an inorder traversal
    if (!root)
        return;
    // printf("Number of Children: %d\n", root->num_children);
    printf("%s => ", get_string_from_term(root->token_type));
    if (root->syn_attribute.token_type != TK_EPSILON)
        printf("Synthesized Attribute: %s\n", get_string_from_term(root->syn_attribute.token_type));
    if (root->children == NULL || root->num_children == 0) {
        printf("Lexeme: %s, Line No: %d\n", root->token.lexeme, root->token.line_no);
    }
    else {
        for (int i=0; i<root->num_children; i++) {
            //printf("Children of %s\n", get_string_from_term(root->token_type));
            print_AST(root->children[i]);
        }       
        //printf("Children End of %s\n", get_string_from_term(root->token_type));
    }
}

void print_AST_without_parent(ASTNode* parent) {
    if (!parent || !(parent->children))
        return;
    printf("Number of Children: %d\n", parent->num_children);
    for (int i=0; i<parent->num_children; i++)
        print_AST(parent->children[i]);
}

void free_AST(ASTNode* root) {
    if (!root)
        return;
    if (!root->children) {
        free(root);
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        free_AST(root->children[i]);
    }
    free(root->children);
    free(root);
    return;
}
