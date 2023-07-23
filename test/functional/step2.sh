#!/bin/bash

executablePath=$1

function test
{
    toCompile_C=$1
    toCompile_KC=$2
    expected=$3

    echo "$toCompile_KC" > "functest.kc"
    $executablePath "functest.kc" --silent
    if [ $? -ne 0 ]; then
        echo "Compilation failed: $toCompile_KC"
        exit 1
    fi

    echo "$toCompile_C" > "functest.c"
    gcc functest.c output.o > /dev/null 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "gcc failed: $toCompile_C"
        exit 1
    fi

    ./a.out > /dev/null
    result=$?
    if [ "$result" != "$expected" ]; then
        echo "Wrong output: $toCompile_KC | Expected: $expected Got: $result"
        exit 1
    fi
}

test 'extern int facto(int); int main(void) { return facto(5); }' 'function facto (int32 to) return int32 is int32 plouf := 1; #loop if to > 0 then plouf := to * plouf; to := to - 1; goto loop; fi return plouf; endfunction'  120
test '' 'function fibo (int32 to) return int32 is if to <= 1 then return to; fi return fibo(to - 1) + fibo(to - 2); endfunction function main () return int32 is return fibo(5); endfunction' 5
test 'extern int facto(int); int f_c(void) { printf("facto(5)=%d\n", facto(5)); return 0; }' 'function facto (int32 to) return int32 is int32 plouf := 1; #loop if to > 0 then plouf := to * plouf; to := to - 1; goto loop; fi return plouf; endfunction function f_ckc (int32 to) return int32; pragma f_ckc symbol_name is f_c; function main () return int32 is return f_ckc(3); endfunction' 0
