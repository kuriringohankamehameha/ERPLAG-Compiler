# Semantic rules for AST

## Group 42
* ROHIT K                 2017A7PS1077P
* R VIJAY KRISHNA         2017A7PS0183P
* Date : 21st March 2020

## ASSUMPTIONS:
* We already have the Symbol table, using which we can get imformation, such as the type and the values of identifiers
* There are multiple ways of assigning attributes to the nodes. We can pass on inherited attributes from the Left to the Right, or simply use a resursive approach when generating the AST from the Parse Tree
* For grammar rules of the form A -> E, we can simply assign the corresponding node as NULL, or simply pass on inherited attributes to the parent, depending on the type of approach chosen.


## AST Rules:
$ <program> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
``` <program>.node = new Node(```
                        <moduleDeclarations>.node,
                         <otherModules>.node,
                         <driverModule>.node,
                         <otherModules>.node,
                        )


$ <moduleDeclarations> <moduleDeclaration> <moduleDeclarations>
``` <moduleDeclarations>.node = new Node(```
                                 <moduleDeclaration>.node,
                                 <moduleDeclarations>.node
                                )


$ <moduleDeclarations> E 
``` <moduleDeclarations>.node = null ```
  
$ <moduleDeclaration> TK_DECLARE TK_MODULE TK_ID TK_SEMICOL
``` addToSymbolTable(TK_ID, “Function”, TK_ID.lexeme)```
``` <moduleDeclaration>.node = new LeafNode(TK_ID,TK_ID.lexeme)```


$ <otherModules> <module> <otherModules> 
``` <otherModules>.node = new Node( ```
                            <module>.node,
                             <otherModules>.node,
                            )


$ <otherModules> E
``` <otherModules>.node = null```

$ <driverModule> TK_DRIVERDEF TK_DRIVER TK_PROGRAM TK_DRIVERENDDEF <moduleDef>
``` <driverModule>.node =  <moduleDef>.node```

$ <module> TK_DEF TK_MODULE TK_ID TK_ENDDEF TK_TAKES TK_INPUT TK_SQBO <input_plist> TK_SQBC TK_SEMICOL <ret> <moduleDef>
``` <module>.node = <moduleDef>.node```
``` <moduleDef>.inh1  =  <input_plist>.list```
``` <moduleDef>.inh2  =  <ret>.list```


$ <ret> TK_RETURNS TK_SQBO <output_plist> TK_SQBC TK_SEMICOL 
``` <ret>.list = <output_plist>.list```

$ <ret> E
``` <ret>.list = []```

$ <input_plist> TK_ID TK_COLON <dataType> <N1>
``` <input_plist>.list = <N1>.list```
``` <N1>.inh = [TK_ID,<dataType>.prop]```

$ <N1> TK_COMMA TK_ID TK_COLON <dataType> <N1>1 
``` <N1>.list = <N1>1.list.append(<N1>.inh)```
``` <N1>1.inh = [TK_ID,<dataType>.prop]```

$ <N1> E
``` <N1>.list =<N1>.inh```

$ <output_plist> TK_ID TK_COLON <type> <N2>
``` <output_plist>.list = <N2>.list```
``` <N2>.inh = [TK_ID,<type>.val]```

$ <N2> TK_COMMA TK_ID TK_COLON <type> <N2> 
``` <N2>.list = <N2>1.list.append(<N2>.inh)```
``` <N2>1.inh = [TK_ID,<type>.val]```


$ <N2> E
``` <N2>.list =<N2>.inh```

$ <dataType> TK_INTEGER 
``` <dataType>.node  = new Leaf Node (TK_INTEGER,TK_INTEGER.lexeme) ```
``` <dataType>.prop.val = TK_INTEGER.lexeme```

$ <dataType> TK_REAL  
``` <dataType>.node  = new Leaf Node (TK_REAL,TK_REAL.lexeme)```
``` <dataType>.prop.val = TK_REAL.lexeme```

$ <dataType> TK_BOOLEAN  
``` <dataType>.node  = new Leaf Node (TK_BOOLEAN,TK_BOOLEAN)```
``` <dataType>.prop.val = TK_BOOLEAN.lexeme```

$ <dataType> TK_ARRAY TK_SQBO <range_arrays> TK_SQBC TK_OF <type>
``` <dataType>.node = new Node (```
                        TK_ARRAY.lexeme,
                        <range_arrays>.node,
                        <type>.node
                      )
