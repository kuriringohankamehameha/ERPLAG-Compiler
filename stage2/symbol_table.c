#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "stack.h"
#include "function_table.h"
#include <assert.h>

int total_scope = 0;
extern FunctionTable* function_tables;

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

static inline void clear_expression_array() {
    // Clears the expression parameter array
    expression_array_params[0] = (int)TYPE_NONE;
    expression_array_params[1] = -1;
    expression_array_params[2] = -1;
    expression_array_params[3] = -1;
    expression_array_name = NULL;
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
    if (start_scope == 0 && end_scope == 0) {
        search = st_search(symboltables[0], key);
        if (search != NULL) {
            return search;
        }
        return NULL;
    }
    int module_scope = modules[module_index];
    for (Stack* head = scope_stacks[module_index]; head != NULL; head=head->next) {
        if (head->data == 0)
            break;
        search = st_search(symboltables[head->data], key);
        if (search != NULL) {
            return search;
        }
    }
    search = st_search(symboltables[module_scope], key);
    if (search != NULL) {
        return search;
    }
    return NULL;
}

void get_offsets(ASTNode* node) {
    if (node->token_type == dataType && node->children[0]->token_type == TK_ARRAY) {
        if (node->children[1]->children[0]->token_type == TK_ID) {
            // Variable Index
            array_offset.offset_id = (node->children[1]->children[0]->token);
            array_offset.offset = -1;
        }
        else {
            array_offset.offset = atoi(node->children[1]->children[0]->token.lexeme);
            array_offset.offset_id.lexeme = NULL;
        }
        if (node->children[1]->children[1]->token_type == TK_ID) {
            // Variable Index
            array_offset.end_id = (node->children[1]->children[1]->token);
            array_offset.end = -1;
        }
        else {
            array_offset.end = atoi(node->children[1]->children[1]->token.lexeme);
            array_offset.end_id.lexeme = NULL;
        }
    }
}

static void process_module_declaration(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    ASTNode* moduleIDNode = root->children[0];
    SymbolHashTable** symboltables = *symboltables_ptr;
    SymbolRecord* search = st_search(symboltables[0], moduleIDNode->token.lexeme);
    if (search != NULL) {
        fprintf(stderr, "Semantic Error (Line No %d): Module ID '%s' already defined in global scope\n", moduleIDNode->token.line_no, moduleIDNode->token.lexeme);
        has_semantic_error = true;
        return;
    }
    
    // Create a new scope and a scope table
    start_scope ++;
    module_index ++;
    modules[module_index] = start_scope;
    create_scope_table(symboltables_ptr, start_scope);
    function_tables = realloc (function_tables, (start_scope + 1) * sizeof(FunctionTable));

    // Use the scope in the global scope table as a reference when encountering module declaration
    SymbolRecord* record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON, NULL, NULL);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, 0);
    // Move onto local scope
    // start_scope ++;
    // record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON, NULL, NULL);
    // insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, start_scope);
}

