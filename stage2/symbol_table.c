#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include <assert.h>

int total_scope = 0;

char* get_string_from_type(TypeName typename) {
    switch(typename) {
        case TYPE_INTEGER:
        return "TYPE_INTEGER";
        case TYPE_BOOLEAN:
        return "TYPE_BOOLEAN";
        case TYPE_REAL:
        return "TYPE_REAL";
        case TYPE_ARRAY:
        return "TYPE_ARRAY";
        case TYPE_NONE:
        return "TYPE_NONE";
        case TYPE_FUNCTION:
        return "TYPE_FUNCTION";
        case TYPE_MODULE:
        return "TYPE_MODULE";
        case TYPE_ERROR:
        return "TYPE_ERROR";
        default:
        return "Not Yet Implemented";
    }
    return NULL;
}

TypeName get_typename_from_term(term token_type) {
    switch(token_type) {
        case TK_INTEGER:
            return TYPE_INTEGER;
        case TK_REAL:
            return TYPE_REAL;
        case TK_NUM:
            return TYPE_INTEGER;
        case TK_RNUM:
            return TYPE_REAL;
        case TK_BOOLEAN:
            return TYPE_BOOLEAN;
        case TK_ARRAY:
            return TYPE_ARRAY;
        case TYPE_ERROR:
            return TYPE_ERROR;
        default:
            return TYPE_NONE;
    }
}

unsigned long hash_function_symbol(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

void create_scope_table(SymbolHashTable*** symboltables_ptr, int index) {
    // Creates a scope table
    //printf("Entering a new scope. start_scope = %d\n...\n", start_scope);
    SymbolHashTable** symboltables = *symboltables_ptr;
    symboltables = (SymbolHashTable**) realloc (symboltables, (index + 1) * sizeof(SymbolHashTable*));
    // Update pointer to new memory location due to realloc
    *symboltables_ptr = symboltables;
    if (symboltables == NULL) {
        perror("Out of Memory\n");
        exit(1);
    }
    symboltables[index] = create_symtable(CAPACITY, hash_function_symbol);
    //printf("Entered new scope. Created a Scope Table\n");
}

void insert_into_symbol_table(SymbolHashTable*** symboltables_ptr, char* key, SymbolRecord* record, int index) {
    //printf("Inserting %s ...\n", key);
    SymbolHashTable** symboltables = *symboltables_ptr;
    symboltables[index] = st_insert(symboltables[index], key, record);
    //printf("Inserted %s successfully\n", key);
}

static SymbolRecord* st_search_scope(SymbolHashTable*** symboltables_ptr, char* key, int start_scope, int end_scope) {
    SymbolRecord* search;
    SymbolHashTable** symboltables = *symboltables_ptr;
    for (int i=start_scope; i>=end_scope; i--) {
        search = st_search(symboltables[i], key);
        if (search != NULL) {
            return search;
        }
    }
    return NULL;
}

int* get_offsets(ASTNode* node) {
    int* offsets = calloc(2, sizeof(int));
    if (node->token_type == dataType && node->children[0]->token_type == TK_ARRAY) {
        offsets[0] = atoi(node->children[1]->children[0]->token.lexeme);
        offsets[1] = atoi(node->children[1]->children[1]->token.lexeme);
    }
    return offsets;
}

static void process_module_declaration(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    SymbolRecord* record;
    ASTNode* moduleIDNode = root->children[0];
    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, 0, 0, 0, TK_EPSILON);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, 0);
    // Move onto local scope
    start_scope ++;
    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, start_scope);
}

