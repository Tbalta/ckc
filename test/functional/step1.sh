#!/bin/bash


executablePath=$1

function test
{
    toCompile=$1
    expected=$2

    echo "$toCompile" > "functest.kc"
    ./$executablePath "functest.kc" --silent
    if [ $? -ne 0 ]; then
        echo "Compilation failed: $toCompile"
        exit 1
    fi
    gcc output.o
    if [ $? -ne 0 ]; then
        echo "gcc failed: $toCompile"
        exit 1
    fi
    ./a.out
    result=$?
    if [ $result -ne "$expected" ]; then
        echo "Wrong output: $toCompile | Expected: $expected Got: $result"
        exit 1
    fi
}

# Basic tests
test "return 42;" 42
test "int32 i := 42; return i;" 42

# Basic arithmetic
test "int32 i := 42; int32 j := 2; return i + j;" 44
test "int32 i := 42; int32 j := 2; return i - j;" 40
test "int32 i := 42; int32 j := 2; return i * j;" 84
test "int32 i := 42; int32 j := 2; return i / j;" 21
test "int32 i := 42; int32 j := 2; return i % j;" 0

# Basic comparisons
test "int32 i := 42; int32 j := 2; return i = j;" 0
test "int32 i := 42; int32 j := 2; return i != j;" 1
test "int32 i := 42; int32 j := 2; return i < j;" 0
test "int32 i := 42; int32 j := 2; return i <= j;" 0
test "int32 i := 42; int32 j := 2; return i > j;" 1
test "int32 i := 42; int32 j := 2; return i >= j;" 1

# Basic bitwise
test "return 5 & 6;" 4
test "return 5 | 6;" 7
test "return ~0;" 255

test "int32 i := 42; int32 j := 2; return i << j;" 168
test "int32 i := 42; int32 j := 2; return i >> j;" 10

# Basic logical
test "return 0 and 0;" 0
test "return 0 and 1;" 0
test "return 1 and 0;" 0
test "return 1 and 1;" 1

test "return 0 or 0;" 0
test "return 0 or 1;" 1
test "return 1 or 0;" 1
test "return 1 or 1;" 1

test "return not 0;" 1
test "return not 1;" 0


# If tests
test "if 1 then return 42; else return 0; fi" 42

# Advanced tests
test "int32 i := 0; int32 res := 1; int32 x := 5; #myif if i < x then res := res * 2; i := i + 1; goto myif; else return res; fi" 32

# Test unary operators
test "int32 i := 42; return not i;" 0
test "return -42;" 214