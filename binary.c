#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"
#include <string.h>

const char *Prefixos[] = { "add", "sub", "mult", "div", "and", "or", "nand", "nor", "sle", "slt", "sge", "addi", "subi", "divi", "multi", "andi", "ori",
                        "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "sti", "ldi", "str", "ldr", "hlt", "in", "out", "jmp", "jal", "jst" };

const char *opcodeBins[] =   {"000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", 
                              "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010", "001011",
                              "001100", "001101", "001110", "001111", "010000", "010001", "010010", "010011", "010100", "010101", "010110",
                              "010111"};

const char *functBins[] = { "000000", "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010" };

const char *regBins[] = {  "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011", "01100", "01101", "01110",
                           "01111", "10000", "10001", "10010", "10011", "10100", "10101", "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101",
                           "11110", "11111" };

char* getImediate (int im, int size) {
    int i = 0;
    char * bin = (char *) malloc(size + 2);
    size --;
    for (unsigned bit = 1u << size; bit != 0; bit >>= 1) {
        bin[i++] = (im & bit) ? '1' : '0';
    }
    bin[i] = '\0';
    return bin;
}

void assembly2binary(AssemblyCode codeLine){
    Instruction inst;
    if(codeLine->kind == instr){
        inst = codeLine->line.instruction;
        switch(inst.format){
        case formatR:
            fprintf(listing,"ram[%d] = %s %s %s %s 00000 %s",codeLine->lineno,
                                                             opcodeBins[inst.opcode],
                                                             regBins[inst.reg2],
                                                             regBins[inst.reg3],
                                                             regBins[inst.reg1],
                                                             functBins[inst.opcode]);
            fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            break;
        case formatJ:
            if(inst.opcode == jst){
                fprintf(listing,"ram[%d] = %s 00000000000000000000000000",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = %s %s",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           getImediate(inst.imed,26));
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatI:
            if(inst.opcode == sti || inst.opcode == ldi){
                fprintf(listing,"ram[%d] = %s 00000 %s %s",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           regBins[inst.reg1],
                                                           getImediate(inst.imed,16));
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = %s %s %s %s",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           regBins[inst.reg2],
                                                           regBins[inst.reg1],
                                                           getImediate(inst.imed,16));
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatSYS:
            if(inst.opcode == hlt){
                fprintf(listing,"ram[%d] = %s 00000000000000000000000000",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = %s 00000 %s 0000000000000000",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           regBins[inst.reg2]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        }
    }else{

    }
}

void generateBinary () {
    AssemblyCode a = getAssembly();
    char *bin;

    if(PrintCode)
        printf(N_AZ "\nCódigo Binário:\n" RESET);
    while (a != NULL && listing != NULL) {
        assembly2binary(a);
        a = a->next;
    }
}