static void process_module_definition(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Move to local scope
    start_scope ++;
    // Create a scope table
    create_scope_table(symboltables_ptr, start_scope);
    ASTNode* moduleIDNode = root->children[0];
    ASTNode* input_plistNode = root->children[1];
    ASTNode* retNode;
    //ASTNode* moduleDefNode;
    if (root->syn_attribute.token_type == TK_EPSILON) {
        retNode = NULL;
        //moduleDefNode = root->children[2];
    }
    else {
        retNode = root->children[2];
        //moduleDefNode = root->children[3];
    }
    
    // Module Name. Add to local and global Symbol table
    SymbolRecord* record;
    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, 0, 0, 0, TK_EPSILON);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, 0);

    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, start_scope);
    
    // Insert <input_plist>
    for (ASTNode* temp = input_plistNode; ; temp=temp->children[2]) {
        ASTNode* idNode = temp->children[0];
        ASTNode* typeNode = temp->children[1];
        int* offsets = get_offsets(typeNode);
        int a = offsets[0]; int b = offsets[1];
        free(offsets);
        if (a > b) {
            fprintf(stderr, "Semantic Error (Line No %d): Array Indices must be of the form [lower..higher]\n", typeNode->children[1]->children[0]->token.line_no);
            has_semantic_error = true;
            return;
        }
        SymbolRecord* record;
        if (typeNode->num_children > 1)
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->children[0]->token_type), start_scope, b-a, a, typeNode->children[2]->token_type);
        else
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->children[0]->token_type), start_scope, b-a, a, TK_EPSILON);
        insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
        if (temp->children[2] == NULL)
            break;
    }

    if (retNode == NULL) {
        printf("Module <<%s>> does not have a return type\n", moduleIDNode->token.lexeme);
    }
    else {
        // Insert <output_plist>
        ASTNode* output_plistNode = retNode;
        for (ASTNode* temp = output_plistNode; ; temp=temp->children[2]) {
            ASTNode* idNode = temp->children[0];
            ASTNode* typeNode = temp->children[1];
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->token_type), start_scope, 0, 0, TK_EPSILON);
            insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
            if (temp->children[2] == NULL)
                break;
        }
    }
}

static void process_driver_module(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Driver Module is inside it's own scope
    start_scope ++;
    // Create a scope table
    create_scope_table(symboltables_ptr, start_scope);
}

static void insert_identifier(SymbolHashTable*** symboltables_ptr, ASTNode* idNode, ASTNode* dataTypeNode) {
    // Inserts a single identifier, specified by datatype
    SymbolRecord* record;
    int* offsets = get_offsets(dataTypeNode);
    int a = offsets[0]; int b = offsets[1];
    free(offsets);
    if (a > b) {
        fprintf(stderr, "Semantic Error (Line No %d): Array Indices must be of the form [lower..higher]\n", dataTypeNode->children[1]->children[0]->token.line_no);
        has_semantic_error = true;
        return;
    }
    if (dataTypeNode->children[0]->token.token_type == TK_ARRAY)
        record = create_symbolrecord(idNode->token, get_typename_from_term(TK_ARRAY), start_scope, b-a, a, dataTypeNode->children[2]->token.token_type);
    else
        record = create_symbolrecord(idNode->token, get_typename_from_term(dataTypeNode->children[0]->token.token_type), start_scope, b-a, a, TK_EPSILON);
    insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
}

static void process_declaration_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    SymbolHashTable** symboltables = *symboltables_ptr;
    SymbolRecord* search;
    // <declarestmt> -> <idList> <dataType>
    // <idList> -> TK_ID <N3>
    // <N3> -> TK_COMMA TK_ID <N3> 
    // <N3> -> E
    ASTNode* idListNode = root->children[0];
    ASTNode* dataTypeNode = root->children[1];
    ASTNode* idNode = idListNode->children[0];

    search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, start_scope);
    if (search != NULL) {
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in current scope at Line No %d. Cannot be declared\n", idNode->token.line_no, idNode->token.lexeme, search->token.line_no);
        has_semantic_error = true;
        return;
    }
    search = st_search(symboltables[0], idNode->token.lexeme);
    if (search != NULL) {
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in global scope. Cannot be declared\n", idNode->token.line_no, idNode->token.lexeme);
        has_semantic_error = true;
        return;
    }

    insert_identifier(symboltables_ptr, idNode, dataTypeNode);

    // Check for the rest of the identifiers
    ASTNode* N3Node = idListNode->children[1];
    if (N3Node != NULL) {
        for (ASTNode* curr = N3Node; ;curr = curr->children[1]) {
            search = st_search_scope(symboltables_ptr, curr->children[0]->token.lexeme, start_scope, start_scope);
            if (search != NULL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in current scope. Cannot be declared\n", curr->children[0]->token.line_no, search->token.lexeme);
                has_semantic_error = true;
                return;
            }
            search = st_search_scope(symboltables_ptr, curr->children[0]->token.lexeme, 0, 0);
            if (search != NULL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in global scope. Cannot be declared\n", curr->children[0]->token.line_no, search->token.lexeme);
                has_semantic_error = true;
                return;
            }
            // Insert into the table
            insert_identifier(symboltables_ptr, curr->children[0], dataTypeNode);
            if (curr->children[1] == NULL)
                break;
        }
    }
}

