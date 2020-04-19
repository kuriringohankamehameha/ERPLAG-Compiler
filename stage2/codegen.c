#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"
#include "function_table.h"
#include <assert.h>

extern SymbolHashTable** symboltables;
extern FunctionTable* function_tables;

void code_init(FILE* fp);

void generate_code(ASTNode* node, FILE* fp) {
    // Assign some address to the stack pointer
    int runstack_ptr = 10000;
    if (node->token_type == driverModule) {
        printf("MOV RSP, %d\n", runstack_ptr);
        ASTNode* statementsNode = node->children[0]->children[1];
        if (statementsNode->token_type == statements) {
            // statements is not Epsilon
            SymbolRecord* record = st_search(symboltables[0], "driver");
            assert (record != NULL);
            int scope_no = record->scope_label;
            FunctionTable table = function_tables[scope_no];
            for (int i=0; i<table.num_input_params; i++) {
                SymbolRecord* record = st_search(symboltables[scope_no], table.input_plist[i]);
                if (record->type_name == TYPE_INTEGER) {
                    printf("SUB RSP, %d\n", SIZE_INTEGER);
                    runstack_ptr -= SIZE_INTEGER;
                }
                else if (record->type_name == TYPE_REAL) {
                    printf("SUB RSP, %d\n", SIZE_REAL);
                    runstack_ptr -= SIZE_REAL;
                }
                else if (record->type_name == TYPE_BOOLEAN) {
                    printf("SUB RSP, %d\n", SIZE_BOOLEAN);
                    runstack_ptr -= SIZE_BOOLEAN;
                }
                else if (record->type_name == TYPE_ARRAY) {
                    if (record->offset_id == NULL) {
                        // Static Array
                        TypeName arr_type = get_typename_from_term(record->element_type);
                        if (arr_type == TYPE_INTEGER) {
                            printf("SUB RSP, %d\n", SIZE_INTEGER * (record->end - record->offset));
                            runstack_ptr -= SIZE_INTEGER * (record->end - record->offset);
                        }
                        else if (arr_type == TYPE_REAL) {
                            printf("SUB RSP, %d\n", SIZE_REAL * (record->end - record->offset));
                            runstack_ptr -= SIZE_REAL * (record->end - record->offset);
                        }
                        else if (arr_type == TYPE_BOOLEAN) {
                            printf("SUB RSP, %d\n", SIZE_BOOLEAN * (record->end - record->offset));
                            runstack_ptr -= SIZE_BOOLEAN * (record->end - record->offset);
                        }
                        record->addr = runstack_ptr;
                    }
                }
            }
        }
        else {
            // Do Nothing
        }
    }
}