static void process_module_definition(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    ASTNode* moduleIDNode = root->children[0];
    ASTNode* input_plistNode = root->children[1];
    ASTNode* retNode;
    // Search in the global scope. It may be present, due to moduledeclaration
    SymbolRecord* search = st_search_scope(symboltables_ptr, moduleIDNode->token.lexeme, 0, 0);
    if (search != NULL) {
        fprintf(stderr, "Semantic Error (Line No %d): Module '%s' already declared at Line %d\n", moduleIDNode->token.line_no, moduleIDNode->token.lexeme, search->token.line_no);
        has_semantic_error = true;
        return;
    }
    // Move to local scope
    start_scope ++;
    module_index ++;
    //realloc_ft(module_index + 1);
    modules[module_index] = start_scope;
    // Create a scope table
    create_scope_table(symboltables_ptr, start_scope);
    function_tables = realloc (function_tables, (start_scope + 1) * sizeof(FunctionTable));

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
    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON, NULL, NULL);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, 0);

    record = create_symbolrecord(moduleIDNode->token, TYPE_MODULE, start_scope, 0, 0, TK_EPSILON, NULL, NULL);
    insert_into_symbol_table(symboltables_ptr, moduleIDNode->token.lexeme, record, start_scope);
    
    // Insert <input_plist>
    for (ASTNode* temp = input_plistNode; ; temp=temp->children[2]) {
        ASTNode* idNode = temp->children[0];
        ASTNode* typeNode = temp->children[1];
        get_offsets(typeNode);
        if(array_offset.offset != -1 && array_offset.end != -1) { 
            if (array_offset.offset > array_offset.end) {
                fprintf(stderr, "Semantic Error (Line No %d): Array Indices must be of the form [lower..higher]\n", typeNode->children[1]->children[0]->token.line_no);
                has_semantic_error = true;
                // Clear array offset parameters
                array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            }
        }
        SymbolRecord* record;
        if (typeNode->num_children > 1)
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->children[0]->token_type), start_scope, array_offset.end, array_offset.offset, typeNode->children[2]->token_type, array_offset.offset_id.lexeme, array_offset.end_id.lexeme);
        else
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->children[0]->token_type), start_scope, array_offset.end, array_offset.offset, TK_EPSILON, array_offset.offset_id.lexeme, array_offset.end_id.lexeme);

        // Insert it into the symbol table
        insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
        // Also put it into the ft record of the module
        add_input_parameter(idNode->token.lexeme, start_scope);

        if (array_offset.offset_id.lexeme) {
            // Add the dynamic array identifiers into scope
            SymbolRecord* search = st_search_scope(symboltables_ptr, array_offset.offset_id.lexeme, start_scope, start_scope);
            if (search != NULL) {
                fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' already in scope\n", array_offset.offset_id.line_no, array_offset.offset_id.lexeme);
                has_semantic_error = true;
                array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
                return;
            }
            record = create_symbolrecord(array_offset.offset_id, TYPE_INTEGER, start_scope, -1, -1, TK_EPSILON, NULL, NULL);
            insert_into_symbol_table(symboltables_ptr, array_offset.offset_id.lexeme, record, start_scope);
            add_input_parameter(array_offset.offset_id.lexeme, start_scope);
        }

        if (array_offset.end_id.lexeme) {
            // Add the dynamic array identifiers into scope
            SymbolRecord* search = st_search_scope(symboltables_ptr, array_offset.end_id.lexeme, start_scope, start_scope);
            if (search != NULL) {
                fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' already in scope\n", array_offset.end_id.line_no, array_offset.end_id.lexeme);
                has_semantic_error = true;
                array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
                return;
            }
            record = create_symbolrecord(array_offset.end_id, TYPE_INTEGER, start_scope, -1, -1, TK_EPSILON, NULL, NULL);
            insert_into_symbol_table(symboltables_ptr, array_offset.end_id.lexeme, record, start_scope);
            add_input_parameter(array_offset.end_id.lexeme, start_scope);
        }
            
        // Clear array offset parameters
        array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
        
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
            record = create_symbolrecord(idNode->token, get_typename_from_term(typeNode->token_type), start_scope, 0, 0, TK_EPSILON, NULL, NULL);
            insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
            add_output_parameter(idNode->token.lexeme, start_scope);
            if (temp->children[2] == NULL)
                break;
        }
    }
}

static void process_driver_module(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Driver Module is inside it's own scope
    if (root == NULL) return;
    start_scope ++;
    module_index ++;
    modules[module_index] = start_scope;
    // Create a scope table
    create_scope_table(symboltables_ptr, start_scope);
    function_tables = realloc (function_tables, (start_scope + 1) * sizeof(FunctionTable));
}