static void get_type_of_expression(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Get the type of the expression
    // Will return TYPE_ERROR if there's a semantic error when processing the expression
    if (!root) return;
    root->visited = true;
    SymbolHashTable** symboltables = *symboltables_ptr;
    TypeName expr_type = TYPE_NONE;
    for (int i=0; i<root->num_children; i++) {
        if (root->children[i]->visited)
            continue;
        get_type_of_expression(symboltables_ptr, root->children[i]);
        if (error == 1) {
            root->visited = true;
            error = 1; expression_type = TYPE_ERROR;
            return;
        }       
    }
    if (root->token_type == var_id_num) {
        //printf("Reached var_id_num: %s\n", root->children[0]->token.lexeme);
        ASTNode* var_id_numNode = root;
        if (var_id_numNode->children[0]->token_type == TK_NUM)
            expr_type = TYPE_INTEGER;
        else if (var_id_numNode->children[0]->token_type == TK_RNUM)
            expr_type = TYPE_REAL;
        else {
            // TK_ID
            ASTNode* idNode = var_id_numNode->children[0];
            if (var_id_numNode->children[1] == NULL) {
                // Simply an identifier
                SymbolRecord* search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, end_scope);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not declared in scope\n", idNode->token.line_no, idNode->token.lexeme);
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
                expr_type = search->type_name;
                if (expression_type != TYPE_NONE && expr_type != expression_type) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", idNode->token.line_no, idNode->token.lexeme, get_string_from_type(expression_type));
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
            }
            else {
                // An array index
                SymbolRecord* search;
                ASTNode* idxNode = var_id_numNode->children[1];
                search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, end_scope);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not declared in scope\n", idNode->token.line_no, idNode->token.lexeme);
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
                expr_type = get_typename_from_term(search->element_type);
                if (expression_type != TYPE_NONE && expr_type != expression_type) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", idNode->token.line_no, idNode->token.lexeme, get_string_from_type(expression_type));
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
                if (idxNode->token.token_type == TK_NUM) {
                    expression_type = expr_type;
                    return;
                }
                search = st_search_scope(symboltables_ptr, idxNode->token.lexeme, start_scope, end_scope);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not declared in scope\n", idxNode->token.line_no, idxNode->token.lexeme);
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
                // Check if array index is of integer type
                // TODO: Add Dynamic Arrays
                if (search->type_name != TYPE_INTEGER) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in array index must be an integer\n", idxNode->token.line_no, idxNode->token.lexeme);
                    has_semantic_error = true;
                    error = 1; expression_type = TYPE_ERROR;
                    return;
                }
            }
        }
        if (expression_type != TYPE_NONE && expr_type != expression_type) {
            fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", var_id_numNode->children[0]->token.line_no, var_id_numNode->children[0]->token.lexeme, get_string_from_type(expression_type));
            has_semantic_error = true;
            error = 1; expression_type = TYPE_ERROR;
            return;
        }
        expression_type = expr_type;
        //printf("var_id_num returns %s\n", get_string_from_type(expression_type));
        return;
    }
    else if (root->token_type == boolConstt) {
        expr_type = TYPE_BOOLEAN;
        expression_type = expr_type;
    }
    else if (root->token_type == N7 || root->token_type == N8) {
        // LogicalOp or relationalOp
        set_to_boolean = true;
    }
    return;
}

