// Group 42:
// R Vijay Krishna 2017A7PS0183P
// Rohit K 2017A7PS0177P

#include "parser.h"

// Define all Global Variables here
HashTable* hash_table = NULL;
int total_memory_parse_tree = 0;
int num_nodes_parse_tree = 0;

// Complete list of Grammar Symbols here
Symbol symbols[] = {
    {"E", TK_EPSILON},
    {"TK_DECLARE", TK_DECLARE},
    {"TK_MODULE", TK_MODULE},
    {"TK_ID", TK_ID},
    {"TK_SEMICOL", TK_SEMICOL},
    {"TK_DRIVERDEF", TK_DRIVERDEF},
    {"TK_DRIVER", TK_DRIVER},
    {"TK_PROGRAM", TK_PROGRAM},
    {"TK_DRIVERENDDEF", TK_DRIVERENDDEF},
    {"TK_DEF", TK_DEF},
    {"TK_ENDDEF", TK_ENDDEF},
    {"TK_TAKES", TK_TAKES},
    {"TK_INPUT", TK_INPUT},
    {"TK_SQBO", TK_SQBO},
    {"TK_SQBC", TK_SQBC},
    {"TK_RETURNS", TK_RETURNS},
    {"TK_COLON", TK_COLON},
    {"TK_COMMA", TK_COMMA},
    {"TK_INTEGER", TK_INTEGER},
    {"TK_REAL", TK_REAL},
    {"TK_BOOLEAN", TK_BOOLEAN},
    {"TK_ARRAY", TK_ARRAY},
    {"TK_OF", TK_OF},
    {"TK_START", TK_START},
    {"TK_END", TK_END},
    {"TK_GET_VALUE", TK_GET_VALUE},
    {"TK_BO", TK_BO},
    {"TK_BC", TK_BC},
    {"TK_PRINT", TK_PRINT},
    {"TK_NUM", TK_NUM},
    {"TK_RNUM", TK_RNUM},
    {"TK_TRUE", TK_TRUE},
    {"TK_FALSE", TK_FALSE},
    {"TK_ASSIGNOP", TK_ASSIGNOP},
    {"TK_USE", TK_USE},
    {"TK_WITH", TK_WITH},
    {"TK_PARAMETERS", TK_PARAMETERS},
    {"TK_MINUS", TK_MINUS},
    {"TK_PLUS", TK_PLUS},
    {"TK_MUL", TK_MUL},
    {"TK_DIV", TK_DIV},
    {"TK_AND", TK_AND},
    {"TK_OR", TK_OR},
    {"TK_LT", TK_LT},
    {"TK_LE", TK_LE},
    {"TK_GT", TK_GT},
    {"TK_GE", TK_GE},
    {"TK_EQ", TK_EQ},
    {"TK_NE", TK_NE},
    {"TK_SWITCH", TK_SWITCH},
    {"TK_CASE", TK_CASE},
    {"TK_BREAK", TK_BREAK},
    {"TK_DEFAULT", TK_DEFAULT},
    {"TK_FOR", TK_FOR},
    {"TK_IN", TK_IN},
    {"TK_WHILE", TK_WHILE},
    {"TK_RANGEOP", TK_RANGEOP},
    {"<program>", program},
    {"<moduleDeclarations>", moduleDeclarations},
    {"<moduleDeclaration>", moduleDeclaration},
    {"<otherModules>", otherModules},
    {"<driverModule>", driverModule},
    {"<module>", module},
    {"<ret>", ret},
    {"<input_plist>", input_plist},
    {"<N1>", N1},
    {"<output_plist>", output_plist},
    {"<N2>", N2},
    {"<dataType>", dataType},
    {"<range_arrays>", range_arrays},
    {"<type>", type},
    {"<moduleDef>", moduleDef},
    {"<statements>", statements},
    {"<statement>", statement},
    {"<ioStmt>", ioStmt},
    {"<var>", var},
    {"<boolConstt>", boolConstt},
    {"<var_id_num>", var_id_num},
    {"<whichId>", whichId},
    {"<simpleStmt>", simpleStmt},
    {"<assignmentStmt>", assignmentStmt},
    {"<whichStmt>", whichStmt},
    {"<lvalueIDStmt>", lvalueIDStmt},
    {"<lvalueARRStmt>", lvalueARRStmt},
    {"<g_index>", g_index},
    {"<moduleReuseStmt>", moduleReuseStmt},
    {"<optional>", optional},
    {"<idList>", idList},
    {"<N3>", N3},
    {"<expression>", expression},
    {"<U>", U},
    {"<new_NT>", new_NT},
    {"<unary_op>", unary_op},
    {"<arithmeticOrBooleanExpr>", arithmeticOrBooleanExpr},
    {"<N7>", N7},
    {"<AnyTerm>", AnyTerm},
    {"<N8>", N8},
    {"<arithmeticExpr>", arithmeticExpr},
    {"<N4>", N4},
    {"<g_term>", g_term},
    {"<N5>", N5},
    {"<factor>", factor},
    {"<op1>", op1},
    {"<op2>", op2},
    {"<logicalOp>", logicalOp},
    {"<relationalOp>", relationalOp},
    {"<declareStmt>", declareStmt},
    {"<condionalStmt>", condionalStmt},
    {"<caseStmts>", caseStmts},
    {"<N9>", N9},
    {"<value>", value},
    {"<g_default>", g_default},
    {"<iterativeStmt>", iterativeStmt},
    {"<range>", range},
};


