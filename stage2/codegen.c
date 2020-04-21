#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "codegen.h"
#include "function_table.h"
#include <assert.h>


FILE* fp;
int uno=0;

void code_init() {
  fprintf(fp,"SECTION .bss\n");
  fprintf(fp, "base: resb 10000\n");

  fprintf(fp,"SECTION .data\n");
  //--
  fprintf(fp,"fmt: db \"Output:%%d\", 10, 0\n");
  fprintf(fp,"intFormat:     db        \"%%d \", 0\n");
  fprintf(fp,"middle_msg:     db        \"array elements of integer type for range %%d \", 0\n");
  fprintf(fp,"to_msg:     db        \" to %%d \",10, 0\n");
  fprintf(fp, "int_inMsg:    db        \"Enter an integer value\" , 10, 0\n");
  fprintf(fp, "true_msg:    db        \"TRUE\" , 10, 0\n");
  fprintf(fp, "false_msg:    db        \"FALSE\" , 10, 0\n");
  fprintf(fp, "output_msg:    db        \"Output: \" , 0\n");
  fprintf(fp, "enter_msg:    db        \"Enter \" , 0\n");
  fprintf(fp, "newline_msg:  db \"\", 10, 0\n");
  fprintf(fp,"extern printf\nextern scanf\n");
  fprintf(fp,"section .text\nglobal main\n");
}

void code_end(){
  fprintf(fp,"main:\nsub rsp,8\ncall driver\nadd rsp,8\nret\n");
        // transfer to final
}

void codegen(ASTNode* node, FunctionTable** function_tables, SymbolHashTable*** symboltables_ptr) {
        SymbolHashTable** symboltables = *(symboltables_ptr);
        if(node->token_type == driverModule || node->token_type == module) {
                int addof = 0;
        fprintf(fp,"driver:\nsub rsp,8\n");

        // 0 corressponds to TK_DRIVER
        ASTNode* statementsNode = node->children[1]->children[1];
        SymbolRecord* record = st_search(symboltables[0], "driver");
        int scope_no = record->scope_label;
        printf("Searching for %s at scope %d\n", record->token.lexeme, record->scope_label);

        // Fix this ASAP
        // Must be scope_no, but it gives segfault
        FunctionTable* table = function_tables[scope_no];

        printf("num_params = %d\n", table->num_input_params);

        for(int i=0; i<table->num_input_params; i++) {
                SymbolRecord* record = st_search(symboltables[scope_no], table->input_plist[i]);
                record->addr = addof;
          if(record->type_name == TYPE_INTEGER) {
              addof += SIZE_INTEGER;
            }
        }

                for(int i=0; i<table->num_variables; i++) {
                SymbolRecord* record = st_search(symboltables[scope_no], table->variables[i]);
                record->addr = addof;
          if(record->type_name == TYPE_INTEGER) {
              addof += SIZE_INTEGER;
            }
        }

        for(int i=0; i<table->num_output_params; i++) {
                SymbolRecord* record = st_search(symboltables[scope_no], table->output_plist[i]);
                record->addr = addof;
          if(record->type_name == TYPE_INTEGER) {
              addof += SIZE_INTEGER;
            }
        }

        printf("statements\n");
        statements_handler(symboltables_ptr, statementsNode, scope_no);
        fprintf(fp,"add rsp,8\nret\n");
        printf("no else\n");
    }
        else  {
        int n = node->num_children;
        printf("n = %d\n", n);
        for(int i=0; i<n ;i++) {
                codegen(node->children[i], function_tables, symboltables_ptr);
      }
    }

}

void write_code(ASTNode* root, FunctionTable** function_tables, SymbolHashTable*** symboltables_ptr) {
    code_init();
    printf("Writing coddde\n");
    codegen(root, function_tables, symboltables_ptr);
    code_end();
}


void statements_handler(SymbolHashTable*** symboltables_ptr, ASTNode* statementsNode, int scope) {
    printf("inside\n");
  if (statementsNode == NULL) return;

  printf("GOing to stmt handler\n");
  stmt_handler(symboltables_ptr, statementsNode->children[0], scope);
  statements_handler(symboltables_ptr, statementsNode->children[1], scope);

}


