#include "parserDef.h"
#include "parser.h"

unsigned long hash_func (char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

Rule make_rule(int left, int* right, int num_right) {
	Rule grule;
	grule.left = left;
	grule.right = right;
	grule.num_right = num_right;
	return grule;
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

    if (g.F.first) {
        for (int i=0; i<g.num_tokens + g.num_symbols + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (g.F.first[i])
                free(g.F.first[i]);
        }
        free(g.F.first);
    }
    
    if (g.F.follow) {
        for (int i=0; i<g.num_symbols + g.num_tokens + 2; i++) {
            // IF YOU GET A SEGFAULT LATER, BLAME ME FOR TRYING
            // TO FREE TK_DOLLAR
            if (g.F.follow[i])
                free(g.F.follow[i]);
        }
        free(g.F.follow);
    }
    
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

Grammar populate_grammar(FILE* fp) {
    Grammar G;
    G.rules = (Rule*) calloc (1, sizeof(Rule));
    G.rules[0].right = NULL;
    G.F.first = G.F.follow = NULL;
    G.num_rules = G.num_symbols = G.num_tokens = 0;
    
    int num;
    char ch = '\0';
    
    int curr_right = 0;
    int curr_rule = -1;
    
    bool is_lhs = false;
    
    char buffer[256];
    
    unsigned long (*hash_fun)(char*);
    
    hash_fun = &hash_func;
    
    size_t num_symbols = sizeof(symbols)/sizeof(Symbol);
    hash_table = create_table(CAPACITY*3, hash_fun);
    
    for (uint32_t i=0; i<num_symbols; i++) {
        hash_table = ht_insert(hash_table, symbols[i].key, symbols[i].tid);
    }
    
    int* temp = (int*) calloc (1, sizeof(int));
    int total = 0;
    
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
    //G.num_symbols++;
    free(temp);
    
    return G;
}

int Union(int* a ,int* b, int length) {
    // Performs a union operation (OR) between
    // two sets a and b, indexed using an array
    //leave 0 epsilon
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
    F.first = (int **)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int *)); 
    for (int i=0; i<G.num_symbols + G.num_tokens + 2; i++) 
         F.first[i] = (int *)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int));
    F.num_symbols = G.num_symbols;
    F.num_tokens = G.num_tokens;
	
    // Keep an extra index for $ (TK_DOLLAR)
    F.follow = (int **)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int *)); 
    for (int i=0; i<G.num_symbols+G.num_tokens+2; i++) 
         F.follow[i] = (int *)calloc(G.num_symbols + G.num_tokens + 2, sizeof(int));
	
    
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
    
    #ifdef DEBUG
    printf("Base cases done!\n");
    #endif

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
                // If we're at the last right symbol of i
                if(j == G.rules[i].num_right-1) {
                    // Then the follow of the rightmost symbol will be UNIONed with the follow of the leftmost!
                    // We also need to include TK_DOLLAR when updating, so the length of the segment mask is
                    // G.num_tokens + 1, and NOT G.num_tokens
                    int temp = Union(F.follow[G.rules[i].right[j]],  F.follow[G.rules[i].left], G.num_tokens + 1);
                    if(change != 1) {
                        change = temp;
                    }
                    break;
                }	
                
                // Otherwise, the follow of the current right symbol will be ORed with the first of the next right symbol
                int temp = Union(F.follow[G.rules[i].right[j]] , F.first[G.rules[i].right[j+1]], G.num_tokens + 1);
                if(change != 1)
                    change = temp;
                
                // Again, if Epsilon is there in the first of the next right symbol, consider the follow set of that
                // symbol instead, since Epsilon cannot appear on any follow set.
                if(F.first[G.rules[i].right[j+1]][0] == 1) {
                    int temp = Union(F.follow[G.rules[i].right[j]] , F.follow[G.rules[i].right[j+1]], G.num_tokens + 1);
                    if(change != 1)
                        change = temp;
                }
            }
        }		
    } while(change==1);
    
    return F;
}

ParseTable createParseTable(FirstAndFollow F, Grammar G){
    // Construct the parse table using Grammar and
    // the First, Follow sets
    ParseTable P;
    P.matrix = (int **) calloc (F.num_symbols + F.num_tokens + 1, sizeof(int*)); 
    for (int i=0; i<F.num_tokens + F.num_symbols + 1; i++) 
         P.matrix[i] = (int *) calloc (F.num_tokens + 1, sizeof(int));
    P.num_symbols = F.num_symbols;
    P.num_tokens = F.num_tokens;
    
    for(int i=0; i<G.num_rules; i++) {
        int A = G.rules[i].left;
        int B =	G.rules[i].right[0];
        
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

int main() {
    FILE* fp = fopen("grammar_rules.txt", "r");
    if (!fp) {
        perror("File does not exist\n");
        exit(EXIT_FAILURE);
    }
    Grammar g = populate_grammar(fp);
    print_grammar(g);
    FirstAndFollow f = ComputeFirstAndFollowSets(g);
    printFirstAndFollowSets(f);
    ParseTable p = createParseTable(f, g);
    printParseTable(p);
    free_first_and_follow(f);
    free_parse_table(p);
    fclose(fp);
    free_grammar(g);
    return 0;
}
