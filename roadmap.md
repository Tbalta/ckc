# 1st Step
## Description
At the end of this step, the compiler should be able to handle basic math operation and goto.
Branch control "if" should also be supported.

The entry point of the program should be the first line of code (not variable declaration).

Only integer should be supported.

Variable declaration should be supported.

the expression inside the return statetement should be the value returned by the function.

## Glossary
expression: a line that has a value.
statement: a line that doesn't have a value.
block: a set of statement.
## BNF
```bnf
<entry> ::= <block>*

<number> ::= [-][0-9][0-9_]*
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*
<type> ::= "uint8" | "int8" | "uint16" | "int16" | "uint32" | "int32" | "uint64" | "int64"
<expression> ::= <number> | <identifier> | <operation> | "(" <expression> ")"
<operation> ::= ( <unary_operator> <expression> ) | ( <expression> <binary_operator> <expression> )
<logical_operators> ::= "and" | "or" | "xor"
<logical_unary_operators> ::= "not"
<unary_operators> ::= <logical_unary_operators> | "-" | "~"

<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "<<" | ">>"
<statement> ::= (<variable_declaration> | <assignment> | <goto> | <return>) ";"
<block> ::= <block_modifier> (<block_if> | <statement>)
<multi_block> ::= <block>*
<block_if> ::= "if" <expression> "then" <multi_block> [ "else" <multi_block>] "fi"
<variable_declaration> ::= <type> <identifier> [ "=" <expression> ]
<assigment> ::= <identifier> "=" <expression>
<goto> ::= "goto" <identifier>
<return> ::= "return" <expression>
<block_modifier> ::= <named_block>
<named_block> ::= "#" <identifier>
```
#### Note
`<binary_operator>` is given for information purpose only. The parser will not use this grammar. 

# 2nd Step
## Functionalities
2nd step will focus on the interoperability between differents languages.
* Declaration and functions call.
* Partial function call.
* Function overloading.
* New type system.
* Function export with the pragma "export_name".
* Function import with the pragma "import_name".
* Type casting.

## Quality of life
A docker image should be provided in order to simplify the compilation of gkc and the compilation with gkc.
## Code quality
Effort with the test suite will be continued.
Code documentation will be improved.


## BNF
```bnf
<function_def> ::= "function" <identifier> "(" <function_parameter> ")"  [return <type>] "is" <multi_block> "endfunction"
<partial_function> ::= "partial" <identifier> "(" <function_parameter> ")" is <function_call>

<function_parameter> ::= (<type> <identifier> ("," <type> <identifier>)*)?
<function_call> ::= <identifier> "(" <expression> ("," <expression>)* ")"
<cast_expression> ::= <type> "(" <expression> ")"

<pragma_identifier> ::= "export_name" | "import_name"
<pragma> ::= "pragma" <identifier> <pragma_identifier> "is" (<number> | <identifier>)


<expression> ::= <number> | <identifier> | <operation> | "(" <expression> ")" | <function_call> | <cast_expression>
<block_modifier> ::= "#" ( <named_block> )
<block> ::= <block_modifier>* (<block_if> | <statement> | <pragma>)
<named_block> ::= <identifier>

```

## Remarks
Function without `return <type>` should be considered as void function.\
Errors in typing should result in a compilation error.\
Block modifier syntax is temporary and subject to change.