void stmt_handler (SymbolHashTable*** symboltables_ptr, ASTNode* stmtNode, int scope) {
    printf("INside stmt handler\n");
        if (stmtNode->token_type == ioStmt) {
        printf("io\n");
         io_handler(symboltables_ptr, stmtNode,scope);
    }

        else if (stmtNode->token_type == assignmentStmt) {
        printf("assign\n");
        assign_handler (symboltables_ptr, stmtNode, scope);
    }
    printf("??\n");
}


void io_handler(SymbolHashTable*** symboltables_ptr, ASTNode* ioStmtNode , int scope) {
        if(ioStmtNode->syn_attribute.token_type == TK_PRINT) {
          // Print
          if(ioStmtNode->children[0]->token_type == var_id_num) {
                  ASTNode* idNode = ioStmtNode->children[0]->children[0];
                  if(idNode->token_type == TK_ID) {
                  if (idNode->parent->num_children == 1) {
                    // id  can be array
                      SymbolRecord* record = st_search_scope(symboltables_ptr, idNode->token.lexeme, scope, scope);

                    if(record->type_name == TYPE_INTEGER) {
                        fprintf(fp,"mov eax, [base+%d]\n", record->addr);
                        fprintf(fp,"mov rdi, fmt\n");
                        fprintf(fp,"mov rsi, [base+%d]\n", record->addr);
                        fprintf(fp,"call printf\n");
                    }
                    else if(record->type_name == TYPE_BOOLEAN){
                        uno++;
                        fprintf(fp,"mov eax, [base+%d]\n", record->addr);
                        fprintf(fp,"cmp eax,0\n");
                        fprintf(fp,"JE X%d\n",uno);

                        // print true
                        fprintf(fp,"lea rdi, [true_msg]\n");
                        fprintf(fp, "xor rax, rax\n");
                        fprintf(fp,"call printf\n");

                        fprintf(fp,"jmp E%d\n",uno);
                        fprintf(fp,"X%d:\n",uno);

                        //print false
                    fprintf(fp,"lea rdi, [false_msg]\n");
                        fprintf(fp, "xor rax, rax\n");
                        fprintf(fp,"call printf\n");

                        fprintf(fp,"E%d:\n",uno);
                    }
                    else if(record->type_name == TYPE_ARRAY) {
                        printf("Array\n");
                      if (get_typename_from_term(record->element_type) == TYPE_INTEGER) {
                        // Integer Array
                        fprintf(fp,"lea rdi, [output_msg]\n");
                        fprintf(fp, "xor rax, rax\n");
                        fprintf(fp,"call printf\n");
                        
                        int size;
                        int offset=0;
                        int end=0;
                        
                        if (record->offset_id != NULL) {
                          
                        }
                        else {
                          offset = record->offset;
                          end = record->end;
                        }
                        
                        size = end - offset  + 1;
        				for(int j=0; j<size; j++) {
                        fprintf(fp,"lea rdi, [intFormat]\n"); 
        				fprintf(fp,"mov esi, [base+%d]\n",record->addr+SIZE_INTEGER*j);
                        fprintf(fp, "xor rax, rax\n");
                        fprintf(fp,"call printf\n");
                        } 
                        // printing line break
                        fprintf(fp,"lea rdi, [newline_msg]\n");
                        fprintf(fp, "xor rax, rax\n");
                        fprintf(fp,"call printf\n");
                        
                      }
                    }
                    
                  }
                  else {
                                //Array
                  }
              }
                //
            else if (idNode->token_type == TK_NUM || idNode->token_type == TK_RNUM) {
                printf("TK_NUM\n");
              /*        char* value = idNode->token.lexeme; //
                uno++;
                                fprintf(fdata,"h%d:     db \'Output:%s\',10,0\n", uno,value);
                uno++;
                fprintf(fdata,"h%d:  equ $-h%d\n",uno,uno-1);

                fprintf(fp,"mov eax,4\n");            // The system call for write (sys_write)
                                fprintf(fp,"mov ebx,1\n");            // File descriptor 1 - standard output
                                fprintf(fp,"mov ecx,h%d\n", uno-2);        // Put the offset of hello in ecx
                                fprintf(fp,"mov edx,h%d\n", uno-1);
                fprintf(fp,"int 80h\n");
               */
             }
			
			
          }
                //
          else {
              printf("boolconstt\n");

            // boolConstt
            //char* value = ioStmtNode->children[0]->token.lexeme;
            /*
            uno++;
                        fprintf(fdata,"h%d:     db \'Output:%s\',10,0\n", uno,value);
            uno++;
            fprintf(fdata,"h%d:  equ $-h%d\n",uno,uno-1);

            fprintf(fp,"mov eax,4\n");            // The system call for write (sys_write)
                        fprintf(fp,"mov ebx,1\n");            // File descriptor 1 - standard output
                        fprintf(fp,"mov ecx,h%d\n", uno-2);        // Put the offset of hello in ecx
                        fprintf(fp,"mov edx,h%d\n", uno-1);
            fprintf(fp,"int 80h\n");
            */
          }
        }
        else {
          char* vname =  ioStmtNode->children[0]->token.lexeme;
          SymbolRecord* record = st_search_scope(symboltables_ptr, vname, scope, scope);
          if(record->type_name == TYPE_INTEGER) {
            fprintf(fp,"lea rdi, [int_inMsg]\n");
            fprintf(fp, "xor rax, rax\n");
            fprintf(fp,"call printf\n");

            fprintf(fp,"lea rdi, [intFormat]\n");
                fprintf(fp,"lea rsi, [base+%d]\n", record->addr);
            fprintf(fp, "xor rax, rax\n");
            fprintf(fp,"call scanf\n");
        	}
          else if(record->type_name == TYPE_ARRAY) {
            if (get_typename_from_term(record->element_type) == TYPE_INTEGER) {
              //Input: Enter 6 array elements of integer type for range 10 to 15
            
            	fprintf(fp,"lea rdi, [enter_msg]\n");
            	fprintf(fp, "xor rax, rax\n");
            	fprintf(fp,"call printf\n");
             
            	int size;
            	int offset=0;
            	int end=0;
                        
                if (record->offset_id != NULL) {
                  
                }
                else {
                	offset = record->offset;
                    end = record->end;
                }
                        
                size = end - offset  + 1;
              
          		fprintf(fp,"lea rdi, [intFormat]\n"); 
        		fprintf(fp,"mov esi, %d\n",size);
            	fprintf(fp, "xor rax, rax\n");
            	fprintf(fp,"call printf\n");
              
				
              	fprintf(fp,"lea rdi, [middle_msg]\n");
              	fprintf(fp,"mov esi, %d\n",offset);
            	fprintf(fp, "xor rax, rax\n");
            	fprintf(fp,"call printf\n");
              
              	fprintf(fp,"lea rdi, [to_msg]\n");
              	fprintf(fp,"mov esi, %d\n",end);
            	fprintf(fp, "xor rax, rax\n");
            	fprintf(fp,"call printf\n");
              
              for(int j=0; j<size; j++) {
                fprintf(fp,"lea rdi, [intFormat]\n");
                fprintf(fp,"lea rsi, [base+%d]\n", record->addr +j*SIZE_INTEGER);
            	fprintf(fp, "xor rax, rax\n");
            	fprintf(fp,"call scanf\n");
              }
              
            }
          }
      	}
}