static void insert_identifier(SymbolHashTable*** symboltables_ptr, ASTNode* idNode, ASTNode* dataTypeNode) {
    // Inserts a single identifier, specified by datatype
    SymbolRecord* record;
    get_offsets(dataTypeNode);
    if(array_offset.offset != -1 && array_offset.end != -1) { 
        if (array_offset.offset > array_offset.end) {
            fprintf(stderr, "Semantic Error (Line No %d): Array Indices must be of the form [lower..higher]\n", dataTypeNode->children[1]->children[0]->token.line_no);
            has_semantic_error = true;
            // Clear array offset parameters
            array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            return;
        }
    }
    if (dataTypeNode->children[0]->token.token_type == TK_ARRAY)
        record = create_symbolrecord(idNode->token, get_typename_from_term(TK_ARRAY), start_scope, array_offset.end, array_offset.offset, dataTypeNode->children[2]->token.token_type, array_offset.offset_id.lexeme, array_offset.end_id.lexeme);
    else
        record = create_symbolrecord(idNode->token, get_typename_from_term(dataTypeNode->children[0]->token.token_type), start_scope, array_offset.end, array_offset.offset, TK_EPSILON, array_offset.offset_id.lexeme, array_offset.end_id.lexeme);
    // Clear array offset parameters
    array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
    insert_into_symbol_table(symboltables_ptr, idNode->token.lexeme, record, start_scope);
    add_variable_ft(idNode->token.lexeme, start_scope);
    
    if (array_offset.offset_id.lexeme) {
        // Add the dynamic array identifiers into scope
        SymbolRecord* search = st_search_scope(symboltables_ptr, array_offset.offset_id.lexeme, start_scope, start_scope);
        if (search == NULL) {
            fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' not declared in scope\n", array_offset.offset_id.line_no, array_offset.offset_id.lexeme);
            has_semantic_error = true;
            array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            return;
        }
        if (search->type_name != TYPE_INTEGER) {
            // Error. The identifier for dynamic arrays must be integer types
            fprintf(stderr, "Semantic Error (Line No %d): Dynamic Array index '%s' must be an integer\n", array_offset.offset_id.line_no, array_offset.offset_id.lexeme);
            has_semantic_error = true;
            array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            return;
        }
    }

    if (array_offset.end_id.lexeme) {
        // Add the dynamic array identifiers into scope
        SymbolRecord* search = st_search_scope(symboltables_ptr, array_offset.end_id.lexeme, start_scope, start_scope);
        if (search == NULL) {
            fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' not declared in scope\n", array_offset.end_id.line_no, array_offset.end_id.lexeme);
            has_semantic_error = true;
            array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            return;
        }
        if (search->type_name != TYPE_INTEGER) {
            // Error. The identifier for dynamic arrays must be integer types
            fprintf(stderr, "Semantic Error (Line No %d): Dynamic Array index '%s' must be an integer\n", array_offset.end_id.line_no, array_offset.end_id.lexeme);
            has_semantic_error = true;
            array_offset.offset = -1; array_offset.end = -1; array_offset.offset_id.lexeme = NULL; array_offset.end_id.lexeme = NULL;
            return;
        }
    }
}

