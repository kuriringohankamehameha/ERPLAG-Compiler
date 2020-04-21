#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"
#include "function_table.h"
#include <assert.h>

FILE*fp;
int uno = 0;

void open_files() {
    fp = fopen("final.txt", "w");
}

void cleanup() {
    fclose(fp);
}

void code_init() {
    fprintf(fp, "SECTION .data\n");
    fprintf(fp, "extern printf\nextern scanf\n");
    fprintf(fp, "section .text\nglobal main\n");
}

void code_end() {
    fprintf(fp, "main:\nsub rsp,8\ncall driver\npop rbp\nadd rsp,8\nret\n");
    // Transfer to final
}

void io_handler(SymbolHashTable*** symboltables_ptr, ASTNode* ioStmtNode, int scope, int runstack_ptr) {
    if (ioStmtNode->syn_attribute.token_type == TK_PRINT) {
        // Print
        if (ioStmtNode->num_children >= 1) {
            if (ioStmtNode->children[0]->token_type == var_id_num) {
                if (ioStmtNode->num_children >= 1) {
                    ASTNode* idNode = ioStmtNode->children[0]->children[0];
                    if (idNode->token_type == TK_ID) {
                        if (idNode->parent->children[1]) {
                            // Array Index
                            char* name = idNode->token.lexeme;
                            fprintf(stderr, "SEGFAULT 1: Probably segfaults here\n");
                            char* index = idNode->parent->children[1]->children[1]->children[1]->token.lexeme;
                            fprintf(stderr, "SEGFAULT 1: Got through this time\n");
                        }
                    }
                    else if (idNode->token_type == TK_NUM || idNode->token_type == TK_RNUM) {
                        char* value = idNode->token.lexeme;
                        uno ++;
                        fprintf(fp, "h%d:\tdb \'Output:%s\', 10, 0\n", uno, value);
                        uno ++;
                        fprintf(fp, "h%d:\tequ $-h%d\n", uno, uno-1);
                        fprintf(fp, "mov eax, 4\n");
                        fprintf(fp, "mov ebx, 1\n");
                        fprintf(fp, "mov ecx, h%d\n", uno-2);
                        fprintf(fp, "mov edx, h%d\n", uno-1);
                        fprintf(fp, "int 80h\n");
                    }
                }
                else {
                    // boolConstt
                    fprintf(stderr, "Handling boolConstt\n");
                    char* value = ioStmtNode->children[0]->token.lexeme;
                    uno++;
                    fprintf(fp, "h%d:\t\tdb \'Output:%s\', 10, 0\n", uno, value);
                    uno++;
                    fprintf(fp, "mov eax, 4\n");
                    fprintf(fp, "mov ebx, 1\n");
                    fprintf(fp, "mov ecx, h%d\n", uno-2);
                    fprintf(fp, "mov edx, h%d\n", uno-1);
                    fprintf(fp, "int 80h\n");
                }
            }
        }
    }
    else {
        // Scan
        fprintf(stderr, "Praceholder for scanf\n");
    }
}

void expression_handler(SymbolHashTable*** symboltables_ptr, ASTNode* expNode, int scope, int runstack_ptr) {
    if (expNode->children[0]->token_type == arithmeticOrBooleanExpr) {
        aob_handler(symboltables_ptr, expNode->children[0], scope, runstack_ptr);
    }
    else if (expNode->children[0]->token_type == U) {
        unary_handler(symboltables_ptr, expNode->children[0], scope, runstack_ptr);
    }
}

void varidnum_handler(SymbolHashTable*** symboltables_ptr, ASTNode* varnode, int scope, int runstack_ptr) {
    ASTNode* varchild = varnode->children[0];
    if (varchild->token_type == TK_ID) {
        if (varchild->parent->num_children >= 2) {
            // Array
        }
        else {
            char* value = varchild->token.lexeme;
            // Use var to get record
            SymbolRecord* record = st_search_scope(symboltables_ptr, value, scope, scope);
            int address = record->addr;
            fprintf(fp, "mov eax, [%d]\n", address);
        }
    }
    else if (varchild->token_type == TK_NUM) {
        char* value = varchild->token.lexeme;
        int val = atoi(value);
        fprintf(fp, "mov eax, dword [%d]\n", val);
    }
    else if (varchild->token_type == TK_RNUM) {
        fprintf(stderr, "TK_RNUM\n");
    }
}