void assign_handler (SymbolHashTable*** symboltables_ptr, ASTNode* assignmentStmtNode, int scope) {
    printf("assign handler\n");
        ASTNode* idNode = assignmentStmtNode->children[0];
    ASTNode* lvalueStmt = assignmentStmtNode->children[1];
        if (lvalueStmt->token_type == lvalueARRStmt) {
          // Array
        }
        else {
          // ID
          // id can be array
          printf("ID\n");
          char*  name = idNode->token.lexeme;
          SymbolRecord* search = st_search_scope(symboltables_ptr, name, scope, scope);
                        // should work for boolean type too;
          if(search->type_name == TYPE_INTEGER || search->type_name == TYPE_BOOLEAN) {

            printf("Going to handling expression\n");
            expression_handler(symboltables_ptr, lvalueStmt->children[0],scope);
            printf("Handled expression\n");
            //WARNING-------------------
            fprintf(fp,"mov [base+%d], eax\n",search->addr);
          }
          //if(search->type_name == TYPE_)
                //boolean;

      }
}


void expression_handler(SymbolHashTable*** symboltables_ptr, ASTNode* expNode, int scope) {
    printf("Ecpression type: %s\n", get_string_from_term(expNode->token_type));
        if (expNode->token_type == arithmeticOrBooleanExpr){
        printf("aob handler\n");
      aob_handler(symboltables_ptr, expNode,scope);
    }
  else if(expNode->token_type == U){
      printf("unary handler\n");
     unary_handler(symboltables_ptr,expNode,scope);
  }
}


