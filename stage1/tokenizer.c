// Group #42:
// R.VIJAY KRISHNA -> 2017A7PS0183P
// ROHIT K -> 2017A7PS0177P

#include "tokenizer.h"

int main(int argc, char* argv[]) {
    // Driver function to test the tokenizer
    //run_tokenizer("sample.txt");
    if (argc == 2) {
    	run_tokenizer(argv[1]);
    }
    else {
    	fprintf(stderr, "Format: %s input.txt\n", argv[0]);
    	exit(EXIT_FAILURE);
    }
    return 0;
}