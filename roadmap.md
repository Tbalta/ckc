# Status


## Current version
Currently on step2 of the roadmap.\

## Glossary
expression: Line that has a value.
statement: Line that doesn't have a value.
block: Set of statements.

## Current BNF
```bnf
<entry> ::= <multi_block>
<multi_block> ::= <block>*
<block> ::= <block_modifier>* (<block_if> | <block_for> | <block_while> | <statement> | <pragma> | <function_def>)
<statement> ::= <inline_statement> ";"
<inline_statement> ::= <variable_declaration> | <assignment> | <goto> | <return> | <partial_function>


<number> ::= [-][0-9][0-9_]*
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*
<type> ::= "uint8" | "int8" | "uint16" | "int16" | "uint32" | "int32" | "uint64" | "int64"
<expression> ::= <number> | <identifier> | <operation> | "(" <expression> ")" | <function_call> | <cast_expression>
<operation> ::= ( <unary_operator> <expression> ) | ( <expression> <binary_operator> <expression> )
<logical_operator> ::= "and" | "or" | "xor"
<logical_unary_operator> ::= "not"
<unary_operator> ::= <logical_unary_operator> | "-" | "~"

<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "<<" | ">>"
<block_if> ::= "if" <expression> "then" <multi_block> [ "else" <multi_block>] "fi"
<variable_declaration> ::= <type> <identifier> [ "=" <expression> ]
<assigment> ::= <identifier> "=" <expression>
<goto> ::= "goto" <identifier> #
<return> ::= "return" <expression>
<block_modifier> ::= <named_block>
<named_block> ::= "#" <identifier>

<function_definition> ::= "function" <identifier> "(" <function_parameter> ")"  (<complete_function_definition> | <simplified_function_definition>)
<complete_function_definition> ::= "return" <type> "is" <multi_block> "endfunction"
<simplified_function_definition> ::= "is" <function_call> ";"


<partial_function> ::= "partial" <identifier> "(" <function_parameter> ")" is <function_call>

<function_parameter> ::= (<type> <identifier> ("," <type> <identifier>)*)?
<function_call> ::= <identifier> "(" <expression> ("," <expression>)* ")"
<cast_expression> ::= <type> "(" <expression> ")"

<pragma_identifier> ::= "symbol_name"
<pragma> ::= "pragma" <identifier> <pragma_identifier> "is" (<number> | <identifier>)


<block_modifier> ::= "#" ( <named_block> )
<named_block> ::= <identifier>
<block_for> ::= "for" "(" <inline_statement> ";" <expression> ";" <inline_statement> ")" <multi_block> "endfor"
<block_while> ::= "while" "(" <expression> ")" <multi_block> "endwhile"
```

# 1st Step
## Description
At the end of this step, the compiler should be able to handle basic math operation and goto.
Branch control "if" should also be supported.

The entry point of the program should be the first line of code (not variable declaration).

Only integer should be supported.

Variable declaration should be supported.

the expression inside the return statement should be the value returned by the function.

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
* For loop.
* Goto has been removed.

## Quality of life
A docker image should be provided in order to simplify the compilation of gkc and the compilation with gkc.
## Code quality
Effort with the test suite will be continued.
Code documentation will be improved.

## Remarks
Function without `return <type>` should be considered as void function.\
Errors in typing should result in a compilation error.\
Block modifier syntax is temporary and subject to change.
