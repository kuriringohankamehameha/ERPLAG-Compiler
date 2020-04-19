# Group 42:
# R.VIJAY KRISHNA 2017A7PS0183P
# ROHIT K 2017A7PS0177P
all:
	gcc -o driver.out driver.c parser.c lexer.c hash_table.c common.c -ggdb3
	gcc -o ast_test.out ast_test.c ast.c parser.c lexer.c hash_table.c common.c -ggdb3
	gcc -o symbol_table_test.out symbol_table_test.c symbol_table.c common.c stack.c function_table.c -ggdb3
	gcc -o semantic_checks.out semantic_checks.c ast.c symbol_table.c common.c hash_table.c lexer.c parser.c stack.c function_table.c -ggdb3

clean:
	rm *.out