static TypeName get_expression_type(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Remember to reset expression_type and error after every call
    error = 0; expression_type = TYPE_NONE; set_to_boolean = false;
    get_type_of_expression(symboltables_ptr, root);
    if ((expression_type !=  TYPE_NONE || expression_type != TYPE_ERROR) && set_to_boolean == true)
        expression_type = TYPE_BOOLEAN;
    TypeName expr_type = expression_type;
    // Remember to reset expression_type and error after every call
    error = 0; expression_type = TYPE_NONE; set_to_boolean = false;
    return expr_type;
}

static void process_expression(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    TypeName expr_type = get_expression_type(symboltables_ptr, root);
    if (expr_type != TYPE_NONE)
        printf("Type of Expression : %s\n", get_string_from_type(expr_type));
}

static void process_assignment_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    SymbolHashTable** symboltables = *symboltables_ptr;
    ASTNode* lvalueNode = root->children[0];
    // This must already be declared
    SymbolRecord* search = st_search_scope(symboltables_ptr, lvalueNode->token.lexeme, start_scope, end_scope);
    if (search == NULL) {
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' being assigned before declaration\n", lvalueNode->token.line_no, lvalueNode->token.lexeme);
        has_semantic_error = true;
        return;
    }

    if (root->children[1]->token_type != expression) {
        // Array
        ASTNode* idxNode = root->children[1];
        if (idxNode->token.token_type == TK_ID) {
            SymbolRecord* idx_search = st_search_scope(symboltables_ptr, idxNode->token.lexeme, start_scope, end_scope);
            if (idx_search == NULL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Lvalue array index %s being assigned before declaration\n", idxNode->token.line_no, idxNode->token.lexeme);
                has_semantic_error = true;
                return;
            }
        }
        TypeName expr_type = get_expression_type(symboltables_ptr, root);
        if (expr_type != TYPE_ERROR) {
            if (expr_type != get_typename_from_term(search->element_type)) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s'. Incompatible types for an lvalue assignment statement. lvalue is of type %s, while the assignment statement is of type %s\n", lvalueNode->token.line_no, lvalueNode->token.lexeme, get_string_from_type(get_typename_from_term(search->element_type)), get_string_from_type(expr_type));
                has_semantic_error = true;
            }
        }
        return;
    }
    // Now the type expressions must match
    // Consider the rhs
    // ASTNode* whichStmtNode = root->children[1];
    TypeName expr_type = get_expression_type(symboltables_ptr, root);
    if (expr_type != TYPE_ERROR) {
        if (expr_type != search->type_name) {
            fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s'. Incompatible types for an lvalue assignment statement. lvalue is of type %s, while the assignment statement is of type %s\n", lvalueNode->token.line_no, lvalueNode->token.lexeme, get_string_from_type(search->type_name), get_string_from_type(expr_type));
                has_semantic_error = true;
        }
    }
}

static void process_module_reuse(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // <optional> => <output_plist> and <idList> => <input_plist>
    // Case 1: If <optional> is not E
    SymbolHashTable** symboltables = *symboltables_ptr;
    if (root->syn_attribute.token_type == TK_EPSILON) {
        // <optional> -> E
    }
    else {
        assert (root->syn_attribute.token_type == optional);
        if (root->children[0]->num_children == 0) {
            // <N3> -> E
            // Optional has only a single identifier
            ASTNode* idNode = root->children[0];
            SymbolRecord* search;
            // Search the local scope first
            search = st_search(symboltables[start_scope], idNode->token.lexeme);
            if (search == NULL) {
                // Otherwise Search the global scope
                search = st_search(symboltables[0], idNode->token.lexeme);
                if (search == NULL) {
                    // Not found. Error
                    fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' not found in scope, but used in module call statement\n", idNode->token.line_no, idNode->token.lexeme);
                    has_semantic_error = true;
                    return;
                }
            }
            // Otherwise, now check types of parameters
        }
        else {
            for (ASTNode* temp=root->children[0]; ; temp=temp->children[1]) {
                ASTNode* idNode = temp->children[0];
                SymbolRecord* search;
                // Search the local scope first
                search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, end_scope);
                if (search == NULL) {
                    // Otherwise Search the global scope
                    search = st_search(symboltables[0], idNode->token.lexeme);
                    if (search == NULL) {
                        // Not found. Error
                        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' not found\n", idNode->token.line_no, idNode->token.lexeme);
                        has_semantic_error = true;
                        return;
                    }
                }
                // Ssearch must have the matched variable
                // Now check types of parameters
                // Result types must match <output_plist>
                // Maintain a Stack / List of all modules with their scopes.
                // This list is used to lookup parameter lists and verify types
                if (temp->children[1]== NULL)
                    break;
            }
        }
    }
}