void factor_handler(SymbolHashTable*** symboltables_ptr, ASTNode* factnode, int scope, int runstack_ptr) {
    ASTNode* factchild = factnode->children[0];
    if (factchild->token_type == var_id_num) {
        varidnum_handler(symboltables_ptr, factchild, scope, runstack_ptr);
    }
    else if (factchild->token_type == arithmeticOrBooleanExpr) {
        aob_handler(symboltables_ptr, factchild, scope, runstack_ptr);
    }
}

void term_handler(SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope, int runstack_ptr) {
    factor_handler(symboltables_ptr, node->children[0], scope, runstack_ptr);
    if (node->num_children < 2) return;
    fprintf(fp, "mov ebx, eax\n");
    term_handler(symboltables_ptr, node->children[1], scope, runstack_ptr);
    if (node->children[1]->syn_attribute.token_type == TK_MUL) {
        fprintf(stderr, "TK_MUL Syn Attribute\n");
        fprintf(fp, "imul ebx\n");
    }
    else {
        // Divide eax and ebx
        fprintf(fp, "mov ecx, eax\n");
        fprintf(fp, "mov eax, ebx\n");
        fprintf(fp, "cdq\n");
        fprintf(fp, "idiv ecx\n");
    }
}

void arithmetic_handler(SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope, int runstack_ptr) {
    term_handler(symboltables_ptr, node->children[0], scope, runstack_ptr);
    if (node->num_children < 2) return;
    fprintf(fp, "mov\t ebx, eax\n");
    arithmetic_handler(symboltables_ptr, node->children[1], scope, runstack_ptr);
    if (node->children[1]->syn_attribute.token_type == TK_PLUS) {
        fprintf(fp, "add eax, ebx\n");
    }
    else {
        assert (node->children[1]->syn_attribute.token_type == TK_MINUS);
        fprintf(fp, "sub ebx, eax\n");
        fprintf(fp, "mov eax, ebx\n");
    }
}

void anyterm_handler(SymbolHashTable*** symboltables_ptr, ASTNode* anynode, int scope, int runstack_ptr){
    if (anynode->children[0]->token_type == arithmeticExpr) {
        if (anynode->num_children < 2) arithmetic_handler(symboltables_ptr, anynode->children[0], scope, runstack_ptr);
        else {
            // Relational Op
        }
    }
    else {
        if (anynode->num_children < 2) {
            if (anynode->children[0]->token.token_type == TK_TRUE) {
                fprintf(fp, "mov eax, 1\n");
            }
            else {
                fprintf(fp, "mov eax, 0\n");
            }
        }
    }
}

void aob_handler(SymbolHashTable*** symboltables_ptr, ASTNode* aobnode, int scope, int runstack_ptr) {
    anyterm_handler(symboltables_ptr, aobnode->children[0], scope, runstack_ptr);
    if (aobnode->num_children < 2) return;
    // Logical Operator
    fprintf(fp, "mov ecx, eax\n");
    aob_handler(symboltables_ptr, aobnode->children[1], scope, runstack_ptr);
    if (aobnode->children[1]->syn_attribute.token_type == TK_AND) {
        fprintf(fp, "and eax, ecx\n");
    }
    else if (aobnode->children[1]->syn_attribute.token_type == TK_OR) {
        fprintf(fp, "or eax, ecx\n");
    }
}

void unary_handler(SymbolHashTable*** symboltables_ptr, ASTNode* unarynode, int scope, int runstack_ptr) {
    if (unarynode->num_children < 1) return;
    if (unarynode->children[0]->token_type == arithmeticExpr) {
        arithmetic_handler(symboltables_ptr, unarynode->children[0],scope, runstack_ptr);
    }
    else if (unarynode->children[0]->token_type == var_id_num) {
        varidnum_handler(symboltables_ptr, unarynode->children[0], scope, runstack_ptr);
    }
    if (unarynode->syn_attribute.token_type == TK_PLUS) {
        fprintf(fp, "mov ecx, 1\n");
    }
    else {
        fprintf(fp, "mov ecx, -1\n");
    }
    fprintf(fp, "imul ecx\n");
}

