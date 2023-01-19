# 1st Step
## Description
At the end of this step, the compiler should be able to handle basic math operation and goto.
Branch control "if" should also be supported.

The entry point of the program should be the first line of code (not variable declaration).

Only integer should be supported.

Variable declaration should be supported.

## Glossary
expression: a line that has a value.
statement: a line that doesn't have a value.
block: a set of statement.
### BNF
```bnf
<entry> ::= <block>*

<number> ::= [-][0-9][0-9_]*
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*
<type> ::= "uint8" | "int8" | "uint16" | "int16" | "uint32" | "int32" | "uint64" | "int64"
<expression> ::= <number> | <identifier> | <operation> | "(" <expression> ")"
<operation> ::= ( <unary_operator> <expression> ) | ( <expression> <binary_operator> <expression> )
<unary_operator> ::= "!" | "-"
<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "&&" | "||" | "<<" | ">>"
<statement> ::= (<variable_declaration> | <assignment> | <goto>) ";"
<block> ::= <block_modifier> (<block_if> | <statement>)
<block_if> ::= "if" <expression> "then" <block> [ "else" <block>] "fi"
<multi_block> ::= <block>*
<variable_declaration> ::= <type> <identifier> [ "=" <expression> ]
<assigment> ::= <identifier> "=" <expression>
<goto> ::= "goto" <identifier>
<block_modifier> ::= <named_block>
<named_block> ::= "#" <identifier>
```