static void process_iterative_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    assert (root->token_type == iterativeStmt);
    
    // Start a new scope and create a new scope table
    start_scope ++;
    create_scope_table(symboltables_ptr, start_scope);

    SymbolHashTable** symboltables = *symboltables_ptr;
    SymbolRecord* search = NULL;
    // An iterativestmt can be for or while
    if (root->children[0]->token_type == arithmeticOrBooleanExpr) {
        // Case 1: WHILE_STMT
        TypeName expr_type = get_expression_type(symboltables_ptr, root->children[0]);
        if (expr_type == TYPE_INTEGER) {
            // Should we convert it to expression != 0 ?
        }
        else if (expr_type == TYPE_REAL) {
            // Should we convert it to expression != 0.0 ?
        }
    }
    else {
        // Case 2: FOR_STMT
        ASTNode* idNode = root->children[0];
        ASTNode* rangeNode = root->children[1];
        for (int i=start_scope; i>=end_scope; i--) {
            search = st_search(symboltables[start_scope], idNode->token.lexeme);
            if (search != NULL)
                break;
        }
        if (search == NULL) {
            fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' in FOR statement was not declared in scope\n", idNode->token.line_no, idNode->token.lexeme);
            has_semantic_error = true;
            return;
        }
        else {
            TypeName type = search->type_name;
            if (type != TYPE_INTEGER) {
                fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' in FOR statement is of type %s. Expected integer type\n", idNode->token.line_no, idNode->token.lexeme, get_string_from_type(type));
                has_semantic_error = true;
                return;
            }
        }
        int lower = atoi(rangeNode->children[0]->token.lexeme);
        int upper = atoi(rangeNode->children[1]->token.lexeme);
        if (lower > upper) {
            fprintf(stderr, "Semantic Error (Line No %d): a FOR statement must be of the form lower..higher\n", rangeNode->children[0]->token.line_no);
            has_semantic_error = true;
            return;
        }
    }
}

void perform_semantic_analysis(SymbolHashTable*** symboltables_ptr, ASTNode* root, int enna_child) {
    // Performs all required Semantic Checks using the Symbol Table
    if (!root)
        return;
    SymbolHashTable** symboltables = *symboltables_ptr; // Careful with realloc()
    if (root->token_type == TK_END) {
        // End of Scope
        end_scope ++;
        printf("Now, start_scope = %d, end_scope = %d\n", start_scope, end_scope);
    }
    else if (root->token_type == moduleDeclaration) {
        // Module Declaration. Add the moduleID to both local and global scopes.
        // Nested Declarations are not allowed, so this directly goes only to the global scope table
        // Can we have a module definition before a declaration?
        SymbolRecord* search = st_search(symboltables[0], root->children[0]->token.lexeme);
        if (search != NULL) {
            fprintf(stderr, "Semantic Error (Line No: %d): Function definition before a declaration\n", search->token.line_no);
            has_semantic_error = true;
        }
        else
            process_module_declaration(symboltables_ptr, root);
    }
    else if (root->token_type == module) {
        // Module Definition. Add moduleID to both local and global scope
        // Add input_plist to it's local scope and output_plist
        // Check in global scope
        SymbolRecord* search = st_search(symboltables[0], root->children[0]->token.lexeme);
        if (search != NULL) {
            // Definition before declaration. Not allowed
            fprintf(stderr, "Semantic Error (Line No: %d): Function declaration is redundant\n", search->token.line_no);
            has_semantic_error = true;
        }
        else {
            process_module_definition(symboltables_ptr, root);
        }
    }
    else if (root->token_type == assignmentStmt) {
        process_assignment_statement(symboltables_ptr, root);
    }
    else if (root->token_type == moduleReuseStmt) {
        process_module_reuse(symboltables_ptr, root);
    }
    else if (root->token_type == declareStmt) {
        process_declaration_statement(symboltables_ptr, root);
    }
    else if (root->token_type == driverModule) {
        process_driver_module(symboltables_ptr, root);
    }
    else if (root->token_type == iterativeStmt) {
        process_iterative_statement(symboltables_ptr, root);
    }
    //else if (root->token_type == arithmeticExpr) {
    //    process_expression(symboltables_ptr, root);
    //}
    for (int i=0; i<root->num_children; i++)
        perform_semantic_analysis(symboltables_ptr, root->children[i], i);
}