``` <dataType>.prop.val = TK_ARRAY```

$ <range_arrays> <index> TK_RANGEOP <index>
``` <range_arrays>.node = new Node (```
                            <index>1.node,
                            <index>2.node
                        )

$ <type> TK_INTEGER 
``` <type>.node = new Leaf Node (TK_INTEGER, TK_INTEGER)```
``` <type>.type = INTEGER```

$ <type> TK_REAL 
``` <type>.node = new Leaf Node (TK_REAL, TK_REAL)```
``` <type>.type = REAL```

$ <type> TK_BOOLEAN
``` <type>.node = new Leaf Node (TK_BOOLEAN, TK_BOOLEAN)```
``` <type>.type = BOOLEAN```

$ <moduleDef> TK_START <statements> TK_END
``` <moduleDef>.node = <statements>.node```

$ <statements> <statement> <statements> 
``` <statements>.node = new Node (<statement>.node, <statements>1.node)```

$ <statements> E
``` <statements>.node = NULL```

$ <statement> <ioStmt>
``` <statement>.node = <ioStmt>.node```

$ <statement> <simpleStmt> 
``` <statement>.node = <simplestmt>.node```

$ <statement> <declareStmt> 
``` <statement>.node = <declarestmt>.node```

$ <statement> <condionalStmt> 
``` <statement>.node = <condionalstmt>.node```

$ <statement> <iterativeStmt>
``` <statement>.node = <iterativestmt>.node```

$ <ioStmt> TK_GET_VALUE TK_BO TK_ID TK_BC TK_SEMICOL 
``` <ioStmt>.node = new Leaf Node (TK_ID, TK_ID.lexeme)```
``` <ioStmt>.type = GET_STMT```

$ <ioStmt> TK_PRINT TK_BO <var> TK_BC TK_SEMICOL
``` <ioStmt>.node = new Node (<var>.node)```
``` <ioStmt>.type = PRINT_STMT```

$ <var> <var_id_num>
``` <var>.node = <var_id_num>.node```
 
$ <var> <boolConstt>
``` <var>.node = <boolConstt>.node```


$ <boolConstt> TK_TRUE 
``` <boolConstt>.node =  new LeafNode(TK_TRUE, TK_TRUE.lexeme )```

$ <boolConstt> TK_FALSE
``` <boolConstt>.node =  new LeafNode(TK_FALSE, TK_FALSE.lexeme )```

$ <var_id_num> TK_ID <whichId> 
``` <var_id_num>.node = new Node(new Leaf Node(TK_ID, TK_ID.Lexeme), <whichId>.node)```
   
$ <var_id_num> TK_NUM 
``` <var_id_num>.node = new LeafNode(TK_NUM, TK_NUM.lexeme)```

$ <var_id_num> TK_RNUM
``` <var_id_num>.node = new LeafNode(TK_RNUM, TK_RNUM.lexeme)```


// * dereference operator
$ <whichId> TK_SQBO <index> TK_SQBC 
``` <whichId>.node  =  <index>.node```
``` <whichId>.node.val = *(<whichId>.inh + <index>.val)```

$ <whichId> E
``` <whichId>.node  = NULL```
``` <whichId>.node.val =  *(<whichID>.inh)```

$ <simpleStmt> <assignmentStmt> 
``` <simpleStmt>.node = <assignmentStmt>.node```

$ <simpleStmt> <moduleReuseStmt>
``` <simpleStmt>.node = <moduleReuseStmt>.node```

$ <assignmentStmt> TK_ID <whichStmt>
``` <assignmentStmt>.node = new Node (new Leaf Node(TK_ID, TK_ID.Lexeme), <whichStmt>.node)```

$ <whichStmt> <lvalueIDStmt> 
``` <whichStmt>.node = <lvalueIDStmt>.node```
``` <whichStmt>.type = <lvaueIDStmt>.type```

$ <whichStmt> <lvalueARRStmt>
``` <whichStmt>.node = <lvalueARRStmt>.node```
``` <whichStmt>.type = <lvalueARRStmt>.type```

$ <lvalueIDStmt> TK_ASSIGNOP <expression> TK_SEMICOL
``` <lvalueIDStmt>.node = <expression>.node```
``` <lvalueIDStmt>.type = <expression>.node.type```

