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

### BNF
```bnf
<entry> ::= <statement>*

<number> ::= [-][0-9][0-9_]*
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*
<type> ::= "uint8" | "int8" | "uint16" | "int16" | "uint32" | "int32" | "uint64" | "int64"
<expression> ::= <number> | <identifier> | <operation> | "(" <expression> ")"
<operation> ::= ( <unary_operator> <expression> ) | ( <expression> <binary_operator> <expression> )
<unary_operator> ::= "!" | "-"
<binary_operator> ::= "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "&&" | "||" | "<<" | ">>"
<statement> ::= (<variable_declaration> | <assignment> | <goto>) ";" | <statement_if>
<statement_if> ::= "if" <expression> "then" <statement> [ "else" <statement>] "fi"
<variable_declaration> ::= <type> <identifier>
<assigment> ::= <type> <identifier> "=" <expression>
<goto> ::= "goto" <identifier>
```
