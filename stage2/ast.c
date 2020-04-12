#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// Reduce Boilerplate code
#define AST_COND(root, gterm, num) (root->token.token_type == gterm && root->num_lhs == num)

ASTNode* make_ASTNode(ASTNode* child, term token_type)
{
    // Allocates memory for an AST Node, with a child
    ASTNode* node = (ASTNode*) calloc (1, sizeof(ASTNode));
    node->token_type = token_type;
    node->parent = NULL;
    node->children = (ASTNode**) calloc (1, sizeof(ASTNode*));
    node->children[0] = child;
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
	if AST_COND(root, program, 0)
	{
		// <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
		TreeNode *moduleDeclarationsNode = root->children[0];
		TreeNode *otherModulesNode = root->children[1];
		TreeNode *driverModuleNode = root->children[2];
		TreeNode *otherModules2Node = root->children[3];
		generate_AST(moduleDeclarationsNode);
		generate_AST(otherModulesNode);
		generate_AST(driverModuleNode);
		generate_AST(otherModules2Node);
		root->node = make_ASTNode(moduleDeclarationsNode->node, program);
		if (otherModulesNode->node)
			add_ASTChild(root->node, otherModulesNode->node);
		if (driverModuleNode->node)
			add_ASTChild(root->node, driverModuleNode->node);
		if (otherModules2Node->node)
			add_ASTChild(root->node, otherModules2Node->node);
	}
	else if AST_COND(root, moduleDeclarations, 0)
	{
		// <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>
		TreeNode *moduleDeclarationNode = root->children[0];
		TreeNode *moduleDeclarationsNode = root->children[1];
		generate_AST(moduleDeclarationNode);
		generate_AST(moduleDeclarationsNode);
		root->node = make_ASTNode(moduleDeclarationNode->node, moduleDeclarations);
		if (moduleDeclarationsNode->node)
			add_ASTChild(root->node, moduleDeclarationsNode->node);
	}
	else if AST_COND(root, moduleDeclarations, 1)
	{
		// <moduleDeclarations> -> E
		root->node = NULL;
	}
	else if AST_COND(root, moduleDeclaration, 0)
	{
		// <moduleDeclaration> -> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), moduleDeclaration);
	}
	else if AST_COND(root, otherModules, 0)
	{
		// <otherModules> -> <module> <otherModules>
		TreeNode *moduleNode = root->children[0];
		TreeNode *otherModulesNode = root->children[1];
		generate_AST(moduleNode);
		generate_AST(otherModulesNode);
		root->node = make_ASTNode(moduleNode->node, otherModules);
		if (otherModulesNode->node)
			add_ASTChild(root->node, otherModulesNode->node);
	}
	else if AST_COND(root, otherModules, 1)
	{
		// <otherModules> -> E
		root->node = NULL;
	}
	else if AST_COND(root, driverModule, 0)
	{
		// <driverModule> -> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>
		TreeNode *moduleDefNode = root->children[4];
		generate_AST(moduleDefNode);
		root->node = make_ASTNode(moduleDefNode->node, driverModule);
	}
	else if AST_COND(root, module, 0)
	{
		// <module> -> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>
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
	else if AST_COND(root, ret, 0)
	{
		// <ret> -> TK_RETURNS TK_SQBO <output_plist> TK_SQBC TK_SEMICOL
		TreeNode *output_plistNode = root->children[2];
		generate_AST(output_plistNode);
		root->node = make_ASTNode(output_plistNode->node, ret);
	}
	else if AST_COND(root, ret, 1)
	{
		// <ret> -> E
		root->node = NULL;
	}
	else if AST_COND(root, input_plist, 0)
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
	else if AST_COND(root, N1, 0)
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
	else if AST_COND(root, N1, 1)
	{
		// <N1> -> E
		root->node = NULL;
	}
	else if AST_COND(root, output_plist, 0)
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
	else if AST_COND(root, N2, 0)
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
	else if AST_COND(root, N2, 1)
	{
		// <N2> -> E
		root->node = NULL;
	}
	else if AST_COND(root, dataType, 0)
	{
		// <dataType> -> TK_INTEGER
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
	}
	else if AST_COND(root, dataType, 1)
	{
		// <dataType> -> TK_REAL
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
	}
	else if AST_COND(root, dataType, 2)
	{
		// <dataType> -> TK_BOOLEAN
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), dataType);
	}
	else if AST_COND(root, dataType, 3)
	{
		// <dataType> -> TK_ARRAY TK_SQBO <range_arrays> TK_SQBC TK_OF <type>
		TreeNode *range_arraysNode = root->children[2];
		TreeNode *typeNode = root->children[5];
		generate_AST(range_arraysNode);
		generate_AST(typeNode);
		root->node = make_ASTNode(range_arraysNode->node, dataType);
		if (typeNode->node)
			add_ASTChild(root->node, typeNode->node);
	}
	else if AST_COND(root, range_arrays, 0)
	{
		// <range_arrays> -> <g_index> TK_RANGEOP <g_index>
		TreeNode *g_indexNode = root->children[0];
		TreeNode *g_index2Node = root->children[2];
		generate_AST(g_indexNode);
		generate_AST(g_indexNode);
		root->node = make_ASTNode(g_indexNode->node, range_arrays);
		if (g_indexNode->node)
			add_ASTChild(root->node, g_index2Node->node);
	}
	else if AST_COND(root, type, 0)
	{
		// <type> -> TK_INTEGER
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), type);
	}
	else if AST_COND(root, type, 1)
	{
		// <type> -> TK_REAL
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), type);
	}
	else if AST_COND(root, type, 2)
	{
		// <type> -> TK_BOOLEAN
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), type);
	}
	else if AST_COND(root, moduleDef, 0)
	{
		// <moduleDef> -> TK_START <statements> TK_END
		TreeNode *statementsNode = root->children[1];
		generate_AST(statementsNode);
		root->node = make_ASTNode(statementsNode->node, moduleDef);
	}
	else if AST_COND(root, statements, 0)
	{
		// <statements> -> <statement> <statements>
		TreeNode *statementNode = root->children[0];
		TreeNode *statementsNode = root->children[1];
		generate_AST(statementNode);
		generate_AST(statementsNode);
		root->node = make_ASTNode(statementNode->node, statements);
		if (statementsNode->node)
			add_ASTChild(root->node, statementsNode->node);
	}
	else if AST_COND(root, statements, 1)
	{
		// <statements> -> E
		root->node = NULL;
	}
	else if AST_COND(root, statement, 0)
	{
		// <statement> -> <ioStmt>
		TreeNode *ioStmtNode = root->children[0];
		generate_AST(ioStmtNode);
		root->node = make_ASTNode(ioStmtNode->node, statement);
	}
	else if AST_COND(root, statement, 1)
	{
		// <statement> -> <simpleStmt>
		TreeNode *simpleStmtNode = root->children[0];
		generate_AST(simpleStmtNode);
		root->node = make_ASTNode(simpleStmtNode->node, statement);
	}
	else if AST_COND(root, statement, 2)
	{
		// <statement> -> <declareStmt>
		TreeNode *declareStmtNode = root->children[0];
		generate_AST(declareStmtNode);
		root->node = make_ASTNode(declareStmtNode->node, statement);
	}
	else if AST_COND(root, statement, 3)
	{
		// <statement> -> <condionalStmt>
		TreeNode *condionalStmtNode = root->children[0];
		generate_AST(condionalStmtNode);
		root->node = make_ASTNode(condionalStmtNode->node, statement);
	}
	else if AST_COND(root, statement, 4)
	{
		// <statement> -> <iterativeStmt>
		TreeNode *iterativeStmtNode = root->children[0];
		generate_AST(iterativeStmtNode);
		root->node = make_ASTNode(iterativeStmtNode->node, statement);
	}
	else if AST_COND(root, ioStmt, 0)
	{
		// <ioStmt> -> TK_GET_VALUE TK_BO TK_ID TK_BC TK_SEMICOL
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), ioStmt);
	}
	else if AST_COND(root, ioStmt, 1)
	{
		// <ioStmt> -> TK_PRINT TK_BO <var> TK_BC TK_SEMICOL
		TreeNode *varNode = root->children[2];
		generate_AST(varNode);
		root->node = make_ASTNode(varNode->node, ioStmt);
	}
	else if AST_COND(root, var, 0)
	{
		// <var> -> <var_id_num>
		TreeNode *var_id_numNode = root->children[0];
		generate_AST(var_id_numNode);
		root->node = make_ASTNode(var_id_numNode->node, var);
	}
	else if AST_COND(root, var, 1)
	{
		// <var> -> <boolConstt>
		TreeNode *boolConsttNode = root->children[0];
		generate_AST(boolConsttNode);
		root->node = make_ASTNode(boolConsttNode->node, var);
	}
	else if AST_COND(root, boolConstt, 0)
	{
		// <boolConstt> -> TK_TRUE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), boolConstt);
	}
	else if AST_COND(root, boolConstt, 1)
	{
		// <boolConstt> -> TK_FALSE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), boolConstt);
	}
	else if AST_COND(root, var_id_num, 0)
	{
		// <var_id_num> -> TK_ID <whichId>
		TreeNode *whichIdNode = root->children[1];
		generate_AST(whichIdNode);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), var_id_num);
		if (whichIdNode->node)
			add_ASTChild(root->node, whichIdNode->node);
	}
	else if AST_COND(root, var_id_num, 1)
	{
		// <var_id_num> -> TK_NUM
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), var_id_num);
	}
	else if AST_COND(root, var_id_num, 2)
	{
		// <var_id_num> -> TK_RNUM
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), var_id_num);
	}
	else if AST_COND(root, whichId, 0)
	{
		// <whichId> -> TK_SQBO <g_index> TK_SQBC
		TreeNode *g_indexNode = root->children[1];
		generate_AST(g_indexNode);
		root->node = make_ASTNode(g_indexNode->node, whichId);
	}
	else if AST_COND(root, whichId, 1)
	{
		// <whichId> -> E
		root->node = NULL;
	}
	else if AST_COND(root, simpleStmt, 0)
	{
		// <simpleStmt> -> <assignmentStmt>
		TreeNode *assignmentStmtNode = root->children[0];
		generate_AST(assignmentStmtNode);
		root->node = make_ASTNode(assignmentStmtNode->node, simpleStmt);
	}
	else if AST_COND(root, simpleStmt, 1)
	{
		// <simpleStmt> -> <moduleReuseStmt>
		TreeNode *moduleReuseStmtNode = root->children[0];
		generate_AST(moduleReuseStmtNode);
		root->node = make_ASTNode(moduleReuseStmtNode->node, simpleStmt);
	}
	else if AST_COND(root, assignmentStmt, 0)
	{
		// <assignmentStmt> -> TK_ID <whichStmt>
		TreeNode *whichStmtNode = root->children[1];
		generate_AST(whichStmtNode);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), assignmentStmt);
		if (whichStmtNode->node)
			add_ASTChild(root->node, whichStmtNode->node);
	}
	else if AST_COND(root, whichStmt, 0)
	{
		// <whichStmt> -> <lvalueIDStmt>
		TreeNode *lvalueIDStmtNode = root->children[0];
		generate_AST(lvalueIDStmtNode);
		root->node = make_ASTNode(lvalueIDStmtNode->node, whichStmt);
	}
	else if AST_COND(root, whichStmt, 1)
	{
		// <whichStmt> -> <lvalueARRStmt>
		TreeNode *lvalueARRStmtNode = root->children[0];
		generate_AST(lvalueARRStmtNode);
		root->node = make_ASTNode(lvalueARRStmtNode->node, whichStmt);
	}
	else if AST_COND(root, lvalueIDStmt, 0)
	{
		// <lvalueIDStmt> -> TK_ASSIGNOP <expression> TK_SEMICOL
		TreeNode *expressionNode = root->children[1];
		generate_AST(expressionNode);
		root->node = make_ASTNode(expressionNode->node, lvalueIDStmt);
	}
	else if AST_COND(root, lvalueARRStmt, 0)
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
	else if AST_COND(root, g_index, 0)
	{
		// <g_index> -> TK_NUM
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), g_index);
	}
	else if AST_COND(root, g_index, 1)
	{
		// <g_index> -> TK_ID
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), g_index);
	}
	else if AST_COND(root, moduleReuseStmt, 0)
	{
		// <moduleReuseStmt> -> <optional> TK_USE TK_MODULE TK_ID TK_WITH TK_PARAMETERS <idList> TK_SEMICOL
		TreeNode *optionalNode = root->children[0];
		TreeNode *idListNode = root->children[6];
		generate_AST(optionalNode);
		generate_AST(idListNode);
		root->node = make_ASTNode(optionalNode->node, moduleReuseStmt);
		add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[3]->token));
		if (idListNode->node)
			add_ASTChild(root->node, idListNode->node);
	}
	else if AST_COND(root, optional, 0)
	{
		// <optional> -> TK_SQBO <idList> TK_SQBC TK_ASSIGNOP
		TreeNode *idListNode = root->children[1];
		generate_AST(idListNode);
		root->node = make_ASTNode(idListNode->node, optional);
	}
	else if AST_COND(root, optional, 1)
	{
		// <optional> -> E
		root->node = NULL;
	}
	else if AST_COND(root, idList, 0)
	{
		// <idList> -> TK_ID <N3>
		TreeNode *N3Node = root->children[1];
		generate_AST(N3Node);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), idList);
		if (N3Node->node)
			add_ASTChild(root->node, N3Node->node);
	}
	else if AST_COND(root, N3, 0)
	{
		// <N3> -> TK_COMMA TK_ID <N3>
		TreeNode *N3Node = root->children[2];
		generate_AST(N3Node);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[1]->token), N3);
		if (N3Node->node)
			add_ASTChild(root->node, N3Node->node);
	}
	else if AST_COND(root, N3, 1)
	{
		// <N3> -> E
		root->node = NULL;
	}
	else if AST_COND(root, expression, 0)
	{
		// <expression> -> <arithmeticOrBooleanExpr>
		TreeNode *arithmeticOrBooleanExprNode = root->children[0];
		generate_AST(arithmeticOrBooleanExprNode);
		root->node = make_ASTNode(arithmeticOrBooleanExprNode->node, expression);
	}
	else if AST_COND(root, expression, 1)
	{
		// <expression> -> <U>
		TreeNode *UNode = root->children[0];
		generate_AST(UNode);
		root->node = make_ASTNode(UNode->node, expression);
	}
	else if AST_COND(root, U, 0)
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
	else if AST_COND(root, new_NT, 0)
	{
		// <new_NT> -> TK_BO <arithmeticExpr> TK_BC
		TreeNode *arithmeticExprNode = root->children[1];
		generate_AST(arithmeticExprNode);
		root->node = make_ASTNode(arithmeticExprNode->node, new_NT);
	}
	else if AST_COND(root, new_NT, 1)
	{
		// <new_NT> -> <var_id_num>
		TreeNode *var_id_numNode = root->children[0];
		generate_AST(var_id_numNode);
		root->node = make_ASTNode(var_id_numNode->node, new_NT);
	}
	else if AST_COND(root, unary_op, 0)
	{
		// <unary_op> -> TK_PLUS
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), unary_op);
	}
	else if AST_COND(root, unary_op, 1)
	{
		// <unary_op> -> TK_MINUS
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), unary_op);
	}
	else if AST_COND(root, arithmeticOrBooleanExpr, 0)
	{
		// <arithmeticOrBooleanExpr> -> <AnyTerm> <N7>
		TreeNode *AnyTermNode = root->children[0];
		TreeNode *N7Node = root->children[1];
		generate_AST(AnyTermNode);
		generate_AST(N7Node);
		root->node = make_ASTNode(AnyTermNode->node, arithmeticOrBooleanExpr);
		if (N7Node->node)
			add_ASTChild(root->node, N7Node->node);
	}
	else if AST_COND(root, N7, 0)
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
	else if AST_COND(root, N7, 1)
	{
		// <N7> -> E
		root->node = NULL;
	}
	else if AST_COND(root, AnyTerm, 0)
	{
		// <AnyTerm> -> <arithmeticExpr> <N8>
		TreeNode *arithmeticExprNode = root->children[0];
		TreeNode *N8Node = root->children[1];
		generate_AST(arithmeticExprNode);
		generate_AST(N8Node);
		root->node = make_ASTNode(arithmeticExprNode->node, AnyTerm);
		if (N8Node->node)
			add_ASTChild(root->node, N8Node->node);
	}
	else if AST_COND(root, AnyTerm, 1)
	{
		// <AnyTerm> -> <boolConstt>
		TreeNode *boolConsttNode = root->children[0];
		generate_AST(boolConsttNode);
		root->node = make_ASTNode(boolConsttNode->node, AnyTerm);
	}
	else if AST_COND(root, N8, 0)
	{
		// <N8> -> <relationalOp> <arithmeticExpr>
		TreeNode *relationalOpNode = root->children[0];
		TreeNode *arithmeticExprNode = root->children[1];
		generate_AST(relationalOpNode);
		generate_AST(arithmeticExprNode);
		root->node = make_ASTNode(relationalOpNode->node, N8);
		if (arithmeticExprNode->node)
			add_ASTChild(root->node, arithmeticExprNode->node);
	}
	else if AST_COND(root, N8, 1)
	{
		// <N8> -> E
		root->node = NULL;
	}
	else if AST_COND(root, arithmeticExpr, 0)
	{
		// <arithmeticExpr> -> <g_term> <N4>
		TreeNode *g_termNode = root->children[0];
		TreeNode *N4Node = root->children[1];
		generate_AST(g_termNode);
		generate_AST(N4Node);
		root->node = make_ASTNode(g_termNode->node, arithmeticExpr);
		if (N4Node->node)
			add_ASTChild(root->node, N4Node->node);
	}
	else if AST_COND(root, N4, 0)
	{
		// <N4> -> <op1> <g_term> <N4>
		TreeNode *op1Node = root->children[0];
		TreeNode *g_termNode = root->children[1];
		TreeNode *N4Node = root->children[2];
		generate_AST(op1Node);
		generate_AST(g_termNode);
		generate_AST(N4Node);
		root->node = make_ASTNode(op1Node->node, N4);
		if (g_termNode->node)
			add_ASTChild(root->node, g_termNode->node);
		if (N4Node->node)
			add_ASTChild(root->node, N4Node->node);
	}
	else if AST_COND(root, N4, 1)
	{
		// <N4> -> E
		root->node = NULL;
	}
	else if AST_COND(root, g_term, 0)
	{
		// <g_term> -> <factor> <N5>
		TreeNode *factorNode = root->children[0];
		TreeNode *N5Node = root->children[1];
		generate_AST(factorNode);
		generate_AST(N5Node);
		root->node = make_ASTNode(factorNode->node, g_term);
		if (N5Node->node)
			add_ASTChild(root->node, N5Node->node);
	}
	else if AST_COND(root, N5, 0)
	{
		// <N5> -> <op2> <factor> <N5>
		TreeNode *op2Node = root->children[0];
		TreeNode *factorNode = root->children[1];
		TreeNode *N5Node = root->children[2];
		generate_AST(op2Node);
		generate_AST(factorNode);
		generate_AST(N5Node);
		root->node = make_ASTNode(op2Node->node, N5);
		if (factorNode->node)
			add_ASTChild(root->node, factorNode->node);
		if (N5Node->node)
			add_ASTChild(root->node, N5Node->node);
	}
	else if AST_COND(root, N5, 1)
	{
		// <N5> -> E
		root->node = NULL;
	}
	else if AST_COND(root, factor, 0)
	{
		// <factor> -> TK_BO <arithmeticOrBooleanExpr> TK_BC
		TreeNode *arithmeticOrBooleanExprNode = root->children[1];
		generate_AST(arithmeticOrBooleanExprNode);
		root->node = make_ASTNode(arithmeticOrBooleanExprNode->node, factor);
	}
	else if AST_COND(root, factor, 1)
	{
		// <factor> -> <var_id_num>
		TreeNode *var_id_numNode = root->children[0];
		generate_AST(var_id_numNode);
		root->node = make_ASTNode(var_id_numNode->node, factor);
	}
	else if AST_COND(root, op1, 0)
	{
		// <op1> -> TK_PLUS
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), op1);
	}
	else if AST_COND(root, op1, 1)
	{
		// <op1> -> TK_MINUS
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), op1);
	}
	else if AST_COND(root, op2, 0)
	{
		// <op2> -> TK_MUL
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), op2);
	}
	else if AST_COND(root, op2, 1)
	{
		// <op2> -> TK_DIV
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), op2);
	}
	else if AST_COND(root, logicalOp, 0)
	{
		// <logicalOp> -> TK_AND
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), logicalOp);
	}
	else if AST_COND(root, logicalOp, 1)
	{
		// <logicalOp> -> TK_OR
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), logicalOp);
	}
	else if AST_COND(root, relationalOp, 0)
	{
		// <relationalOp> -> TK_LT
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, relationalOp, 1)
	{
		// <relationalOp> -> TK_LE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, relationalOp, 2)
	{
		// <relationalOp> -> TK_GT
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, relationalOp, 3)
	{
		// <relationalOp> -> TK_GE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, relationalOp, 4)
	{
		// <relationalOp> -> TK_EQ
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, relationalOp, 5)
	{
		// <relationalOp> -> TK_NE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), relationalOp);
	}
	else if AST_COND(root, declareStmt, 0)
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
	else if AST_COND(root, condionalStmt, 0)
	{
		// <condionalStmt> -> TK_SWITCH TK_BO TK_ID TK_BC TK_START <caseStmts> <g_default> TK_END
		TreeNode *caseStmtsNode = root->children[5];
		TreeNode *g_defaultNode = root->children[6];
		generate_AST(caseStmtsNode);
		generate_AST(g_defaultNode);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), condionalStmt);
		if (caseStmtsNode->node)
			add_ASTChild(root->node, caseStmtsNode->node);
		if (g_defaultNode->node)
			add_ASTChild(root->node, g_defaultNode->node);
	}
	else if AST_COND(root, caseStmts, 0)
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
	else if AST_COND(root, N9, 0)
	{
		// <N9> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
		TreeNode *valueNode = root->children[1];
		TreeNode *statementsNode = root->children[3];
		TreeNode *N9Node = root->children[6];
		generate_AST(valueNode);
		generate_AST(statementsNode);
		generate_AST(N9Node);
		root->node = make_ASTNode(valueNode->node, N9);
		if (statementsNode->node)
			add_ASTChild(root->node, statementsNode->node);
		if (N9Node->node)
			add_ASTChild(root->node, N9Node->node);
	}
	else if AST_COND(root, N9, 1)
	{
		// <N9> -> E
		root->node = NULL;
	}
	else if AST_COND(root, value, 0)
	{
		// <value> -> TK_NUM
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), value);
	}
	else if AST_COND(root, value, 1)
	{
		// <value> -> TK_TRUE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), value);
	}
	else if AST_COND(root, value, 2)
	{
		// <value> -> TK_FALSE
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), value);
	}
	else if AST_COND(root, g_default, 0)
	{
		// <g_default> -> TK_DEFAULT TK_COLON <statements> TK_BREAK TK_SEMICOL
		TreeNode *statementsNode = root->children[2];
		generate_AST(statementsNode);
		root->node = make_ASTNode(statementsNode->node, g_default);
	}
	else if AST_COND(root, g_default, 1)
	{
		// <g_default> -> E
		root->node = NULL;
	}
	else if AST_COND(root, iterativeStmt, 0)
	{
		// <iterativeStmt> -> TK_FOR TK_BO TK_ID TK_IN <range> TK_BC TK_START <statements> TK_END
		TreeNode *rangeNode = root->children[4];
		TreeNode *statementsNode = root->children[7];
		generate_AST(rangeNode);
		generate_AST(statementsNode);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), iterativeStmt);
		if (rangeNode->node)
			add_ASTChild(root->node, rangeNode->node);
		if (statementsNode->node)
			add_ASTChild(root->node, statementsNode->node);
	}
	else if AST_COND(root, iterativeStmt, 1)
	{
		// <iterativeStmt> -> TK_WHILE TK_BO <arithmeticOrBooleanExpr> TK_BC TK_START <statements> TK_END
		TreeNode *arithmeticOrBooleanExprNode = root->children[2];
		TreeNode *statementsNode = root->children[5];
		generate_AST(arithmeticOrBooleanExprNode);
		generate_AST(statementsNode);
		root->node = make_ASTNode(arithmeticOrBooleanExprNode->node, iterativeStmt);
		if (statementsNode->node)
			add_ASTChild(root->node, statementsNode->node);
	}
	else if AST_COND(root, range, 0)
	{
		// <range> -> TK_NUM TK_RANGEOP TK_NUM
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[0]->token), range);
		add_ASTChild(root->node, make_ASTLeaf(NULL, root->children[2]->token));
	}
}

