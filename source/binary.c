#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"
#include <string.h>

#define END_SWITCH 67

const char *Prefixos[] = { "add" , "sub" , "mult", "div"  , "and" , "or" , "nand", "nor" , "sle" , "slt", "sge",
                           "addi", "subi", "divi", "multi", "andi", "ori", "nori", "slei", "slti", "beq", "bne",
                           "blt" , "bgt" , "sti" , "ldi"  , "str" , "ldr", "hlt" , "in"  , "out" , "jmp", "jal",
                           "jst" , "lstk", "sstk", "mov"  , "put" , "ctso", "nop" };

const char *opcodeBins[] =   {"000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", 
                              "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010", "001011",
                              "001100", "001101", "001110", "001111", "010000", "010001", "010010", "010011", "010100", "010101", "010110",
                              "010111", "011100", "011101"};

const char *functBins[] = { "000000", "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010" };

void assembly2binary(AssemblyCode codeLine){
    Instruction inst;
    if(codeLine->kind == instr){
        inst = codeLine->line.instruction;
        switch(inst.format){
        case formatR:
            if (inst.opcode == nop) {
                fprintf(listing,"ram[%d] = {32'd0};", codeLine->lineno);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
                break;
            }

            fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 5'd%d, 5'd0, 6'b%s};",codeLine->lineno,
                                                             opcodeBins[inst.opcode],
                                                             inst.reg2,
                                                             inst.reg3,
                                                             inst.reg1,
                                                             functBins[inst.opcode]);
            fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            break;
        case formatJ:
            if(inst.opcode == jst){
                fprintf(listing,"ram[%d] = {6'b%s, 26'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = {6'b%s, 26'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatI:
            if(inst.opcode == sti || inst.opcode == ldi){
                fprintf(listing,"ram[%d] = {6'b%s, 5'd0, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == bgt || inst.opcode == blt){
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.reg2,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == mov || inst.opcode == put){
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[addi],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatSYS:
            if(inst.opcode == hlt) {
                fprintf(listing,"ram[%d] = {6'b%s, 26'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == ctso){
                fprintf(listing,"ram[%d] = {6'b%s, 26'd%d};",codeLine->lineno,
                                                           opcodeBins[jal],END_SWITCH);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
                fprintf(listing,"ram[%d] = {6'b%s, 5'd0, 5'd%d, 16'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg2);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        }
    }else{
        fprintf(listing,"// %s\n",codeLine->line.label);
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
    PrintCode = FALSE;
}