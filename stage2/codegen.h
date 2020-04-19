#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "ast.h"
#include "symbol_table.h"

#define SIZE_INTEGER 2
#define SIZE_BOOLEAN 1
#define SIZE_REAL 4

void code_init(FILE* fp);
void generate_code(ASTNode* node, FILE* fp);

#endif