$ <lvalueARRStmt> TK_SQBO <index> TK_SQBC TK_ASSIGNOP <expression> TK_SEMICOL
``` <lvalueARRStmt>.node = new Node(<index>.node, <expression>.node)```
``` <lvalueARRStmt>.type = <expression>.node.type```

$ <index> TK_NUM 
``` <index>.node = new Leaf Node(TK_NUM, TK_NUM.lexeme)```

$ <index> TK_ID
``` <index>.node = new Leaf Node(TK_ID, TK_ID.lexeme)```

$ <moduleReuseStmt> <optional> TK_USE  TK_ID TK_WITH TK_PARAMETERS <idList> TK_SEMICOL
``` <moduleReuseStmt>.node = new Node (<optional>.node, new Leaf Node(TK_ID, TK_ID.Lexeme), <idList>.node)```

$ <optional> TK_SQBO <idList> TK_SQBC TK_ASSIGNOP 
``` <optional>.node = <idList>.node```

$ <optional> E
``` <optional>.node = NULL```

$ <idList> TK_ID <N3>
``` <idList>.node = new Node (new Leaf Node(TK_ID, TK_ID.Lexeme), <N3>.node)```

$ <N3> TK_COMMA TK_ID <N3> 
``` <N3>.node = new Node (new Leaf Node(TK_ID, TK_ID.Lexeme), <N3>.node)```

$ <N3> E
``` <N3>.node = NULL```

$ <expression> <arithmeticOrBooleanExpr> 
``` <expression>.node = <arithmeticOrBooleanExpr>.node```


$ <expression> <U>
``` <expression>.node = <U>.node```


$ <U> <unary_op> <new_NT>
``` <U>.node = new Node(<unary_op>.node, <new_NT>.node)```


$ <new_NT> TK_BO <arithmeticExpr> TK_BC
``` <new_NT>.node =  <arithmeticExpr>.node```


$ <new_NT> <var_id_num>
``` <new_NT>.node = <var_id_num>.node```


$ <unary_op> TK_PLUS 
``` <unary_op>.node = new Leaf Node(TK_PLUS, TK_PLUS)```


$ <unary_op> TK_MINUS
``` <unary_op>.node = new Leaf Node(TK_MINUS, TK_MINUS)```


$ <arithmeticOrBooleanExpr> <AnyTerm> <N7>
``` <arithmeticOrBooleanExpr>.node = new Node(<AnyTerm>.node, <N7>.node)```


$ <N7> <logicalOp> <AnyTerm> <N7>1 
``` <N7>.node  = new Node(<logicalOp>.node, <AnyTerm>.node, <N7>.node)```

$ <N7> E
``` <N7>.node  = NULL```


$ <AnyTerm> <arithmeticExpr> <N8> 
``` <AnyTerm>.node = new Node (<arithmeticExpr>.node, <N8>.node)```


$ <AnyTerm> <boolConstt>
``` <AnyTerm>.node = <boolConstt>.node```

$ <N8> <relationalOp> <arithmeticExpr> 
``` <N8>.node = new Node(<relationalOp>.node, <arithmeticExpr>.node)```

$ <N8> E
``` <N8>.node = NULL OR  <N8>.node = <N8>.inh```

$ <arithmeticExpr> <term> <N4>
``` <arithmeticExpr>.node = new Node(<term>.node, <N4>.node)```
``` <arithmeticExpr>.val =  <N4>.val```
``` <N4>.inh = <term>.val```

$ <N4> <op1> <term> <N4>1  
``` <N4>.val =  <N4>.inh <op1>.op <N4>1.val```
``` <N4>1.inh = <term>.val```

$ <N4> E
``` <N4>1.val = <term>.inh```

$ <term> <factor> <N5>
``` <term>.val =  <N5>.val```
```  <N5>.inh = <factor>.val ```


$ <N5> <op2> <factor> <N5>1
``` <N5>.val = <N5>.inh <op2>.op <N5>1.val```
``` <N5>1.inh = <factor>.val```
 
$ <N5> E
``` <N5>.val = <N5>.inh```


$ <factor> TK_BO <arithmeticOrBooleanExpr> TK_BC 
``` <factor>.node  = <arithmeticOrBooleanExpr>.node```


$ <factor> <var_id_num>
``` <factor>.node  = <var_id_num>.node ```


$ <op1> TK_PLUS 
``` <relationalOp>.node = new Leaf Node(TK_PLUS, TK_PLUS)```
``` <op1>.val = TK_PLUS.lexeme```


