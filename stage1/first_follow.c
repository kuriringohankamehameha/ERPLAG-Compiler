#include "first_follow.h"

int main() {
	FILE* fp = fopen("grammar_encoding.txt", "r");
	Grammar g = get_grammar(fp);
	print_grammar(g);
	fclose(fp);
	free_grammar(g);
    return 0;
}