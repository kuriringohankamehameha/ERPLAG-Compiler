# Compiler Construction Project
This is the repository for the course project **CS F363 - Compiler Construction** in **BITS Pilani**.

## Objective

Build a compiler for a custom C-like language **ERPLAG**, that is a strongly typed language.

## Compiler RoadMap
* Analyze the source code and convert it into `tokens` and `lexmes`, using a DFA for state to state transitions. This is the task of the **Lexical Analyzer**.<br/>
* We will be using a top-down predictive parser to generate the Parse Tree, so this requires construction of a Parse Table from the given language Grammar.
* The Parse Table is constructed after computing the first and follow sets for the Grammar, which must be `LL-1` compatible.
* The next step is for the program to check the validity of the tokens and perform some syntax checks involving the `tokens` and `lexmes`. Checks are done using a **Parse Table** Data Structure. After checking for errors, on successful completion, a parse tree is generated. This is all contained in the **Syntax Analyzer** module.<br/>
* The next stage involves checking the ambiguity of the generated parse tree using the grammar of the language. This is contained in a module called the **Semantic Analyzer**. The output of this stage will be an unambiguous parse tree.<br/>
* Now, the unambiguous parse tree generated will complete the front-end of the compiler.<br/>


## Stage 1
- [x] DFA
- [x] Lexer / Tokenizer
- [x] Grammar Rules
- [x] First and Follow Set Implementation
- [x] Parse Table Generation
- [x] Parse Tree Generation

Stage 1 is complete.

## Stage 2
- &#9744; Unambiguous Parse Tree Generation
- &#9744; Code Optimizer
- &#9744; Intermediate Code Generation

Group Members:<br/>
**VijayKrishna Ramchandran : 2017A7PS0183P**<br/>
**Rohit Krishnamoorthy : 2017A7PS0177P**