void factor_handler(SymbolHashTable*** symboltables_ptr, ASTNode* factnode, int scope){
        ASTNode* factchild = factnode->children[0];
        if(factchild->token_type == var_id_num){
        printf("factchild\n");
      varidnum_handler(symboltables_ptr, factchild, scope);
    }
  else if (factchild->token_type == arithmeticOrBooleanExpr){
      printf("arithmeticOrBooleanExpr\n");
    aob_handler(symboltables_ptr, factchild, scope);
  }
}

void varidnum_handler(SymbolHashTable*** symboltables_ptr, ASTNode* varnode, int scope) {
    printf("varidnum_handler\n");
  ASTNode* varchild = varnode->children[0];
  if(varchild->token_type == TK_ID) {
     int arr_index =0; 
    if (varchild->parent->children[1]) {
          index_handler(symboltables_ptr, varnode->children[1], scope);      
      }
      else{
				fprintf(fp,"MOV EBX,0\n");
          }
                char* value = varchild->token.lexeme;
                // use var to get record
                SymbolRecord* record = st_search_scope(symboltables_ptr, value, scope, scope);
                int address = record->addr;
    			fprintf(fp,"LEA EDX, [base + %d]\n", address);
                fprintf(fp,"ADD EDX ,EBX\n");
                fprintf(fp,"MOV EAX, [EDX]\n", address);
  }
  else if (varchild->token_type == TK_NUM) {
  //NUM
        char* value = varchild->token.lexeme;
        int val = atoi(value);
        fprintf(fp,"MOV EAX, %d\n", val);
  }
  else if( varchild->token_type == TK_RNUM){/**/}


}


void index_handler(SymbolHashTable*** symboltables_ptr, ASTNode* indexnode, int scope){
        char* value = indexnode->token.lexeme;
	  if (indexnode->token_type == TK_NUM) {
        	int num = atoi(value);
        	fprintf(fp,"MOV EBX, %d\n", num*SIZE_INTEGER);
        
      }
  else if(indexnode->token_type == TK_ID) {
    	SymbolRecord* record = st_search_scope(symboltables_ptr, value, scope, scope);
        int address = record->addr;
    	fprintf(fp,"MOV EAX,[base + %d]\n",record->addr);
    	fprintf(fp,"MOV ECX, %d\n", SIZE_INTEGER);
    	fprintf(fp,"IMUL ECX\n");
    	fprintf(fp,"MOV EBX,EAX");
  }
}

void term_handler (SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope) {
    printf("term handler\n");
  factor_handler(symboltables_ptr, node->children[0],scope);

  if(node->children[1] == NULL)
    return;
  //fprintf(fp,"MOV     EBX, EAX\n");
  fprintf(fp, "push rax\n");
  term_handler(symboltables_ptr, node->children[1],scope);
  fprintf(fp, "pop rbx\n");

  if(node->children[1]->syn_attribute.token_type == TK_MUL) {
        //multiply eax and ebx;
   fprintf(fp,"IMUL     EBX\n");
   //possibility of overflow but i dont care
   // value is at EDX :EAX

  }
  else {
        //divide eax and ebx;
    fprintf(fp,"MOV     ECX, EAX\n");
    fprintf(fp,"MOV     EAX, EBX\n");
        fprintf(fp,"CDQ\n");
        fprintf(fp,"IDIV ECX\n");
  }
}

