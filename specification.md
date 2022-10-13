# General specification

## Runtime
No runtime is allowed. The language only expect a stack to be set in order to be functional.
Everything that can be written without any library should be able to run on bare metal.
Only symbol defined by the user should created.

## Execution time
No check should be performed at the execution time.

## Compilation
Everything that can be done at compilation time should be done.

## Symbol name
User should have a full control over generated symbol name.

# BNF
A statement is line without a value.
An expression is a line with a value.

```bnf
<statement> ::= ( <procedure_call> | <if> | <assigment> | <multiline_statement> ) ";"
<multiline_statement> ::= "{" <statement>* "}"

<expression> ::= ( <function_call> | <number> )
<function_call> ::= <function_name> "(" <expression>* ")"
<function_name> ::= "function"
<number> ::= [0-9]+


<if> ::= "if" <expression> "then" <statement> ("else" <statement>)?
<assigment> ::= <type> <identifier> "=" <expression>
<procedure_call> ::= <procedure_identifier> "(" <expression>* ")"
<procedure_identifier> ::= "procedure"
<type> ::= "int" | "bool" | "void"
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*

```

# Detailled specification
## Subtype declaration.

```
type <name> is <type>;
```

```
type boolean is int range 0..1;
type day is int range 0..31;
```

### Compilation verification
```
type my_int is int range 0..100;

my_int x = 50;
my_int y = 30;
my_int a = x + y; // Should be valid because value can be determined at compil time.
my_int b = x + x + 1; // Is illegal.

// Illegal declaration. cannot assert x + y is of type my_int.
function test (my_int x, my_int y) returns my_int {
    return x + y;
}

```

### Type operation
```
type my_int is int range 0..100;
type my_double_int is my_int + my_int;
type my_double_int_bis is range 0 .. 200;

struct whatever 
{
    my_int foo;
    my_double_int bar;
}


type whatever_add is whatever + my_int; // Error: no addition function found for type whatever and my_int.

function operator+ (whatever y, my_int x) return whatever
{
    return whatever {
        foo = y.foo + x,
        bar;
    }
}

type whatever_add is whatever + my_int; // Is valid now that operator+ is defined.
```

### With clause
```
type my_int is int range 0..100;
with : <identifier> 
{
    "Size" => <size>;
    "Alignment" => <alignment>;
    ("export_name" | "import_name") => <export_name>;
    "Packed" => Boolean;
}


with : my_int 
{
    "Size" => 4;
    "Alignment" => 16;
}


my_int x = 50;
with : x 
{
    "Size" => 4; // Warning attribute Size is already inherited from my_int.
    export_name => "my_int_x";
}
```