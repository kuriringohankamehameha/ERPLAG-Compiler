# Compiler Construction Project
This is the repository for the course project **CS F363 - Compiler Construction** in **BITS Pilani**.

## Objective

Build a compiler for a custom C-like language **ERPLAG**, that is a strongly typed language.

## Compiler RoadMap
* Analyze the source code and convert it into `tokens` and `lexmes`, using a DFA for state to state transitions. This is the task of the **Lexical Analyzer**.<br/>
* We will be using a top-down predictive parser to generate the Parse Tree, so this requires construction of a Parse Table from the given language Grammar.
* The Parse Table is constructed after computing the first and follow sets for the Grammar, which must be `LL-1` compatible.
* The next step is for the program to check the validity of the tokens and perform some syntax checks involving the `tokens` and `lexemes`. Checks are done using a **Parse Table** Data Structure. After checking for errors, on successful completion, a parse tree is generated. This is all contained in the **Syntax Analyzer** module.<br/>
* The next stage involves checking the ambiguity of the generated parse tree using the grammar of the language. This is contained in a module called the **Semantic Analyzer**. The output of this stage will be an unambiguous parse tree.<br/>
* Now, the unambiguous parse tree generated will complete the front-end of the compiler.<br/>
* Now, we generate an **Abstract Syntax Tree (AST)** which is a reduction of the original parse tree, and only contains meaningful tokens for semantic analysis.
* After generating the AST, we now create a Symbol Table data structure which comprises of a list of Hash Tables, made for scope-wise analysis.
* We then perform semantic checks, based on various semantic rules, after inserting the symbols into the Table.
* If there is no error in the semantic analysis, we then move on to the Code Generation step.
* The code generation module takes in the Symbol Table and the AST as input, and outputs an output assembly file.
* This generates a _NASM 64-bit_ Assembly file (for Linux), which we can run using NASM, via :
```
nasm -f elf64 output.asm
```

## Instructions

Compile using:
```
make
```

Run the driver program using:
```
./driver.out input_file.txt output.asm
```

This takes an input testcase file, and returns an `output.asm` assembly file.

You can then run the generated assembly code using *NASM*:
```
nasm -f elf64 output.asm
./output
```

## Stage 1
- [x] DFA
- [x] Lexer / Tokenizer
- [x] Grammar Rules
- [x] First and Follow Set Implementation
- [x] Parse Table Generation
- [x] Parse Tree Generation

Stage 1 is complete.

## Stage 2
- [x] Abstract Syntax Tree Generation
- [x] Symbol Table
- [x] Semantic Analyzer Module
- [x] Code Generation

Stage 2 is complete.

Group Members:<br/>
**Vijay Krishna : 2017A7PS0183P**<br/>
**Rohit K : 2017A7PS0177P**
