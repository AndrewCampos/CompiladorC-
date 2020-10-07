#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"

const char *Instr[] = { "add", "sub", "mult", "div", "and", "or", "nand", "nor", "sle", "slt", "sge", "addi", "subi", "divi", "multi", "andi", "ori",
                        "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "sti", "ldi", "str", "ldr", "hlt", "in", "out", "jmp", "jal", "jst" };

const char *opcodeBins[] =   {"000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", 
                              "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010", "001011",
                              "001100", "001101", "001110", "001111", "010000", "010001", "010010", "010011", "010100", "010101", "010110",
                              "010111"};

const char *functBins[] = { "000000", "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010" };

const char *regBins[] = {  "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011", "01100", "01101", "01110",
                           "01111", "10000", "10001", "10010", "10011", "10100", "10101", "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101",
                           "11110", "11111" };

void assembly2binary(Instruction inst){

}

void generateBinary () {
    AssemblyCode a = getAssembly();
    printf(AZ "\nC- Binary Code\n" RESET);

    while (a != NULL) {
        if (a->kind == instr) {
        fprintf(listing,"mem[%d] = 32'b", a->lineno);
            assembly2binary(a->line.instruction);
            fprintf(listing,"// %s\n", Instr[a->line.instruction.opcode]);
        }
        else {
            fprintf(listing,"//%s\n", a->line.label);
        }
        a = a->next;
    }
}
