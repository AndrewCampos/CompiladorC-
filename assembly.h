/****************************************************/
/* File:assembly.h                                  */
/* Adapted from:                                    */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#define nregisters 32
#define nregtemp 16
#define nregparam 10

#define sploc 0  // tam 32 
#define gploc 32 // tam 16
#define raloc 47 // tam 16

typedef enum {  nop, halt, add, addi, sub, mult, divi, mod, and, or, not, xor, slt, sgt,
                sle, sge, shl, shr, move,
                ldi, beq, bne, jmp, in, out, str, load, jr } InstrKind;
typedef enum {  format1, format2, format3, format4 } InstrFormat;
typedef enum {  instr, lbl } LineKind;
typedef enum {  simple, vector, address } VarKind;
typedef enum {  $zero, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13, $t14,
                $t15, $a0, $a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $sp, $gp, $ra, $ret, $jmp } Reg;

typedef struct {
    InstrFormat format;
    InstrKind opcode;
    Reg reg1;
    Reg reg2;
    Reg reg3;
    int im;
    char * imlbl;
} Instruction;

typedef struct AssemblyCodeRec {
    int lineno;
    LineKind kind;
    union {
        Instruction instruction;
        char * label;
    } line;
    struct AssemblyCodeRec * next;
} * AssemblyCode;

typedef struct VarListRec {
    char * id;
    int size;
    int memloc;
    VarKind kind;
    struct VarListRec * next;
} * VarList;

typedef struct FunListRec {
    char * id;
    int size;
    int memloc;
    VarList vars;
    struct FunListRec * next;
} * FunList;

void generateAssembly (QuadList head);

AssemblyCode getAssembly ();

int getSize();