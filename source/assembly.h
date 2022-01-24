typedef enum {
    formatR,
    formatI,
    formatSYS,
    format4,
    formatJ
} InstrFormat;

typedef enum {
    instr,
    lbl
} LineKind;

typedef enum {
    simple, // Variável simples
    vector, // Vetor
    address // Ponteiro para vetor
} VarKind;

typedef enum { $zero, $r1, $r2, $r3, $r4, $r5, $r6, $r7, $r8, $r9, $r10, $r11, $r12, $r13, $r14, $r15,
                $r16, $r17, $r18, $r19, $p1, $p2, $p3, $p4, $p5, $p6, $p7, $p8, $p9, $p10, $ret, $lp, none } Reg;

typedef enum {
    add, sub, mult, divi, and, or , nand, nor, sle, slt, sge, addi, subi, divim, multi, andi, ori, nori, slei, slti, beq,
    bne, blt, bgt, sti, ldi, str, ldr, hlt, in, out, jmp, jal, jst, lstk, sstk, mov, put, ctso, nop
} InstrKind;

typedef struct {
    InstrFormat format; // Formato da instrução
    InstrKind opcode; // Código de identificação
    Reg reg1;
    Reg reg2;
    Reg reg3;
    int imed;
    char *label;
} Instruction;

typedef struct AssemblyCodeRec {
    int lineno;
    LineKind kind;
    union {
        Instruction instruction;
        char *label;
    } line;
    struct AssemblyCodeRec *next;
} *AssemblyCode;

typedef struct VarListRec {
    char *id;
    int size;
    int memloc;
    VarKind kind;
    struct VarListRec *next;
} *VarList;

typedef struct FunListRec {
    char *id;
    int size;
    VarList vars;
    struct FunListRec *next;
} *FunList;

/**
 * @brief Gera código lista com as instruções do código assembly.
 * 
 * @param head Topo da lista de instruções
 */
void generateAssembly(QuadList head);

/**
 * @brief Get the Assembly object
 * 
 * @return AssemblyCode 
 */
AssemblyCode getAssembly();

void printAssembly();

int getSize();