SymbolHashTable*** createSymbolTables(ASTNode* root) {
    // Creates the Symbol Tables; one for every scope
    SymbolHashTable*** symboltables_ptr = calloc (1, sizeof(SymbolHashTable**));
    symboltables_ptr[0] = calloc (1, sizeof(SymbolHashTable*));
    symboltables_ptr[0][0] = create_symtable(CAPACITY, hash_function_symbol);
    ASTNode* temp = root;
    perform_semantic_analysis(symboltables_ptr, temp, 0);
    total_scope = start_scope;
    return symboltables_ptr;
}

SymbolRecord* create_symbolrecord(Token token, TypeName type_name, int scope_label, int total_size, int offset, term element_type) {
    SymbolRecord* symbolrecord = (SymbolRecord*) calloc (1, sizeof(SymbolRecord));
    symbolrecord->token = token;
    symbolrecord->type_name = type_name;
    symbolrecord->scope_label = scope_label;
    symbolrecord->total_size = total_size;
    symbolrecord->offset = offset;
    symbolrecord->element_type = element_type;
    return symbolrecord;
}

static SymbolLinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    SymbolLinkedList* list = (SymbolLinkedList*) calloc (1, sizeof(SymbolLinkedList));
    return list;
}

static SymbolLinkedList* linkedlist_insert(SymbolLinkedList* list, St_item* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        SymbolLinkedList* head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
    
    else if (list->next == NULL) {
        SymbolLinkedList* node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    SymbolLinkedList* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }
    
    SymbolLinkedList* node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    
    return list;
}

static void free_linkedlist(SymbolLinkedList* list) {
    SymbolLinkedList* temp = list;
    if (!list)
        return;
    while (list) {
        temp = list;
        list = list->next;
        if (temp->item->key)
            free(temp->item->key);
        if (temp->item->value)
            free_symrecord(temp->item->value);
        //free(temp->item->value);
        if (temp->item)
            free(temp->item);
        free(temp);
    }
}

