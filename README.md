# Compiler Construction Project
This is the repository for the course project **CS F363 - Compiler Construction** in **BITS Pilani**.

## Objective

Build a compiler for a custom language **ERPLAG**, that is a strongly typed language.

## Compiler RoadMap
* Analyze the source code and convert it into `tokens` and `lexmes`. This is the task of the lexical analyzer.<br/>
* The next step is for the program to check the validity of the tokens and perform some syntax checks involving the `tokens` and `lexmes`. Checks are done using a **Lookup Table** Data Structure. After checking for errors, on successful completion, a parse tree is generated.<br/>
* The next stage involves checking the ambiguity of the generated parse tree using the grammar of the language. This is contained in a module called the **Semantic Analyzer**. The output of this stage will be an unambiguous parse tree.<br/>
* Now, the parse tree generated will complete the front-end of the compiler.<br/>


## Stage 1
- [x] DFA
- [x] Lexer / Tokenizer
- &#9744; Symbol Table Operations
- &#9744; Parser
- &#9744; Code Optimizer
- &#9744; Intermediate Code Generation

Group Members:<br/>
**VijayKrishna Ramchandran : 2017A7PS0183P**<br/>
**Rohit Krishnamoorthy : 2017A7PS0177P**