static bool is_module_parameter(SymbolHashTable*** symboltables_ptr, char* id) {
    return search_function_table_input_parameter(id, module_index);
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

    search = st_search(symboltables[start_scope], idNode->token.lexeme);
    if (search != NULL && !(is_module_parameter(symboltables_ptr, idNode->token.lexeme))) {
        // There is an exception if the matched identifier corresponds to an input_plist
        // or an output_plist node
        // This check can be overridden
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in current scope at Line No %d. Cannot be declared\n", idNode->token.line_no, idNode->token.lexeme, search->token.line_no);
        has_semantic_error = true;
    }
    search = st_search(symboltables[0], idNode->token.lexeme);
    if (search != NULL) {
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in global scope. Cannot be declared\n", idNode->token.line_no, idNode->token.lexeme);
        has_semantic_error = true;
    }

    insert_identifier(symboltables_ptr, idNode, dataTypeNode);

    // Check for the rest of the identifiers
    ASTNode* N3Node = idListNode->children[1];
    if (N3Node != NULL) {
        for (ASTNode* curr = N3Node; ;curr = curr->children[1]) {
            search = st_search(symboltables[start_scope], curr->children[0]->token.lexeme);
            if (search != NULL && !(is_module_parameter(symboltables_ptr, curr->children[0]->token.lexeme))) {
                // There is an exception if the matched identifier corresponds to an input_plist
                // or an output_plist node
                // This check can be overridden
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in current scope. Cannot be declared\n", curr->children[0]->token.line_no, search->token.lexeme);
                has_semantic_error = true;
            }
            search = st_search_scope(symboltables_ptr, curr->children[0]->token.lexeme, 0, 0);
            if (search != NULL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' already in global scope. Cannot be declared\n", curr->children[0]->token.line_no, search->token.lexeme);
                has_semantic_error = true;
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
    TypeName expr_type = TYPE_NONE;
    if (root->token_type == AnyTerm) {
        if (root->children[0]->token_type == arithmeticExpr) {
            if (root->children[1] != NULL) {
                // RelatonalOp
                // Convert it to boolean
                convert_to_bool = true;
                get_type_of_expression(symboltables_ptr, root->children[0]);
                if (expression_type == TYPE_ERROR) {
                    convert_to_bool = false;
                    return;
                }
                for (ASTNode* exprNode=root->children[1]; ; exprNode = exprNode->children[1]) {
                    if (exprNode == NULL || exprNode->children == NULL) break;
                    get_type_of_expression(symboltables_ptr, exprNode->children[0]);
                    exprNode->visited = true;
                    if (expression_type == TYPE_ERROR) {
                        convert_to_bool = false;
                    }
                }
                expression_type = TYPE_BOOLEAN;
                convert_to_bool = false; // Just in case I do something stupid
                return;
            }
        }
    }
    else if (root->token_type == N4 || root->token_type == U || root->token_type == N5) {
        // N4 or Unary Operator or N5 cannot have array operands
        if (root->token_type == U) {
            ASTNode* expressionNode = root->parent->children[1];
            if (expression_type != TYPE_INTEGER && expression_type != TYPE_REAL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Unary Operand can only apply to an arithmetic expression of type Integer or Real\n", root->children[0]->token.line_no);
                has_semantic_error = 1;
                expression_type = TYPE_ERROR;
                return;
            }
            return;
        }
        ASTNode* prev_expressionNode = root->parent->children[0];
        get_type_of_expression(symboltables_ptr, prev_expressionNode);
        if (expression_type == TYPE_ARRAY) {
            fprintf(stderr, "Semantic Error (Line No: %d): Arrays cannot be used with operand %s\n", expression_array_params[3], get_string_from_term(root->syn_attribute.token_type));
            has_semantic_error = true;
            expression_type = TYPE_ERROR;
            return;
        }
    }
    else if (root->token_type == var_id_num) {
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
                    expression_type = TYPE_ERROR;
                    return;
                }
                expr_type = search->type_name;
                // If the expression type is an array, there should be nothing else
                if (expr_type == TYPE_ARRAY) {
                    expression_array_params[0] = (int)get_typename_from_term(search->element_type);
                    expression_array_params[1] = search->offset;
                    expression_array_params[2] = search->end;
                    expression_array_params[3] = idNode->token.line_no;
                    expression_array_name = search->token.lexeme;
                    if (search->offset_id != NULL && search->end_id != NULL) {
                        // Dynamic Arrays
                        is_dynamic = true;
                    }
                }
                if (convert_to_bool != true) {
                    if (expression_type != TYPE_NONE && expr_type != expression_type) {
                        if (expression_type != TYPE_ERROR) {
                            // Don't continue printing useless messages
                            fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", idNode->token.line_no, idNode->token.lexeme, get_string_from_type(expression_type));
                        }
                        has_semantic_error = true;
                        expression_type = TYPE_ERROR;
                        return;
                    }
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
                    expression_type = TYPE_ERROR;
                    return;
                }
                expr_type = get_typename_from_term(search->element_type);
                if (convert_to_bool != true) {
                    if (expression_type != TYPE_NONE && expr_type != expression_type) {
                        if (expression_type != TYPE_ERROR) {
                            fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", idNode->token.line_no, idNode->token.lexeme, get_string_from_type(expression_type));
                        }
                        has_semantic_error = true;
                        expression_type = TYPE_ERROR;
                        return;
                    }
                }
                if (idxNode->token.token_type == TK_NUM) {
                    // Check if array index integer within bounds
                    // Integer
                    // Check for bounds
                    int value = atoi(idxNode->token.lexeme);
                    if (value >= search->end || value < search->offset) {
                        fprintf(stderr, "Semantic Error (Line No: %d): Array index for '%s' out of bounds\n", idxNode->token.line_no, var_id_numNode->children[0]->token.lexeme);
                        has_semantic_error = true;
                        expression_type = TYPE_ERROR;
                        return;
                    }
                    else {
                        expression_type = expr_type;
                        return;
                    }
                }
                search = st_search_scope(symboltables_ptr, idxNode->token.lexeme, start_scope, end_scope);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not declared in scope\n", idxNode->token.line_no, idxNode->token.lexeme);
                    has_semantic_error = true;
                    expression_type = TYPE_ERROR;
                    return;
                }
                // Check if array index is of integer type
                // TODO: Add Dynamic Arrays
                if (search->type_name != TYPE_INTEGER) {
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in array index must be an integer\n", idxNode->token.line_no, idxNode->token.lexeme);
                    has_semantic_error = true;
                    expression_type = TYPE_ERROR;
                    return;
                }
            }
        }
        if (convert_to_bool == false) {
            if (expression_type != TYPE_NONE && expr_type != expression_type) {
                if (expression_type != TYPE_ERROR)
                    fprintf(stderr, "Semantic Error (Line No: %d) : Identifier '%s' used in arithmetic expression not conforming to type %s\n", var_id_numNode->children[0]->token.line_no, var_id_numNode->children[0]->token.lexeme, get_string_from_type(expression_type));
                has_semantic_error = true;
                expression_type = TYPE_ERROR;
                return;
            }
        }
        expression_type = expr_type;
        //printf("var_id_num returns %s\n", get_string_from_type(expression_type));
    }
    else if (root->token_type == boolConstt) {
        expr_type = TYPE_BOOLEAN;
        expression_type = expr_type;
    }
    else if (root->token_type == N7 || root->token_type == N8) {
        // LogicalOp or relationalOp
        set_to_boolean = true;
    }
    for (int i=0; i<root->num_children; i++) {
        if (root->children[i]->visited)
            continue;
        get_type_of_expression(symboltables_ptr, root->children[i]);
        if (error == 1) {
            root->visited = true;
            expression_type = TYPE_ERROR;
        }       
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

static void process_assignment_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    ASTNode* lvalueNode = root->children[0];
    // This must already be declared
    SymbolRecord* search = st_search_scope(symboltables_ptr, lvalueNode->token.lexeme, start_scope, end_scope);
    if (search == NULL) {
        fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' being assigned before declaration\n", lvalueNode->token.line_no, lvalueNode->token.lexeme);
        has_semantic_error = true;
        return;
    }

    if (root->children[1]->token_type == expression) {
        // lvalueIDStmt
        // However, ID can also be an array.
        // We must check the offsets and end indices too in that case
        ASTNode* exprNode = root->children[1];
        TypeName expr_type = get_expression_type(symboltables_ptr, exprNode);
        if (expr_type != TYPE_ERROR) {
            if (expr_type != search->type_name) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s'. Incompatible types for an lvalue assignment statement. lvalue is of type %s, while the assignment statement is of type %s\n", lvalueNode->token.line_no, lvalueNode->token.lexeme, get_string_from_type(search->type_name), get_string_from_type(expr_type));
                has_semantic_error = true;
                return;
            }
        }
        if (expr_type == TYPE_ARRAY) {
            int lvalue_offset = search->offset;
            int lvalue_end = search->end;
            // For Dynamic Arrays
            // For Static Arrays
            if ((TypeName)expression_array_params[0] != TYPE_NONE) {
                if (get_typename_from_term(search->element_type) != (TypeName)expression_array_params[0]) {
                    // Types of arrays don't match
                    fprintf(stderr, "Semantic Error (Line No: %d): Types do not match when assigning array '%s' to '%s'\n", lvalueNode->token.line_no, expression_array_name, lvalueNode->token.lexeme);
                    clear_expression_array();
                    has_semantic_error = true;
                    return;
                }
                if (is_dynamic == false) {
                    if (lvalue_offset != expression_array_params[1]) {
                        fprintf(stderr, "Semantic Error (Line No: %d): Types do not match when assigning array '%s' to '%s'\n", lvalueNode->token.line_no, expression_array_name, lvalueNode->token.lexeme);
                        clear_expression_array();
                        has_semantic_error = true;
                        return;
                    }
                    if (lvalue_end != expression_array_params[2]) {
                        fprintf(stderr, "Semantic Error (Line No: %d): Types do not match when assigning array '%s' to '%s'\n", lvalueNode->token.line_no, expression_array_name, lvalueNode->token.lexeme);
                        clear_expression_array();
                        has_semantic_error = true;
                        return;
                    }
                }
                else {
                    is_dynamic = false;
                }
            }
        }
    }
    else {
        // lvalueARRStmt
        assert (root->children[1]->token_type == lvalueARRStmt);
        ASTNode* lvalueARRStmt = root->children[1];
        ASTNode* idxNode = lvalueARRStmt->children[0];
        if (idxNode->token.token_type == TK_ID) {
            SymbolRecord* idx_search = st_search_scope(symboltables_ptr, idxNode->token.lexeme, start_scope, end_scope);
            if (idx_search == NULL) {
                fprintf(stderr, "Semantic Error (Line No: %d): Lvalue array index '%s' being assigned before declaration\n", idxNode->token.line_no, idxNode->token.lexeme);
                has_semantic_error = true;
                return;
            }
        }
        else if (idxNode->token.token_type == TK_NUM){
            // Integer
            // Check for bounds
            int value = atoi(idxNode->token.lexeme);
            if (value >= search->end || value < search->offset) {
                fprintf(stderr, "Semantic Error (Line No: %d): Array index for '%s' out of bounds\n", idxNode->token.line_no, search->token.lexeme);
                has_semantic_error = true;
                return;
            }
        }
        TypeName expr_type = get_expression_type(symboltables_ptr, lvalueARRStmt->children[1]);
        if (expr_type != TYPE_ERROR) {
            if (expr_type != get_typename_from_term(search->element_type)) {
                fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s'. Incompatible types for an lvalue assignment statement. lvalue is of type %s, while the assignment statement is of type %s\n", lvalueNode->token.line_no, lvalueNode->token.lexeme, get_string_from_type(get_typename_from_term(search->element_type)), get_string_from_type(expr_type));
                has_semantic_error = true;
            }
        }
        return;
    }
}