/**
void generate_AST(TreeNode* root) {
    // Generates the AST from the Parse Tree
    if AST_COND(root, program, 0) {
        // <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
		TreeNode *moduleDeclarationsNode = root->children[0];
	    TreeNode *otherModulesNode1 = root->children[1];
		TreeNode *driverModulesNode = root->children[2];
	    TreeNode *otherModulesNode2 = root->children[3];

        generate_AST(moduleDeclarationsNode);
	    generate_AST(otherModulesNode1);
	    generate_AST(driverModulesNode);
	    generate_AST(otherModulesNode2);
        
        // Case 1: If <moduledeclarations> -> E
        if (moduleDeclarationsNode->node == NULL) {
            if (otherModulesNode1->node == NULL) {
                // Case 1a: If <othermodules1> -> E
                // Place the sub-tree under <driverModule>
                root->node = make_ASTNode(driverModulesNode->node, program);

		        if ((otherModulesNode2->node)) add_ASTChild(root->node, otherModulesNode2->node);
            }
            else {            
                // Case 1b: <othermodules> exists
                // Place the sub-tree under <otherModules>
		        root->node = make_ASTNode(otherModulesNode1->node, program);
		        add_ASTChild(root->node, driverModulesNode->node);
			    if ((otherModulesNode2->node))
				    add_ASTChild(root->node, otherModulesNode2->node);
            }
        }
        else {
            // Case 2: Simply place it under <moduledeclarations>
		    root->node = make_ASTNode(moduleDeclarationsNode->node, program);
            if (otherModulesNode1->node) { add_ASTChild(root->node, otherModulesNode1->node); }
	        add_ASTChild(root->node, driverModulesNode->node);
	        if (otherModulesNode2->node)
		        add_ASTChild(root->node, otherModulesNode2->node);
        }
    }
    else if AST_COND(root, moduleDeclarations, 0) {
        // <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>
		TreeNode *moduleDeclarationNode = root->children[0];
	    TreeNode *moduleDeclarationsNode = root->children[1];
        
        generate_AST(moduleDeclarationNode);
        generate_AST(moduleDeclarationsNode);

        root->node = make_ASTNode(moduleDeclarationNode->node, moduleDeclarations);
	    if (moduleDeclarationsNode->node)
	        add_ASTChild(root->node, moduleDeclarationsNode->node);
	    
    }
    else if AST_COND(root, moduleDeclarations, 1) {
        // <moduleDeclarations> -> E
        // This node is NULL 
        return;
    }
    else if AST_COND(root, moduleDeclaration, 0) {
        // <moduleDeclaration> -> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL
        // This is a terminal. Create a Leaf node

		root->node = make_ASTLeaf(NULL, root->children[2]->token);
        return;
    }
    else if AST_COND(root, otherModules, 0) {
        // <otherModules> -> <module> <otherModules>
		TreeNode *moduleNode = root->children[0];
	    TreeNode *otherModulesNode = root->children[1];
        
        generate_AST(moduleNode);
	    generate_AST(otherModulesNode);

        add_ASTChild(root->node, moduleNode);
	    add_ASTChild(root->node, otherModulesNode);
        return; 
    }
    else if AST_COND(root, otherModules, 1) {
        // <otherModules> -> E
        return;
    
    }
    else if AST_COND(root, driverModule, 0) {
        // <driverModule> -> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>
		TreeNode *moduleDefNode = root->children[4];
	    generate_AST(moduleDefNode);
		add_ASTChild(root->node, moduleDefNode->node);
        return;
    }
    else if AST_COND(root, module, 0) {
    
        // <module> -> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>
		TreeNode *inputplistNode = root->children[7];
	    TreeNode *retNode = root->children[10];
		TreeNode *moduleDefNode = root->children[11];
	    generate_AST(inputplistNode);
		generate_AST(retNode);
	    generate_AST(moduleDefNode);
		root->node = make_ASTNode(make_ASTLeaf(NULL, root->children[2]->token), module);
    }
    else if AST_COND(root, ret, 0) {
        // <ret> -> TK_RETURNS TK_SQBO <output_plist> TK_SQBC TK_SEMICOL
		TreeNode *outputplistNode = root->children[2];
	    generate_AST(outputplistNode);
		root->node = make_ASTNode(outputplistNode->node, ret);
    }
    else if AST_COND(root, ret, 1) {
        // <ret> -> E
		return;
    }
    else if AST_COND(root, input_plist, 0) {
        // <input_plist> -> TK_ID TK_COLON <dataType> <N1>
		TreeNode *datatypeNode = root->children[2];
	    TreeNode *n1Node = root->children[3];
		generate_AST(datatypeNode);
	    generate_AST(n1Node);
		root->node = make_ASTNode(datatypeNode->node, input_plist);
	    if (n1Node->node)
		    add_ASTChild(root->node, n1Node->node);
    }
    else if AST_COND(root, N1, 0) {
        // <N1> -> TK_COMMA TK_ID TK_COLON <dataType> <N1>
		TreeNode *datatypeNode = root->children[3];
		TreeNode *n1Node = root->children[4];
		generate_AST(datatypeNode);
		generate_AST(n1Node);
		root->node = make_ASTNode(datatypeNode->node, N1);
		if (n1Node->node)
			add_ASTChild(root->node, n1Node->node);
    }
    else if AST_COND(root, N1, 1) {
        // <N1> -> E
		return;
    }
    else if AST_COND(root, output_plist, 0) {
        // <output_plist> -> TK_ID TK_COLON <type> <N2>
        
    }
    else if AST_COND(root, N2, 0) {
        // <N2> -> TK_COMMA TK_ID TK_COLON <type> <N2>
    
    }
    else if AST_COND(root, N2, 1) {
        // <N2> -> E
		return;
    }
    else if AST_COND(root, dataType, 0) {
        // <dataType> -> TK_INTEGER
    
    }
    else if AST_COND(root, dataType, 1) {
        // <dataType> -> TK_REAL
    
    }
    else if AST_COND(root, dataType, 2) {
        // <dataType> -> TK_BOOLEAN
    
    }
    else if AST_COND(root, dataType, 3) {
        // <dataType> -> TK_ARRAY TK_SQBO <range_arrays> TK_SQBC TK_OF <type>
    
    }
    else if AST_COND(root, range_arrays, 0) {
        // <range_arrays> -> <g_index> TK_RANGEOP <g_index>
    
    }
    else if AST_COND(root, type, 0) {
        // <type> -> TK_INTEGER
    
    }
    else if AST_COND(root, type, 1) {
        // <type> -> TK_REAL
    
    }
    else if AST_COND(root, type, 2) {
        // <type> -> TK_BOOLEAN
    
    }
    else if AST_COND(root, moduleDef, 0) {
        // <moduleDef> -> TK_START <statements> TK_END
    
    }
    else if AST_COND(root, statements, 0) {
        // <statements> -> <statement> <statements>
    
    }
    else if AST_COND(root, statements, 1) {
        // <statements> -> E
    
    }
    else if AST_COND(root, statement, 0) {
        // <statement> -> <ioStmt>
    
    }
    else if AST_COND(root, statement, 1) {
        // <statement> -> <simpleStmt>
    
    }
    else if AST_COND(root, statement, 2) {
        // <statement> -> <declareStmt>
    
    }
    else if AST_COND(root, statement, 3) {
        // <statement> -> <condionalStmt>
    
    }
    else if AST_COND(root, statement, 4) {
        // <statement> -> <iterativeStmt>
    
    }
    else if AST_COND(root, ioStmt, 0) {
        // <ioStmt> -> TK_GET_VALUE TK_BO TK_ID TK_BC TK_SEMICOL
    
    }
    else if AST_COND(root, ioStmt, 1) {
        // <ioStmt> -> TK_PRINT TK_BO <var> TK_BC TK_SEMICOL
    
    }
    else if AST_COND(root, var, 0) {
        // <var> -> <var_id_num>
    
    }
    else if AST_COND(root, var, 1) {
        // <var> -> <boolConstt>
    
    }
    else if AST_COND(root, boolConstt, 0) {
        // <boolConstt> -> TK_TRUE
    
    }
    else if AST_COND(root, boolConstt, 1) {
        // <boolConstt> -> TK_FALSE
    
    }
    else if AST_COND(root, var_id_num, 0) {
        // <var_id_num> -> TK_ID <whichId>
    
    }
    else if AST_COND(root, var_id_num, 1) {
        // <var_id_num> -> TK_NUM
    
    }
    else if AST_COND(root, var_id_num, 2) {
        // <var_id_num> -> TK_RNUM
    
    }
    else if AST_COND(root, whichId, 0) {
        // <whichId> -> TK_SQBO <g_index> TK_SQBC
    
    }
    else if AST_COND(root, whichId, 1) {
        // <whichId> -> E
    
    }
    else if AST_COND(root, simpleStmt, 0) {
        // <simpleStmt> -> <assignmentStmt>
    
    }
    else if AST_COND(root, simpleStmt, 1) {
        // <simpleStmt> -> <moduleReuseStmt>
    
    }
    else if AST_COND(root, assignmentStmt, 0) {
        // <assignmentStmt> -> TK_ID <whichStmt>
    
    }
    else if AST_COND(root, whichStmt, 0) {
        // <whichStmt> -> <lvalueIDStmt>
    
    }
    else if AST_COND(root, whichStmt, 1) {
        // <whichStmt> -> <lvalueARRStmt>
    
    }
    else if AST_COND(root, lvalueIDStmt, 0) {
        // <lvalueIDStmt> -> TK_ASSIGNOP <expression> TK_SEMICOL
    
    }
    else if AST_COND(root, lvalueARRStmt, 0) {
        // <lvalueARRStmt> -> TK_SQBO <g_index> TK_SQBC TK_ASSIGNOP <expression> TK_SEMICOL
    
    }
    else if AST_COND(root, g_index, 0) {
        // <g_index> -> TK_NUM
    
    }
    else if AST_COND(root, g_index, 1) {
        // <g_index> -> TK_ID
    
    }
    else if AST_COND(root, moduleReuseStmt, 0) {
        // <moduleReuseStmt> -> <optional> TK_USE TK_MODULE TK_ID TK_WITH TK_PARAMETERS <idList> TK_SEMICOL
    
    }
    else if AST_COND(root, optional, 0) {
        // <optional> -> TK_SQBO <idList> TK_SQBC TK_ASSIGNOP
    
    }
    else if AST_COND(root, optional, 1) {
        // <optional> -> E
    
    }
    else if AST_COND(root, idList, 0) {
        // <idList> -> TK_ID <N3>
    
    }
    else if AST_COND(root, N3, 0) {
        // <N3> -> TK_COMMA TK_ID <N3>
    
    }
    else if AST_COND(root, N3, 1) {
        // <N3> -> E
    
    }
    else if AST_COND(root, expression, 0) {
        // <expression> -> <arithmeticOrBooleanExpr>
    
    }
    else if AST_COND(root, expression, 1) {
        // <expression> -> <U>
    
    }
    else if AST_COND(root, U, 0) {
        // <U> -> <unary_op> <new_NT>
    
    }
    else if AST_COND(root, new_NT, 0) {
        // <new_NT> -> TK_BO <arithmeticExpr> TK_BC
    
    }
    else if AST_COND(root, new_NT, 1) {
        // <new_NT> -> <var_id_num>
    
    }
    else if AST_COND(root, unary_op, 0) {
        // <unary_op> -> TK_PLUS
    
    }
    else if AST_COND(root, unary_op, 1) {
        // <unary_op> -> TK_MINUS
    
    }
    else if AST_COND(root, arithmeticOrBooleanExpr, 0) {
        // <arithmeticOrBooleanExpr> -> <AnyTerm> <N7>
    
    }
    else if AST_COND(root, N7, 0) {
        // <N7> -> <logicalOp> <AnyTerm> <N7>
    
    }
    else if AST_COND(root, N7, 1) {
        // <N7> -> E
    
    }
    else if AST_COND(root, AnyTerm, 0) {
        // <AnyTerm> -> <arithmeticExpr> <N8>
    
    }
    else if AST_COND(root, AnyTerm, 1) {
        // <AnyTerm> -> <boolConstt>
    
    }
    else if AST_COND(root, N8, 0) {
        // <N8> -> <relationalOp> <arithmeticExpr>
    
    }
    else if AST_COND(root, N8, 1) {
        // <N8> -> E
    
    }
    else if AST_COND(root, arithmeticExpr, 0) {
        // <arithmeticExpr> -> <g_term> <N4>
    
    }
    else if AST_COND(root, N4, 0) {
        // <N4> -> <op1> <g_term> <N4>
    
    }
    else if AST_COND(root, N4, 1) {
        // <N4> -> E
    
    }
    else if AST_COND(root, g_term, 0) {
        // <g_term> -> <factor> <N5>
    
    }
    else if AST_COND(root, N5, 0) {
        // <N5> -> <op2> <factor> <N5>
    
    }
    else if AST_COND(root, N5, 1) {
        // <N5> -> E
    
    }
    else if AST_COND(root, factor, 0) {
        // <factor> -> TK_BO <arithmeticOrBooleanExpr> TK_BC
    
    }
    else if AST_COND(root, factor, 1) {
        // <factor> -> <var_id_num>
    
    }
    else if AST_COND(root, op1, 0) {
        // <op1> -> TK_PLUS
    
    }
    else if AST_COND(root, op1, 1) {
        // <op1> -> TK_MINUS
    
    }
    else if AST_COND(root, op2, 0) {
        // <op2> -> TK_MUL
    
    }
    else if AST_COND(root, op2, 1) {
        // <op2> -> TK_DIV
    
    }
    else if AST_COND(root, logicalOp, 0) {
        // <logicalOp> -> TK_AND
    
    }
    else if AST_COND(root, logicalOp, 1) {
        // <logicalOp> -> TK_OR
    
    }
    else if AST_COND(root, relationalOp, 0) {
        // <relationalOp> -> TK_LT
    
    }
    else if AST_COND(root, relationalOp, 1) {
        // <relationalOp> -> TK_LE
    
    }
    else if AST_COND(root, relationalOp, 2) {
        // <relationalOp> -> TK_GT
    
    }
    else if AST_COND(root, relationalOp, 3) {
        // <relationalOp> -> TK_GE
    
    }
    else if AST_COND(root, relationalOp, 4) {
        // <relationalOp> -> TK_EQ
    
    }
    else if AST_COND(root, relationalOp, 5) {
        // <relationalOp> -> TK_NE
    
    }
    else if AST_COND(root, declareStmt, 0) {
        // <declareStmt> -> TK_DECLARE <idList> TK_COLON <dataType> TK_SEMICOL
    
    }
    else if AST_COND(root, condionalStmt, 0) {
        // <condionalStmt> -> TK_SWITCH TK_BO TK_ID TK_BC TK_START <caseStmts> <g_default> TK_END
    
    }
    else if AST_COND(root, caseStmts, 0) {
        // <caseStmts> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
    
    }
    else if AST_COND(root, N9, 0) {
        // <N9> -> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
    
    }
    else if AST_COND(root, N9, 1) {
        // <N9> -> E
    
    }
    else if AST_COND(root, value, 0) {
        // <value> -> TK_NUM
    
    }
    else if AST_COND(root, value, 1) {
        // <value> -> TK_TRUE
    
    }
    else if AST_COND(root, value, 2) {
        // <value> -> TK_FALSE
    
    }
    else if AST_COND(root, g_default, 0) {
        // <g_default> -> TK_DEFAULT TK_COLON <statements> TK_BREAK TK_SEMICOL
    
    }
    else if AST_COND(root, g_default, 1) {
        // <g_default> -> E
    
    }
    else if AST_COND(root, iterativeStmt, 0) {
        // <iterativeStmt> -> TK_FOR TK_BO TK_ID TK_IN <range> TK_BC TK_START <statements> TK_END
    
    }
    else if AST_COND(root, iterativeStmt, 1) {
        // <iterativeStmt> -> TK_WHILE TK_BO <arithmeticOrBooleanExpr> TK_BC TK_START <statements> TK_END
    
    }
    else if AST_COND(root, range, 0)
    {
        // <range> -> TK_NUM TK_RANGEOP TK_NUM
    
    }
    else
    {
        return;
    }
    return;
}
**/

void print_AST(ASTNode* root) {
    // Prints the AST in an inorder traversal
    if (!root)
        return;
    printf("Node: %s => ", get_string_from_term(root->token_type));
    if (root->children == NULL || root->num_children == 0) {
        printf("Lexeme: %s, Line No: %d\n", root->token.lexeme, root->token.line_no);
    }
    else {
        for (int i=0; i<root->num_children; i++) {
            print_AST(root->children[i]);
        }       
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