static SymbolLinkedList** create_overflow_buckets(SymbolHashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    SymbolLinkedList** buckets = (SymbolLinkedList**) calloc (table->size, sizeof(SymbolLinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(SymbolHashTable* table) {
    // Free all the overflow bucket lists
    SymbolLinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}


St_item* create_symitem(char* key, SymbolRecord* value) {
    // Creates a pointer to a new hash table item
    St_item* item = (St_item*) malloc (sizeof(St_item));
    item->key = (char*) calloc (strlen(key) + 1, sizeof(char));
    //item->value = (char*) calloc (strlen(value) + 1, sizeof(char));
    
    strcpy(item->key, key);
    item->value = value;
    //strcpy(item->value, value);

    return item;
}

SymbolHashTable* create_symtable(int size, unsigned long (*hash_fun)(char*)) {
    // Creates a new SymbolHashTable
    SymbolHashTable* table = (SymbolHashTable*) malloc (sizeof(SymbolHashTable));
    table->hash_function = hash_fun;
    table->size = size;
    table->count = 0;
    table->items = (St_item**) calloc (table->size, sizeof(St_item*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);

    return table;
}

void free_symitem(St_item* item) {
    // Frees an item
    if (!item)
        return;
    if (item->key)
        free(item->key);
    if (item->value) {
        free(item->value);
    }
    free(item);
}

void free_symrecord(SymbolRecord* record) {
    if (!record)
        return;
    free(record);
}

void free_symtable(SymbolHashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        St_item* item = table->items[i];
        if (item != NULL)
            free_symitem(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision_sym(SymbolHashTable* table, unsigned long index, St_item* item) {
    SymbolLinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

SymbolHashTable* st_insert(SymbolHashTable* table, char* key, SymbolRecord* value) {
    //printf("Inserting %s : %s\n", item->key, get_string_from_SymbolRecord*(item->value));

    // Compute the index
    int index = table->hash_function(key);

    St_item* current_item = table->items[index];
    
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Avoid this free, since our value is now a pointer to a struct!!!
            // free_symitem(item);
            return table;
        }
        
        // Insert directly
        St_item* item = create_symitem(key, value);
        table->items[index] = item; 
        //printf("Inserting %s : %s\n", table->items[index]->key, get_string_from_SymbolRecord*(table->items[index]->value));
        table->count++;
    }

    else {
            // Scenario 1: We only need to update value
            if (strcmp(current_item->key, key) == 0) {
                //free(table->items[index]->value);
                //table->items[index]->value = (char*) calloc (strlen(value) + 1, sizeof(char));
                //strcpy(table->items[index]->value, value);
                if (table->items[index]->value)
                    free_symrecord(table->items[index]->value);
                table->items[index]->value = value;
                // Avoid this free, since our value is now a pointer to a struct!!!
                // free_symitem(item);
                return table;
            }
    
        else {
            // Scenario 2: Collision
            // Create the item
            St_item* item = create_symitem(key, value);
            handle_collision_sym(table, index, item);
            return table;
        }
    }
    return table;
}

SymbolRecord* st_search(SymbolHashTable* table, char* key) {
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = table->hash_function(key);
    St_item* item = table->items[index];
    SymbolLinkedList* head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}

void st_delete(SymbolHashTable* table, char* key) {
    // Deletes an item from the table
    int index = table->hash_function(key);
    St_item* item = table->items[index];
    SymbolLinkedList* head = table->overflow_buckets[index];

    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && strcmp(item->key, key) == 0) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_symitem(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (strcmp(item->key, key) == 0) {
                // Remove this item and set the head of the list
                // as the new item
                
                free_symitem(item);
                SymbolLinkedList* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_symitem(node->item->key, node->item->value);
                node->item->key = NULL;
                node->item->value = NULL;
                free_linkedlist(node);
                table->overflow_buckets[index] = head;
                return;
            }

            SymbolLinkedList* curr = head;
            SymbolLinkedList* prev = NULL;
            
            while (curr) {
                if (strcmp(curr->item->key, key) == 0) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }

        }
    }

}

void print_symrecord(SymbolRecord* t, char ch) {
    // Prints the SymbolRecord* type
    printf("token_type = %s , ", get_string_from_type(t->type_name));
    if (t->token.lexeme) {
        printf("lexeme = %s , ", t->token.lexeme);
    }
    printf("scope_label = %d , ", t->scope_label);
    printf("total_size = %d , ", t->total_size);
    printf("offset = %d , ", t->offset);
    printf("element_type = %s%c", get_string_from_term(t->element_type), ch);
}

void print_search_symtable(SymbolHashTable* table, char* key) {
    SymbolRecord* val;
    if ((val = st_search(table, key)) == NULL) {
        printf("%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Value:", key);
        print_symrecord(val, '\n');
    }
}

void print_symtable(SymbolHashTable* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:", i, table->items[i]->key);
            print_symrecord(table->items[i]->value, '\0');
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                SymbolLinkedList* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%s, Value:" , head->item->key);
                    print_symrecord(head->item->value, ' ');
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}

void print_symtables(SymbolHashTable** tables, int num_tables) {
    // Table #0 is for global scoped variables
    for (int i=0; i<=num_tables; i++) {
        printf("Table #%d\n\n", i);
        print_symtable(tables[i]);
    }
}

void free_symtables(SymbolHashTable** tables, int num_tables) {
    // Index #0 is for global scope
    for (int i=0; i<=num_tables; i++) if (tables[i]) free_symtable(tables[i]);
    free(tables);
}