static void process_module_reuse(SymbolHashTable*** symboltables_ptr, ASTNode* root, char* moduleID) {
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
                    fprintf(stderr, "Semantic Error (Line No: %d): Identifier '%s' not found in scope, but used in module call statement\n", idNode->token.line_no, idNode->token.lexeme);
                    has_semantic_error = true;
                }
                // Ssearch must have the matched variable
                // Now check types of parameters
                // Result types must match <output_plist>
                // Maintain a Stack / List of all modules with their scopes.
                // This list is used to lookup parameter lists and verify types
                
                // Search the global scope for the moduleID
                search = st_search(symboltables[0], moduleID);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No: %d): Module ID '%s' not found in global scope. Cannot be called\n", idNode->token.line_no, moduleID);
                    has_semantic_error = true;
                }

                // TODO: Handle the <input_plist> and <output_plist> semantics
                // using the ft record structure
                int module_scope = search->scope_label;

                // Now search the module ft records for the identifier
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
    scope_stacks[module_index] = stack_push(scope_stacks[module_index], start_scope);
    create_scope_table(symboltables_ptr, start_scope);
    function_tables = realloc (function_tables, (start_scope + 1) * sizeof(FunctionTable));

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
        search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, start_scope);
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

static void process_conditional_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Start a new scope and create a new scope table
    // NOTE: I'm creating tables despite it being potentially empty. For now, I'm not placing
    // any conditional check to open or close scope tables wrt TK_END, although I may do so later
    start_scope ++;
    scope_stacks[module_index] = stack_push(scope_stacks[module_index], start_scope);
    create_scope_table(symboltables_ptr, start_scope);
    function_tables = realloc (function_tables, (start_scope + 1) * sizeof(FunctionTable));

    ASTNode* idNode = root->children[0];
    ASTNode* caseStmtsNode = root->children[1];
    ASTNode* defaultNode = root->children[2];

    // Identifier must already be visible in scope
    SymbolRecord* search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, start_scope);
    if (search == NULL) {
        fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' in SWITCH statement not declared in current scope\n", idNode->token.line_no, idNode->token.lexeme);
        has_semantic_error = true;
        return;
    }
    
    int next_jump = 2;
    for (ASTNode* curr = caseStmtsNode; ; curr = curr->children[next_jump]) {
        next_jump = curr->num_children - 1;
        if (next_jump < 0)
            break;
        if (curr->children[1]->token_type == statements) {
            // statements exists
            ASTNode* statementsNode = curr->children[1];
            TypeName expr_type = get_expression_type(symboltables_ptr, statementsNode->children[0]);
            if (expr_type == TYPE_INTEGER) {
                // Dummy Check
            }
        }
        if (curr->children[next_jump]->token_type != N9) {
            // N9 -> E
            break;
        }
    }
    if (defaultNode == NULL) {
        // defaultnode -> E
        return;
    }
    else {
        // defaultnode -> <statements> and <statements> is not Epsilon   
        TypeName expr_type = get_expression_type(symboltables_ptr, defaultNode->children[0]);
        if (expr_type == TYPE_INTEGER) { // Dummy Check
        }
    }
}

