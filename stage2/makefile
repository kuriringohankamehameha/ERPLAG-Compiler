# Group 42:
# R.VIJAY KRISHNA 2017A7PS0183P
# ROHIT K 2017A7PS0177P
all:
	gcc -o driver.out driver.c parser.c lexer.c hash_table.c common.c ast.c stack.c function_table.c symbol_table.c old_codegen.c -ggdb3
	gcc -o codegen_test.out codegen_test.c old_codegen.c symbol_table.c stack.c function_table.c ast.c parser.c lexer.c common.c hash_table.c -ggdb3

clean:
	rm *.out
