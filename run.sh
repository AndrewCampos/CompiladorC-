#!/bin/bash
cd source
flex Scanner.l
bison -d Parser.y
gcc -c *.c -fno-builtin-exp -Wno-implicit-function-declaration
gcc *.o -lfl -o /home/andrew/Documentos/Unifesp/CompiladorC-/compilador -fno-builtin-exp