# Stage 1

* Some of the ideas involved in this implementation, like the double buffer scheme, are taken from [this](https://www2.cs.arizona.edu/~debray/Teaching/CSc453/class_notes/PPT/1_LexicalAnalysis.ppt) presentation on Lexical Analysis.
* The parse Tree is generated from the grammar, which is taked from `grammar_rules.txt`, which assumes production rules of the format:
```
$ LHS1 -> RHS1 RHS2 ... RHSN
```
* Syntax Errors are handled based on the current symbol on the Stack and a token from the Lexer.
