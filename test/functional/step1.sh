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
    if [ $? -ne "$expected" ]; then
        echo "Wrong output: $toCompile"
        exit 1
    fi
}

# Basic tests
test "return 42;" 42
test "int32 i = 42; return i;" 42

# If tests
test "if 1 then return 42; else return 0; fi" 42

# Advanced tests
test "int32 i = 0; int32 res = 1; int32 x = 5; #myif if i < x then res = res * 2; i = i + 1; goto myif; else return res; fi" 32