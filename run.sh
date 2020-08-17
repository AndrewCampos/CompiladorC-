#!/bin/bash
flex Scanner.l
bison -d Parser.y
gcc -c *.c -fno-builtin-exp -Wno-implicit-function-declaration
gcc *.o -lfl -o compilador -fno-builtin-exp
