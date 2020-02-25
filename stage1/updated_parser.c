typedef struct TreeNode TreeNode;
struct TreeNode {
    // Structure for the TreeNode used to
    // build the complete parse tree
    struct Token token; // Token from the lexer
    TreeNode* parent; // Pointer to it's parent
    TreeNode** children; // as well as to it's children
    TreeNode* right_sibling; // Pointer to it's right sibling for moving across the tree
    int rule_no; // Store rule number for debugging
    int num_children;
	bool check_term;
};

typedef struct StackNode StackNode;
struct StackNode {
    TreeNode* data;
    StackNode* next;
    bool is_dollar;
};

typedef struct Token Token;

// Define the Token here
struct Token {
    // A Token consists of a token name (terminal), a lexeme,
    // followed by pointers to the previous
    // and next tokens in the line to be tokenized

    term token_type;
    void* lexeme;
    int line_no;
    //Token* prev, *next;
};

TreeNode* make_tree_node(TreeNode* parent, Token token);
TreeNode* add_tree_node(TreeNode* parent, Token token);

StackNode* make_stack_node(TreeNode* data);
StackNode* push(StackNode* stack, TreeNode* data);
StackNode* pop(StackNode* stack);
StackNode* top(StackNode* stack);
bool is_empty(StackNode* stack);

// Updated TK_EOF to TK_DOLLAR
// TK_EOF = 114;

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

TreeNode* generateParseTree (char* filename, ParseTable t, Grammar g, FirstAndFollow f);
{
	Token t = {TK_DOLLAR, NULL, -1};
	TreeNode* dollar = make_tree_node(NULL, t);
	StackNode* stack = make_stack_node(dollar);
	
	t.token_type = program;
	TreeNode* root = make_tree_node(NULL, t);
	stack = push(stack, make_tree_node(NULL, t));

	init_tokenizer(filename);
	
	t = get_next_token();
	
	bool is_complete = false;
	
	while (!is_empty(stack)) {
		if (stack->data->check_term == false) {
			// Non terminal
			TreeNode* curr = stack->data;
			// Look at the parse table
			int rule_no = t.matrix[curr->token.token_type][t.token_type];
			if (rule_no == -1) {
				fprintf(stderr, "Syntax Error: At Token: %s at line number: %d\n", get_string_from_term(t), t.line_no);
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
			
			for (int i=rule.num_right-1; i>=0; i++) {
				stack = push(stack, curr->children[i]);
			}
			continue;
		}
		else {
			TreeNode* curr = stack->data;
			if (curr->token.token_type == t.token_type} {
				if (t.token_type == TK_DOLLAR) {
					// End of stack
					printf("Successfully Parsed!\n");
					is_complete = true;
					break;
				}
				// Pop the stack and then go to the next token
				stack = pop(stack);
				//memcpy(&curr->token.lexeme, &t.lexeme);
				strcpy(curr->token.lexeme, t.lexeme);
				curr->token.line_no = t.line_no;
				t = get_next_token();
			}
			else {
				fprintf(stderr, "Should never happen!\n");
			}
		}
		if (is_complete)
			break;
	}
	free_stack()
	close_tokenizer();
	return curr;
}

/*
lexeme         lineno     tokenName      valueIfNumber    parentNodeSymbol             isLeafNode(yes/no)         NodeSymbol

*/

void pretty_print(TreeNode* node) {
	// Pirnts only one node
	if (node->check_term == true) {
		// Terminal
		Token t = node->token;
		printf("%s\t", node->token.lexeme);
		printf("%d\t", node->token.line_no);
		printf("%s\t", get_string_from_term(node->token.token_type));
		if (t.token_type == TK_NUM)
			printf("%d\t", atoi(t));
		else if (t.token_type == TK_RNUM) {
			for (int i=0; t.lexeme[i] != '\0'; i++)
				if (t.lexeme[i] == 'E')
					t.lexeme[i] = 'e';
			printf("%.4f\t", atof(t));
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
		Token t = node->token;
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