static void process_io_statement(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    if (root->syn_attribute.token_type == TK_PRINT) {
        // Print Statement
        if (root->children[0]->token_type == var_id_num) {
            ASTNode* idNode = root->children[0]->children[0];
            if (idNode->token_type == TK_ID) {
                SymbolRecord* search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, start_scope);
                if (search == NULL) {
                    fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' is not declared in scope\n", idNode->token.line_no, idNode->token.lexeme);
                    has_semantic_error = true;
                    return;
                }
                ASTNode* idxNode = root->children[0]->children[1];
                if (idxNode != NULL) {
                    // TK_NUM or TK_ID
                    if (idxNode->token.token_type == TK_NUM) {
                        if (search->offset > atoi(idxNode->token.lexeme) || search->end <= atoi(idxNode->token.lexeme)) {
                            fprintf(stderr, "Semantic Error (Line No %d): Array index for '%s' out of bounds\n", idNode->token.line_no, idNode->token.lexeme);
                            has_semantic_error = true;
                            return;

                        }
                    }
                    else {
                        // TK_ID
                        SymbolRecord* arr_search = st_search_scope(symboltables_ptr, idxNode->token.lexeme, start_scope, start_scope);
                        if (arr_search == NULL) {
                            fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' is not declared in scope\n", idxNode->token.line_no, idxNode->token.lexeme);
                            has_semantic_error = true;
                            return;
                        }
                    }
                }
            }
        }
    }
    else {
        assert(root->syn_attribute.token_type == TK_GET_VALUE);
        ASTNode* idNode = root->children[0];
        SymbolRecord* search = st_search_scope(symboltables_ptr, idNode->token.lexeme, start_scope, start_scope);
        if (search == NULL) {
            fprintf(stderr, "Semantic Error (Line No %d): Identifier '%s' is not declared\n", idNode->token.line_no, idNode->token.lexeme);
            has_semantic_error = true;
            return;
        }
    }
}