$ <op1> TK_MINUS
``` <relationalOp>.node = new Leaf Node(TK_MINUS, TK_MINUS)```
``` <op1>.val = TK_MINUS.lexeme```


$ <op2> TK_MUL 
``` <relationalOp>.node = new Leaf Node(TK_MUL, TK_MUL)```
``` <op2>.val = TK_MUL.lexeme```


$ <op2> TK_DIV
``` <relationalOp>.node = new Leaf Node(TK_DIV, TK_DIV)```
``` <op2>.val = TK_DIV.lexeme```


$ <logicalOp> TK_AND
``` <relationalOp>.node = new Leaf Node(TK_AND, TK_AND)```
``` <logicalOp>.val = TK_AND.lexeme```


$ <logicalOp> TK_OR
``` <relationalOp>.node = new Leaf Node(TK_OR, TK_OR)```
``` <logicalOp>.op = TK_OR.lexeme```


$ <relationalOp> TK_LT
``` <relationalOp>.node = new Leaf Node(TK_LT, TK_LT)```
``` <relationalOp>.op = TK_LT.lexeme```

$ <relationalOp> TK_LE
``` <relationalOp>.node = new Leaf Node(TK_LE, TK_LE)```
``` <relationalOp>.op = TK_LE.lexeme```


$ <relationalOp> TK_GT
``` <relationalOp>.node = new Leaf Node(TK_GT, TK_GT)```
``` <relationalOp>.op = TK_GT.lexeme```


$ <relationalOp> TK_GE
``` <relationalOp>.node = new Leaf Node(TK_GE, TK_GE)```
``` <relationalOp>.op = TK_GE.lexeme```


$ <relationalOp> TK_EQ
``` <relationalOp>.node = new Leaf Node(TK_EQ, TK_EQ)```
``` <relationalOp>.op = TK_EQ.lexeme```

$ <relationalOp> TK_NE
``` <relationalOp>.node = new Leaf Node(TK_NE, TK_NE)```
``` <relationalOp>.op = TK_NE.lexeme```

$ <declareStmt> TK_DECLARE <idList> TK_COLON <dataType> TK_SEMICOL
``` <declareStmt>.node = new Node (```
                                <idList>.node,
                                <datatype>.node
                                )


$ <condionalStmt> TK_SWITCH TK_BO TK_ID TK_BC TK_START <caseStmts> <default> TK_END
``` <conditionalStmt>.node = new Node (new Leaf Node(TK_ID, TK_ID.Lexeme), <caseStmts>.node, <default>.node)```

$ <caseStmts> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
``` <casestmts>.node = new Node (<value>.node, <statements>.node, <N9>.node)```

$ <N9> TK_CASE <value> TK_COLON <statements> TK_BREAK TK_SEMICOL <N9>
``` <N9>.node = new Node (<value>.node, <statements>.node, <N9>.node)```

$ <N9> E
``` <N9>.node = NULL (or even <N9>.inh)```

$ <value> TK_NUM 
``` <value>.node = new Leaf Node(TK_NUM, TK_NUM.Lexeme)```
``` <value>.type = TK_NUM.type (from the Symbol table)```

$ <value> TK_TRUE 
``` <value>.node = new Leaf Node(TK_TRUE, TK_TRUE)```
``` <value>.type = BOOLEAN```

$ <value> TK_FALSE
``` <value>.node = new Leaf Node(TK_FALSE, TK_FALSE)```
``` <value>.type = BOOLEAN```

$ <default> TK_DEFAULT TK_COLON <statements> TK_BREAK TK_SEMICOL 
``` <default>.node = <statements>.node```

$ <default> E
``` <default>.node = NULL (or even <default>.inh)```

$ <iterativeStmt> TK_FOR TK_BO TK_ID TK_IN <range> TK_BC TK_START <statements> TK_END 
``` <iterativestmt>.node = new Node (new Leaf Node(TK_ID, TK_ID.Lexeme), <range>.node, <statements>.node)```

$ <iterativeStmt> TK_WHILE TK_BO <arithmeticOrBooleanExpr> TK_BC TK_START <statements> TK_END
``` <iterativestmt>.node = new Node (<arithmeticOrBooleanExpr>.node, <statements>.node)```

$ <range> TK_NUM TK_RANGEOP TK_NUM
``` <range>.node = new Leaf Node(TK_NUM1, TK_NUM1.Lexeme, TK_NUM2, TK_NUM2.Lexeme)```