void assign_handler(SymbolHashTable*** symboltables_ptr, ASTNode* assignmentStmtNode, int scope, int runstack_ptr) {
    fprintf(stderr, "SEGFAULT 2: Could segfault here\n");
    ASTNode* idNode = assignmentStmtNode->children[0];
    ASTNode* lvalueStmt = assignmentStmtNode->children[1];
    fprintf(stderr, "SEGFAULT 2: Pass\n");
    if (lvalueStmt->token_type == lvalueARRStmt) {
        // Array
    }
    else {
        // ID
        char* name = idNode->token.lexeme;
        SymbolRecord* search = st_search_scope(symboltables_ptr, name, scope, scope);
        if (search->type_name == TYPE_INTEGER) {
            expression_handler(symboltables_ptr, lvalueStmt->children[0], scope, runstack_ptr);
            fprintf(fp, "mov rbx, %d\n", search->addr);
            fprintf(fp, "mov [rbx], rax\n");
        }
    }
}

void stmt_handler(SymbolHashTable*** symboltables_ptr, ASTNode* stmtNode, int scope, int runstack_ptr) {
    if (stmtNode->num_children >= 1) {
        if (stmtNode->children[0]->token_type == ioStmt) {
            io_handler(stmtNode->children[0], scope, runstack_ptr);
        }
        else if (stmtNode->children[0]->token_type == assignmentStmt) {
            assign_handler(symboltables_ptr, stmtNode->children[0], scope, runstack_ptr);
        }
    }
}

void statements_handler(SymbolHashTable*** symboltables_ptr, ASTNode* statementsNode, int scope, int runstack_ptr) {
    if (statementsNode == NULL)
        return;
    stmt_handler(symboltables_ptr, statementsNode->children[0], scope, runstack_ptr);
    if (statementsNode->num_children >= 2)
        statements_handler(symboltables_ptr, statementsNode->children[1], scope, runstack_ptr);
}

void generate_code(ASTNode* node, FunctionTable** function_tables, SymbolHashTable*** symboltables_ptr) {
    // Assign some address to the stack pointer
    SymbolHashTable** symboltables = *(symboltables_ptr);
    int size = 0;
    int runstack_ptr = 10000;
    if (node->token_type == driverModule) {
        fprintf(fp, "driver:\nsub rsp,8\n");
        ASTNode* statementsNode = node->children[0]->children[1];
        SymbolRecord* record = st_search(symboltables[0], "driver");
        assert (record != NULL);
        int scope_no = record->scope_label;
        FunctionTable* table = function_tables[scope_no];
        for (int i=0; i<table->num_input_params; i++) {
            SymbolRecord* record = st_search(symboltables[scope_no], table->input_plist[i]);
            if (record->type_name == TYPE_INTEGER) {
                printf("SUB RSP, %d\n", SIZE_INTEGER);
                runstack_ptr -= SIZE_INTEGER;
                size += SIZE_INTEGER;
            }
            else if (record->type_name == TYPE_REAL) {
                printf("SUB RSP, %d\n", SIZE_REAL);
                runstack_ptr -= SIZE_REAL;
                size += SIZE_REAL;
            }
            else if (record->type_name == TYPE_BOOLEAN) {
                printf("SUB RSP, %d\n", SIZE_BOOLEAN);
                runstack_ptr -= SIZE_BOOLEAN;
                size += SIZE_BOOLEAN;
            }
            else if (record->type_name == TYPE_ARRAY) {
                if (record->offset_id == NULL) {
                    // Static Array
                    TypeName arr_type = get_typename_from_term(record->element_type);
                    if (arr_type == TYPE_INTEGER) {
                        printf("SUB RSP, %d\n", SIZE_INTEGER * (record->end - record->offset));
                        runstack_ptr -= SIZE_INTEGER * (record->end - record->offset);
                        size += SIZE_INTEGER * (record->end - record->offset);
                    }
                    else if (arr_type == TYPE_REAL) {
                        printf("SUB RSP, %d\n", SIZE_REAL * (record->end - record->offset));
                        runstack_ptr -= SIZE_REAL * (record->end - record->offset);
                        size += SIZE_REAL * (record->end - record->offset);
                    }
                    else if (arr_type == TYPE_BOOLEAN) {
                        printf("SUB RSP, %d\n", SIZE_BOOLEAN * (record->end - record->offset));
                        runstack_ptr -= SIZE_BOOLEAN * (record->end - record->offset);
                        size += SIZE_BOOLEAN * (record->end - record->offset);
                    }
                }
                else {
                    // Dynamic Array
                }
            }
            record->addr = runstack_ptr;
        }
        statements_handler(symboltables_ptr, statementsNode, scope_no, runstack_ptr);
        fprintf(fp, "ADD RSP,8\nret\n");
    }
}

void perform_codegen(FILE* fp, ASTNode* root) {
    code_init();
    generate_code(fp, root);
    code_end();
}