static void initialize_stacks(int max_modules, int max_nesting_level) {
    modules = calloc (max_modules, sizeof(int));
    scope_stacks = calloc (max_modules, sizeof(Stack*));
    for (int i=0; i<max_modules; i++) {
        scope_stacks[i] = init_stack(0, max_nesting_level);
    }
}

static void free_stacks(int max_modules) {
    for (int i=0; i<max_modules; i++) {
        if (scope_stacks[i]) free_stacknode(scope_stacks[i]);
    }
    if (scope_stacks) free(scope_stacks);
    if (modules) free(modules);
}

void semantic_analyzer_wrapper(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Wrapper function for the semantic analyzer
    int max_modules = 30; int max_nesting_level = 10;
    initialize_stacks(max_modules, max_nesting_level);
    function_tables = calloc (1, sizeof(FunctionTable));
    perform_semantic_analysis(symboltables_ptr, root);
    print_function_tables(start_scope + 1);
    free_function_table(start_scope + 1);
    free_stacks(max_modules);
}

void perform_semantic_analysis(SymbolHashTable*** symboltables_ptr, ASTNode* root) {
    // Performs all required Semantic Checks using the Symbol Table
    if (!root)
        return;
    SymbolHashTable** symboltables = *symboltables_ptr; // Careful with realloc()
    if (root->token_type == TK_END) {
        if (root->parent->token_type == condionalStmt || root->parent->token_type == iterativeStmt) {
            // End of Scope. Pop off from the module stack
            //printf("Popping from module num: %d, scope_num = %d\n", module_index, scope_stacks[module_index]->data);
            if (tmp_scope == -1) {
                tmp_scope = start_scope;
            }
            scope_stacks[module_index] = stack_pop(scope_stacks[module_index]);
            //print_stack(scope_stacks[module_index]);
            start_scope --;
        }
        else if (root->parent->token_type == moduleDef) {
            if (tmp_scope != -1) {
                start_scope = tmp_scope;
                tmp_scope = -1;
            }
        }
        // End of Scope
        end_scope ++;
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
        char* moduleID = root->children[1]->token.lexeme;
        process_module_reuse(symboltables_ptr, root, moduleID);
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
    else if (root->token_type == condionalStmt) {
        process_conditional_statement(symboltables_ptr, root);
    }
    else if (root->token_type == ioStmt) {
        process_io_statement(symboltables_ptr, root);
    }
    //else if (root->token_type == arithmeticExpr) {
    //    process_expression(symboltables_ptr, root);
    //}
    for (int i=0; i<root->num_children; i++)
        perform_semantic_analysis(symboltables_ptr, root->children[i]);
}

SymbolHashTable*** createSymbolTables(ASTNode* root) {
    // Creates the Symbol Tables; one for every scope
    SymbolHashTable*** symboltables_ptr = calloc (1, sizeof(SymbolHashTable**));
    symboltables_ptr[0] = calloc (1, sizeof(SymbolHashTable*));
    symboltables_ptr[0][0] = create_symtable(CAPACITY, hash_function_symbol);
    ASTNode* temp = root;
    semantic_analyzer_wrapper(symboltables_ptr, temp);
    total_scope = start_scope;
    return symboltables_ptr;
}

SymbolRecord* create_symbolrecord(Token token, TypeName type_name, int scope_label, int end, int offset, term element_type, char* offset_id, char* end_id) {
    SymbolRecord* symbolrecord = (SymbolRecord*) calloc (1, sizeof(SymbolRecord));
    symbolrecord->token = token;
    symbolrecord->type_name = type_name;
    symbolrecord->scope_label = scope_label;
    symbolrecord->end = end;
    symbolrecord->offset = offset;
    symbolrecord->element_type = element_type;
    symbolrecord->offset_id = offset_id;
    symbolrecord->end_id = end_id;
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
    printf("end = %d , ", t->end);
    printf("offset = %d , ", t->offset);
    printf("element_type = %s , ", get_string_from_term(t->element_type));
    if (t->offset_id) printf("offset_id = %s , ", t->offset_id);
    if (t->end_id) printf("end_id = %s , ", t->end_id);
    printf("%c", ch);
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