void arithmetic_handler(SymbolHashTable*** symboltables_ptr, ASTNode* node, int scope) {
  term_handler(symboltables_ptr, node->children[0], scope);
  if(node->children[1] == NULL)
        return;
  // fprintf(fp,"MOV    EBX, EAX\n");
  fprintf(fp, "push rax\n");
  arithmetic_handler(symboltables_ptr, node->children[1],scope);
  fprintf(fp, "pop rbx\n");

  if(node->children[1]->syn_attribute.token_type == TK_PLUS) {
        fprintf(fp,"ADD EAX,EBX\n");
  }
  else {
        fprintf(fp,"SUB EBX,EAX\n");
    fprintf(fp,"MOV     EAX, EBX\n");
  }
}

void anyterm_handler(SymbolHashTable*** symboltables_ptr, ASTNode* anynode, int scope){
  if(anynode->children[0]->token_type == arithmeticExpr ) {
    if(anynode->children[1] == NULL) {
      arithmetic_handler(symboltables_ptr, anynode->children[0], scope);
    }
    else{
      // relational op
      arithmetic_handler(symboltables_ptr, anynode->children[0], scope);
      fprintf(fp,"PUSH RAX\n");

      term temp = anynode->children[1]->syn_attribute.token_type;

      anyterm_handler(symboltables_ptr, anynode->children[0], scope);

      fprintf(fp,"POP RBX\n");
      fprintf(fp,"CMP EBX,EAX\n");
      uno++;
      char cstring1[5] = "JL";
      char cstring2[5] = "JLE";
      char cstring3[5] = "JG";
      char cstring4[5] = "JGE";
      char cstring5[5] = "JE";
      char cstring6[5] = "JNE";
      char ctrl[5];

      if(temp == TK_LT){ strcpy(ctrl,cstring1); }
      else if(temp == TK_LE){ strcpy(ctrl,cstring2); }
      else if(temp == TK_GT){ strcpy(ctrl,cstring3); }
      else if(temp == TK_GE){ strcpy(ctrl,cstring4); }
      else if(temp == TK_EQ){ strcpy(ctrl,cstring5); }
      else if(temp == TK_NE){ strcpy(ctrl,cstring6); }

      fprintf(fp,"%s X%d\n",ctrl,uno);
      fprintf(fp,"MOV EAX,0\n");
      fprintf(fp,"JMP E%d\n",uno);
      fprintf(fp,"X%d:\n",uno);
      fprintf(fp,"MOV EAX,1\n");
      fprintf(fp,"E%d:\n",uno);
    }
  }
  else {
      if(anynode->children[0]->token_type == TK_TRUE)
        fprintf(fp, "MOV EAX,1\n");
      else
        fprintf(fp, "MOV EAX,0\n");
  }

}

void aob_handler(SymbolHashTable*** symboltables_ptr, ASTNode* aobnode, int scope){
  anyterm_handler(symboltables_ptr, aobnode->children[0], scope);

  if(aobnode->children[1] == NULL)
    return;
  // logical operator;
  // fprintf(fp,"MOV ECX,EAX\n");
  fprintf(fp, "push rax\n");
  aob_handler(symboltables_ptr, aobnode->children[1],scope);
  fprintf(fp, "pop rcx\n");

  if(aobnode->children[1]->syn_attribute.token_type == TK_AND){
          fprintf(fp,"AND EAX,ECX\n");
  }else if(aobnode->children[1]->syn_attribute.token_type == TK_OR){
    fprintf(fp,"OR EAX,ECX\n");
  }


}
void unary_handler(SymbolHashTable*** symboltables_ptr, ASTNode* unarynode, int scope) {
        if(unarynode->children[0]->token_type == arithmeticExpr){
        arithmetic_handler(symboltables_ptr, unarynode->children[0], scope);
    }
        else if(unarynode->children[0]->token_type == var_id_num) {
        varidnum_handler(symboltables_ptr, unarynode->children[0],scope);
        }

      if (unarynode->syn_attribute.token_type == TK_PLUS) {
        // Unary Plus
        fprintf(fp,"MOV CX,1\n");
      }
      else {
        fprintf(fp,"MOV CX,-1\n");
      }
        fprintf(fp,"IMUL        CX");
}