unsigned long hash_func (char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

Rule make_rule(int left, int* right, int num_right) {
    // Construct a grammar rule
    Rule grule;
    grule.left = left;
    grule.right = right;
    grule.num_right = num_right;
    return grule;
}

TreeNode* make_tree_node(TreeNode* parent, Token token) {
    // Allocate memory for a TreeNode
    TreeNode* node = (TreeNode*) calloc (1, sizeof(TreeNode));
    node->token = token;
    node->parent = parent;
    node->children = NULL;
    node->right_sibling = NULL;
    node->num_children = 0;
    node->rule_no = -1;
    node->check_term = is_terminal(token.token_type);
    num_nodes_parse_tree++;
    total_memory_parse_tree += sizeof(TreeNode);
    return node;
}

void free_parse_tree(TreeNode* root) {
    // Free memory for the complete Parse Tree
    if (!root)
        return;
    if (root->children) {
        for (int i=0; i<root->num_children; i++) {
            free_parse_tree(root->children[i]);
        }
        free(root->children);
    }
    if (root->token.lexeme)
        free(root->token.lexeme);
    free(root);
}

TreeNode* add_tree_node(TreeNode* parent, Token token) {
    // Adds the token to the tree under parent
    if (!parent) {
        parent = make_tree_node(NULL, token); 
    }
    else {
        if (parent->num_children == 0) {
            parent->children = (TreeNode**) calloc (1, sizeof(TreeNode*));
            parent->children[parent->num_children] = make_tree_node(parent, token);
        }
        else {
            parent->children = (TreeNode**) realloc (parent->children, (parent->num_children + 1) * sizeof(TreeNode*));
            TreeNode* node = make_tree_node(parent, token);
            parent->children[parent->num_children] = node;
            parent->children[parent->num_children - 1]->right_sibling = node;
        }
        parent->num_children++;
    }
    return parent;
}

void printTreeNode(TreeNode* root) {
    // Prints a single tree node
    if (!root)
        return;
    for (int i=0; i<root->num_children; i++) {
        printTreeNode(root->children[i]);
    }
    printf("Node: %s => ", get_string_from_term(root->token.token_type));
}

StackNode* make_stack_node(TreeNode* data) {
    // Make a stack node using the TreeNode pointer
    StackNode* node = (StackNode*) calloc (1, sizeof(StackNode));
    node->data = data;
    node->is_dollar = (data->token.token_type == TK_DOLLAR);
    node->next = NULL;
    return node;
}

StackNode* push(StackNode* stack, TreeNode* data) {
    // Pushes the element into the stack and returns the updated head
    StackNode* node = make_stack_node(data);
    node->next = stack;
    stack = node;
    return stack;
}

StackNode* pop(StackNode* stack) {
    // Pops the element from the stack and returns the updated head
    if (!stack)
        return NULL;
    StackNode* node = stack->next;
    if (!node) {
        free(stack);
        stack = NULL;
    }
    else {
        StackNode* temp = stack;
        stack = node;
        temp->next = NULL;
        free(temp);
    }
    return stack;
}

StackNode* top(StackNode* stack) {
    return stack;
}

bool is_empty(StackNode* stack) {
    return stack == NULL;
}

void free_stack(StackNode* stack) {
    if (!stack)
        return;
    StackNode* temp = stack;
    stack = stack->next;
    temp->next = NULL;
    free_parse_tree(temp->data);
    free(temp);
    free_stack(stack);
}

void free_tree(TreeNode* root) {
    if (!root)
        return;
    if (root->num_children == 0) {
        if (root->parent)
            root->parent->num_children--;
        root->parent = NULL;
        if (root->token.lexeme) {
            free(root->token.lexeme);
        }
        free(root);
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        free_tree(root->children[i]);
    }
}

void free_grammar_rule(Rule grule) {
    // Frees a grammar tule
    free(grule.right);
}

void free_grammar(Grammar g) {
    // Free the grammar structure pointed
    // by g
    for (int i=0; i < g.num_rules; i++) {
        free_grammar_rule(g.rules[i]);
    }
    free(g.rules);

    // Free the HashTable
    if (hash_table)
        free_table(hash_table);
}

void free_first_and_follow(FirstAndFollow f) {
    if (f.first) {
        for (int i=0; i<f.num_tokens + f.num_symbols + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (f.first[i])
                free(f.first[i]);
        }
        free(f.first);
    }
    if (f.follow) {
        for (int i=0; i<f.num_symbols + f.num_tokens + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (f.follow[i])
                free(f.follow[i]);
        }
        free(f.follow);
    }
}

void free_parse_table(ParseTable p) {
    for (int i=0; i<p.num_symbols + p.num_tokens + 1; i++) {
        if (p.matrix[i])
            free(p.matrix[i]);
    }
    free(p.matrix);
}

void print_rule(Rule grule) {
    // Prints a grammar rule
    printf("LHS Number: %d => ", grule.num_lhs);
    printf("LHS : %s , RHS : ", get_string_from_term((term)grule.left));
    for (int i=0; i<grule.num_right; i++) {
        printf("%s , ", get_string_from_term((term)grule.right[i]));
    }
    printf("\n");
}

void print_grammar(Grammar g) {
    // Prints the grammar
    if (g.num_rules <= 0)
        return;
    printf("Grammar retrieved from file.\n-----------------------------\nStatistics:\n");
    printf("Number of Rules: %d\n", g.num_rules);
    printf("Number of Non Terminals: %d\n", g.num_symbols);
    printf("Number of Tokens: %d\n", g.num_tokens);
    for (int i=0; i<g.num_rules; i++) {
        print_rule(g.rules[i]);
    }
    printf("-----------------------------\n");
}

void printFirstAndFollowSets(FirstAndFollow f) {
    // Prints the first and follow sets
    printf("-----------------------------------------------------\n");
    printf("First Sets\n");
    for (int i=0; i<f.num_symbols + f.num_tokens; i++) {
        printf("Symbols: %s => ", get_string_from_term(i));
        for (int j=0; j<f.num_tokens; j++) {
            printf(",%s: %d ", get_string_from_term(j), f.first[i][j]);
        }
        printf("\n\n");
    }
    printf("-----------------------------------------------------\n");
    
    printf("Follow Sets\n");
    for (int i=0; i<f.num_symbols + f.num_tokens; i++) {
        printf("Symbols: %s => ", get_string_from_term(i));
        for (int j=0; j<f.num_tokens; j++) {
            printf(",%s: %d ", get_string_from_term(j), f.follow[i][j]);
        }
        // Include TK_DOLLAR in follow set
        printf(",%s: %d ", get_string_from_term(TK_DOLLAR), f.follow[i][TK_DOLLAR]);
        printf("\n\n");
    }

    printf("-----------------------------------------------------\n");
}

void printParseTable(ParseTable p) {
    // Prints the parse table
    // [SYMBOLS * TERMINALS]
    printf("-----------------------------------------------------\n");
    printf("Parse Table\n");
    for (int i=p.num_tokens; i<p.num_tokens + p.num_symbols; i++) {
        printf("Symbol : %s=>\t", get_string_from_term(i));
        for (int j=0; j<p.num_tokens; j++) {
            printf("<%s, %d> |", get_string_from_term(j), p.matrix[i][j]);
        }
        printf("\n\n");
    }
    printf("-----------------------------------------------------\n");
}

char* get_parser_token(Token t) {
    switch(t.token_type) {
        case TK_NUM:
        return "Integer";
        break;
        case TK_RNUM:
        return "Floating point number";
        break;
        case TK_BOOLEAN:
        return "Boolean";
        break;
        case TK_OF:
        return "Of";
        break;
        case TK_ARRAY:
        return "Array";
        break;
        case TK_START:
        return "start";
        break;
        case TK_END:
        return "end";
        break;
        case TK_DECLARE:
        return "declare";
        break;
        case TK_MODULE:
        return "module";
        break;
        case TK_DRIVER:
        return "driver";
        break;
        case TK_PROGRAM:
        return "program";
        break;
        case TK_GET_VALUE:
        return "get_value";
        break;
        case TK_PRINT:
        return "print";
        break;
        case TK_USE:
        return "use";
        break;
        case TK_TRUE:
        return "true";
        break;
        case TK_FALSE:
        return "false";
        break;
        case TK_TAKES:
        return "takes";
        break;
        case TK_SWITCH:
        return "switch";
        break;
        case TK_CASE:
        return "case";
        break;
        case TK_BREAK:
        return "break";
        break;
        case TK_DEFAULT:
        return "default";
        break;
        case TK_WHILE:
        return "while";
        break;
        case TK_COLON:
        return "colon";
        break;
        case TK_RANGEOP:
        return "range operator";
        break;
        case TK_SEMICOL:
        return ";";
        break;
        case TK_COMMA:
        return ",";
        break;
        case TK_ASSIGNOP:
        return ":=";
        break;
        case TK_SQBO:
        return "[";
        break;
        case TK_SQBC:
        return "]";
        break;
        case TK_BO:
        return "(";
        break;
        case TK_BC:
        return ")";
        break;
        case TK_ID:
        return "Identifier";
        break;
        default:
        break;
    }
    return get_string_from_term(t.token_type);
}

Grammar populate_grammar(FILE* fp) {
    Grammar G;
    G.rules = (Rule*) calloc (1, sizeof(Rule));
    G.rules[0].right = NULL;
    G.num_rules = G.num_symbols = G.num_tokens = 0;
    
    int num;
    char ch = '\0';
    
    int curr_right = 0;
    int curr_rule = -1;
    
    bool is_lhs = false;
    
    char buffer[256];
    char lhs_buffer[256] = {""};
    
    unsigned long (*hash_fun)(char*);
    
    hash_fun = &hash_func;
    
    size_t num_symbols = sizeof(symbols)/sizeof(Symbol);
    hash_table = create_table(CAPACITY*3, hash_fun);
    
    for (uint32_t i=0; i<num_symbols; i++) {
        hash_table = ht_insert(hash_table, symbols[i].key, symbols[i].tid);
    }
    
    int* temp = (int*) calloc (1, sizeof(int));
    int total = 0;
    int lhs_count = 0; // Count of repeat occurences of the same LHS symbol
    
    do {
    while (fscanf(fp, " %s", buffer) == 1) {
        //printf("Buffer = %s\n", buffer);
        if (strcmp(buffer, "$") == 0) {
        is_lhs = true;
        curr_rule++;
        G.num_rules++;
        curr_right = 0;
        continue;
        }
        else if (strcmp(buffer, "->") == 0) {
        is_lhs = false;
        continue;
        }
        else {
        num = ht_search(hash_table, buffer);
        if ((term)num == TK_NONE) {
            fprintf(stderr, "Symbol %s not in hash table\n", buffer);
            exit(EXIT_FAILURE);
        }
        
        bool flag = false;
        for (int i=0; i<total; i++) {
            if (temp[i] == num) {
            flag = true;
            break;
            }
        }
        if (!flag) {
            temp = (int*) realloc (temp, (total + 2) * sizeof(int));
            temp[total] = num;
            total++;
        }
                
        if (is_lhs) {
            // Belongs to the lhs
            //printf("LHS: curr_rule = %d, curr_right = %d\n", curr_rule, curr_right);
            G.rules = (Rule*) realloc (G.rules, (curr_rule + 1) * sizeof(Rule));
            G.rules[curr_rule].right = (int*) calloc ((curr_right + 2),  sizeof(int));
            //G.rules[curr_rule].right[curr_right + 1] = num;
            G.rules[curr_rule].rule_no = curr_rule; // Useless variable
            if (strcmp(lhs_buffer, buffer) == 0) {
                lhs_count ++;
            }
            else {
                lhs_count = 0;
                strcpy(lhs_buffer, buffer);
            }
            G.rules[curr_rule].num_lhs = lhs_count;
            G.rules[curr_rule].left = num;
        }
        else {
            // Go to RHS
            //printf("RHS: curr_left = %d, curr_right = %d\n", curr_left, curr_right);
            G.rules = (Rule*) realloc (G.rules, (curr_rule + 1) * sizeof(Rule));
            G.rules[curr_rule].right = (int*) realloc (G.rules[curr_rule].right, (curr_right + 2) * sizeof(int));
            G.rules[curr_rule].num_right++;
            G.rules[curr_rule].right[curr_right] = num;
            curr_right++;
        }
        // Update the number of terminals / non-terminals
        if (!flag) {
            if (is_terminal(num)) {
            G.num_tokens++;
            }
            else {
            G.num_symbols++;
            }
        }
        }
        is_lhs = false;
    }
    } while (fscanf(fp, " %c", &ch) == 1);


    //G.rules[curr_rule].num_right = curr_right;
    G.num_tokens++;
    free(temp);
    
    return G;
}

int Union(int* a ,int* b, int length) {
    // Performs a union operation (OR) between
    // two sets a and b, indexed using an array
    int change = 0;
    for(int i=1; i<length; i++) {
        if(b[i] == 1) {
            if(a[i] == 1) {
                a[i] = 1;
                continue;
            }
            else {
                a[i] = 1;
                change = 1;
            }
        }
    }
    return change;
}

FirstAndFollow ComputeFirstAndFollowSets(Grammar G){
    // Compute the first and follow sets for the grammar
    FirstAndFollow F;
    
    // Keep an extra index for $ (TK_DOLLAR)
    // although we don't care about that for the first sets
    F.first = (int**) calloc (G.num_symbols + G.num_tokens + 2, sizeof(int*)); 
    for (int i=0; i < G.num_symbols + G.num_tokens + 2; i++) 
         F.first[i] = (int*) calloc (G.num_symbols + G.num_tokens + 2, sizeof(int));
    F.num_symbols = G.num_symbols;
    F.num_tokens = G.num_tokens;
    
    F.follow = (int **) calloc (G.num_symbols + G.num_tokens + 2, sizeof(int *)); 
    for (int i=0; i < G.num_symbols + G.num_tokens + 2; i++) 
         F.follow[i] = (int *) calloc (G.num_symbols + G.num_tokens + 2, sizeof(int));
    
    
    // F.first
    int change;

    // Base Cases for Terminals 
    for(int i=0; i<G.num_symbols; i++) {
        if(is_terminal(i))
            F.first[i][i] = 1;
    }
    
    // A -> Epsilon
    for (int i=0; i<G.num_rules; i++) {
        if ((term) G.rules[i].right[0] == TK_EPSILON) {
            int nt_idx = G.rules[i].left;
            F.first[nt_idx][0] = 1;
        }
    }

    do{
        change=0;

        for(int i=0; i<G.num_rules; i++) {
            for(int j=0; j<G.rules[i].num_right; j++) {
                // If the current right symbol is a terminal
                if( is_terminal(G.rules[i].right[j]) ) {
                    // Add it to the first set and go to the next rule
                    int t_index = G.rules[i].right[j];
                    F.first[G.rules[i].left][t_index] = 1; 
                    // We don't update change, since we reached a terminal.
                    // This belongs to a base case, so this is acceptable
                    //change = 1;   
                    break;
                }

                // Perform an OR operation between 2 first sets, and update the first set
                // with the mask of a right symbol
                int temp = Union(F.first[G.rules[i].left], F.first[G.rules[i].right[j]], G.num_tokens);
                
                if(change != 1)
                    change = temp;
                
                // If we're at the last right symbol
                if(j == (G.rules[i].num_right)-1) {
                    // And if it's set contains Epsilon
                    if(F.first[G.rules[i].right[j]][0] == 1) {
                        // Then Epsilon must also belong to first(left symbol)
                        F.first[G.rules[i].left][0] = 1;
                        change = 1;
                        break;  
                    }
                }
                
                // Otherwise, if the current right symbol has Epsilon in it's
                // first set, don't include it for the left symbol
                else if(F.first[G.rules[i].right[j]][0] != 1)
                    break;
            }
        }

    } while (change == 1);
    
    // Follow sets only for non terminals
    // Set the follow for program to be $
    // Find a way to separately include $ for the
    // follow set, since it's not a normal terminal
    F.follow[(int)program][(int)TK_DOLLAR] = 1;
    
    change = 0;
    do {
        change = 0;
        for(int i=0; i< G.num_rules; i++) {
            for(int j=0; j<G.rules[i].num_right ; j++) {
                if (is_terminal(G.rules[i].right[j])) {
                    // Just continue, since the right is a terminal.
                    // We shouldn't union along with the first / follow
                    // of a terminal
                    continue;
                }
                // If we're at the last right symbol of i
                if(j == G.rules[i].num_right-1) {
                    // Then the follow of the rightmost symbol will be UNIONed with the follow of the leftmost!
                    // We also need to include TK_DOLLAR when updating, so the length of the segment mask is
                    // G.num_tokens + 1, and NOT G.num_tokens
                    int temp = Union(F.follow[G.rules[i].right[j]],  F.follow[G.rules[i].left], G.num_tokens);
                    if(change != 1) {
                        change = temp;
                    }
                    break;
                }   
                
                // Otherwise, the follow of the current right symbol will be ORed with the first of the next right symbol
                int temp = Union(F.follow[G.rules[i].right[j]] , F.first[G.rules[i].right[j+1]], G.num_tokens);
                if(change != 1)
                    change = temp;
                
                // Again, if Epsilon is there in the first of the next right symbol, consider the follow set of that
                // symbol instead, since Epsilon cannot appear on any follow set.
                if(F.first[G.rules[i].right[j+1]][0] == 1) {
                    int temp = Union(F.follow[G.rules[i].right[j]] , F.follow[G.rules[i].right[j+1]], G.num_tokens);
                    if(change != 1)
                        change = temp;
                }
            }
        }       
    } while(change == 1);
    
    for (int i=0; i<G.num_tokens; i++)
        for (int j=0; j<G.num_tokens + G.num_symbols; j++)
            // Follow sets for terminals don't exist
            F.follow[i][j] = 0;
    
    return F;
}

ParseTable createParseTable(FirstAndFollow F, Grammar G){
    // Construct the parse table using Grammar and
    // the First, Follow sets
    ParseTable P;
    P.matrix = (int **) calloc (F.num_symbols + F.num_tokens + 1, sizeof(int*)); 
    for (int i=0; i<F.num_tokens + F.num_symbols + 1; i++) 
         P.matrix[i] = (int *) calloc (F.num_tokens + 1, sizeof(int));
    
    for (int i=0; i<G.num_rules; i++)
        for (int j=0; j<G.num_symbols; j++)
        P.matrix[i][j] = -1;
    
    P.num_symbols = F.num_symbols;
    P.num_tokens = F.num_tokens;
    
    for(int i=0; i<G.num_rules; i++) {
        int A = G.rules[i].left;
        int B = G.rules[i].right[0];
        
        if (B == TK_EPSILON) {
            // If the rule is A -> E, we need
            // to insert this rule to all the 
            // elements in the follow of A
            for(int j=1; j<G.num_tokens; j++) {
                if(F.follow[A][j] == 1)
                    P.matrix[A][j] = i;
            }
            // Set p[A][B] as 1!!
            P.matrix[A][B] = i;
            continue;
        }
        
        for(int j=1; j<G.num_tokens; j++) {
            // Add nonterminal rules which don't have epsilon on it's
            // first set. We add the corresponding rule number
            if(F.first[B][j] == 1) {
                P.matrix[A][j] = i;
            }
        }
        
        // Otherwise, if there's an epsilon, simply add
        // it's follow set to the correct index
        if(F.first[B][0] == 1) {
            for(int j=1; j<G.num_tokens; j++) {
                if(F.follow[B][j] == 1)
                    P.matrix[A][j] = i;
            }   
        }
    }
    return P;
}

TreeNode* generateParseTree (char* filename, ParseTable p, Grammar g) {
    // Generates the Parse Tree from the Grammar rules and the
    // parse table. The tokens are retrieved from the Lexer.
    Token t = {TK_DOLLAR, NULL, -1};
    TreeNode* dollar = make_tree_node(NULL, t);
    StackNode* stack = make_stack_node(dollar);
    
    t.token_type = program;
    TreeNode* root = make_tree_node(NULL, t);
    stack = push(stack, root);

    // Invoke the Lexer
    init_tokenizer(filename);
    
    // Process the next token
    t = get_next_token();
    
    bool is_complete = false;
    
    bool is_correct = true;
        
    while (!is_empty(stack)) {
        // Until the stack is empty, process
        // tokens using get_next_token()
        if (t.token_type == TK_ERROR) {
            // Shout for a Syntax Error
            fprintf(stderr, "Syntax Error: At line number: %d\n", t.line_no);
            t = get_next_token();
            continue;
        }
        else if (t.token_type == TK_DOLLAR) {
            // We reached the EOF while parsing.
            // We're almost done
            // Set all remaining rules to E, if possible
            // If any one of them can't resolve to E, then it's an error
            int rule_no = -1;
            while (!(stack->data->token.token_type == TK_DOLLAR)) {
                // printf("Stack contains %s\n", get_string_from_term(stack->data->token.token_type));
                if ((rule_no = p.matrix[stack->data->token.token_type][TK_EPSILON] != -1)) {
                    TreeNode* curr = stack->data;
                    Rule rule = g.rules[rule_no];
                    curr->children = (TreeNode**) calloc (rule.num_right, sizeof(TreeNode*));
                    curr->rule_no = rule_no;
                    curr->num_lhs = rule.num_lhs;
                    curr->num_children = rule.num_right;
                    // Resolves to Epsilon. Set it to this rule
                    // For some reason, g.rules[rule_no].num_right = 2 for <othermodules> -> E
                    // Add to the tree
                    Token temp = {TK_EPSILON, NULL, -1};
                    curr->children[0] = make_tree_node(curr, temp);
                }
                else {
                    // Boo Hoo. Error
                    fprintf(stderr, "Error during Parsing. Non Terminal Rule (LHS = %s) must resolve to Epsilon\n", get_string_from_term(stack->data->token.token_type));
                }

                stack = pop(stack);
            }
            if (is_correct) {
                printf("Parsed successfully!\n");
                break;
            }
            else {
                printf("Finished parsing with errors\n");
                break;
            }
        }
        else if (t.token_type == TK_COMMENTMARK) {
            // Ignore Comments
            t = get_next_token();
            continue;
        }
        if (stack->data->check_term == false) {
            // Non terminal
            TreeNode* curr = stack->data;
            // Look at the parse table
            // printf("Lookup at p[%s][%s]\n", get_string_from_term(curr->token.token_type), get_string_from_term(t.token_type));
            int rule_no = p.matrix[curr->token.token_type][t.token_type];
            if (rule_no == -1) {
                if (t.token_type != TK_COMMENTMARK) {
                    fprintf(stderr, "Syntax Error at Line %d: Got Token: %s\n", t.line_no, get_parser_token(t));
                    is_correct = false;
                }
                t = get_next_token();
                continue;
            }
            // Pop the stack
            stack = pop(stack);
            
            // Create a node for that token
            // and insert it into the tree, indexed by curr
            // So curr's child will have the new node
            Rule rule = g.rules[rule_no];
            curr->children = (TreeNode**) calloc (rule.num_right, sizeof(TreeNode*));
            curr->rule_no = rule_no;
            curr->num_lhs = rule.num_lhs;
            curr->num_children = rule.num_right;
            
            for (int i=0; i<rule.num_right; i++) {
                // Add to the tree
                Token temp = {rule.right[i], NULL, -1};
                curr->children[i] = make_tree_node(curr, temp);
            }
            
            for (int i=rule.num_right-1; i>=0; i--) {
                // Push to the stack in reverse order
                stack = push(stack, curr->children[i]);
            }
            continue;
        }
        else {
            // Is a terminal
            // Check current symbol from the top of stack
            TreeNode* curr = stack->data;
            if (curr->token.token_type == TK_EPSILON) {
                // Pop from the stack and add to curr
                stack = pop(stack);
                curr->token.lexeme = NULL;
                curr->token.line_no = -1;
                continue;
            }
            if (curr->token.token_type == t.token_type) {
                // Matched with stack
                if (t.token_type == TK_DOLLAR) {
                    // End of stack
                    is_complete = true;
                    break;
                }
                // Pop the stack and then go to the next token
                stack = pop(stack);
                curr->token.lexeme = t.lexeme;
                curr->token.line_no = t.line_no;
                t = get_next_token();
            }
            else {
                // Terminal from stack and Terminal from token stream don't match
                // Throw a Syntax Error and pop the current symbol from the stack
                // We'll try our best to continue parsing by looking for the next match
                fprintf(stderr, "Syntax Error at Line %d: Expected: %s, but got: %s\n", t.line_no, get_parser_token(stack->data->token), get_parser_token(t));
                // Pop from the stack
                stack = pop(stack);
                t = get_next_token();
                continue;
            }
        }
        if (is_complete)
            break;
    }

    // Free whatever is left in the stack
    StackNode* temp = stack;
    while(temp) {
        if (temp->data->token.token_type == TK_DOLLAR)
            free_parse_tree(temp->data);
        temp->data = NULL;
        StackNode* rmnode =  temp;
        temp = temp->next;
        rmnode->next = NULL;
        free(rmnode);
    }
    // Close the Lexer
    close_tokenizer();
    return root;
}

void pretty_print(TreeNode* node) {
    // Prints only one node
    if (!node)
        return;
    if (node->check_term == true) {
        // Terminal
        Token t = node->token;
        if (t.token_type == TK_EPSILON) {
            printf("-----\t");
            printf("-----\t");
        }
        else {
            printf("%s\t", node->token.lexeme);
            printf("%d\t", node->token.line_no);
        }
        printf("%s\t", get_string_from_term(node->token.token_type));
        if (t.token_type == TK_NUM)
            printf("%d\t", atoi(t.lexeme));
        else if (t.token_type == TK_RNUM) {
            for (int i=0; t.lexeme[i] != '\0'; i++)
                if (t.lexeme[i] == 'E')
                    t.lexeme[i] = 'e';
            printf("%.4f\t", atof(t.lexeme));
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
    if (!root)
        return;
    if (root->num_children == 0) {
        pretty_print(root);
        return;
    }
    for (int i=0; i<root->num_children; i++) {
        printParseTree(root->children[i]);
    }
}

/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Format: %s %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
    FILE* fp = fopen("grammar_rules.txt", "r");
    if (!fp) {
        perror("File 'grammar_rules.txt' does not exist\n");
        exit(EXIT_FAILURE);
    }
    Grammar g = populate_grammar(fp);
    print_grammar(g);
    FirstAndFollow f = ComputeFirstAndFollowSets(g);
    printFirstAndFollowSets(f);
    ParseTable p = createParseTable(f, g);
    printParseTable(p);
    
    char* filename = argv[1];
    printf("Generating Parse Tree...\n");
    TreeNode* root = generateParseTree(filename, p, g);
    printParseTree(root);
    printTreeNode(root);
    free_first_and_follow(f);
    free_parse_table(p);
    fclose(fp);
    free_grammar(g);
    free_tree(root);
    return 0;
}
*/
