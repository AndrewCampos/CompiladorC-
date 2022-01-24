#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"

#define QUANTUM 20

const char *InstrNames[] = {"add", "sub", "mult", "div", "and", "or", "nand", "nor", "sle", "slt", "sge", "addi", "subi", "divi", "multi", "andi", "ori",
                            "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "sti", "ldi", "str", "ldr", "hlt", "in", "out", "jmp", "jal", "jst",
                            "lstk", "sstk", "mov", "put", "ctso", "nop"};

const char *regNames[] = {"$zero", "$r1", "$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15",
                          "$r16", "$r17", "$r18", "$r19", "$p1", "$p2", "$p3", "$p4", "$p5", "$p6", "$p7", "$p8", "$p9", "$p10", "$ret", "$lp", ""};

AssemblyCode codehead = NULL;
FunList funlisthead = NULL;

int line;
int nscopes = 0;
int curparam = 0;
int curarg = 0;
int narg = 0;
int jmpmain = 0;
int switch_SO = 0;

void insertFun(char *id) {
    FunList new = (FunList) malloc(sizeof(struct FunListRec));
    new->id = (char *)malloc(strlen(id) * sizeof(char));
    strcpy(new->id, id);
    new->size = 0;
    new->next = NULL;
    if (funlisthead == NULL) {
        funlisthead = new;
    
    } else {
        FunList f = funlisthead;
        while (f->next != NULL) {
            f = f->next;
        }
        f->next = new;
    }
    nscopes++;
}

void insertVar(char *scope, char *id, int size, VarKind kind) {
    FunList f = funlisthead;
    if (scope == NULL) {
        if (kind == 1) {
            scope = f->id;
        } else {
            scope = f->next->id;
        }
    }

    while (f != NULL && strcmp(f->id, scope) != 0){
        f = f->next;
    }

    if (f == NULL) {
        insertFun(scope);
        f = funlisthead;

        while (f != NULL && strcmp(f->id, scope) != 0) {
            f = f->next;
        }
    }

    VarList new = (VarList)malloc(sizeof(struct VarListRec));
    new->id = (char *)malloc(strlen(id) * sizeof(char));
    strcpy(new->id, id);
    new->size = size;
    new->memloc = getMemLoc(id, scope);
    new->kind = kind;
    new->next = NULL;

    if (f->vars == NULL) {
        f->vars = new;

    } else {
        VarList v = f->vars;
        while (v->next != NULL) {
            v = v->next;
        }
        v->next = new;
    }

    f->size = f->size + size;
}

VarKind checkType(QuadList l) {
    QuadList aux = l;
    Quad q = aux->quad;
    aux = aux->next;

    while (aux != NULL && aux->quad.op != opEND) {
        if (aux->quad.op == opVEC && strcmp(aux->quad.addr2.contents.var.name, q.addr1.contents.var.name) == 0) {
            return address;
        }
        aux = aux->next;
    }
    return simple;
}

void insertLabel(char *label) {
    AssemblyCode new = (AssemblyCode)malloc(sizeof(struct AssemblyCodeRec));
    new->lineno = line;
    new->kind = lbl;
    new->line.label = (char *)malloc(strlen(label) * sizeof(char));
    strcpy(new->line.label, label);
    new->next = NULL;

    if (codehead == NULL) {
        codehead = new;
    
    } else {
        AssemblyCode a = codehead;
        while (a->next != NULL) {
            a = a->next;
        }
        a->next = new;
    }
}

void insertInstruction(InstrFormat format, InstrKind opcode, Reg reg1, Reg reg2, Reg reg3, int imed, char *label) {
    Instruction i;
    switch_SO++;

    if (opcode == ctso) {
        switch_SO = 0;
    }
    
    i.format = format;
    i.opcode = opcode;
    i.reg1 = reg1;
    i.reg2 = reg2;
    i.reg3 = reg3;
    i.imed = imed;

    if (label != NULL) {
        i.label = (char *)malloc(strlen(label) * sizeof(char));
        strcpy(i.label, label);
    }

    AssemblyCode new = (AssemblyCode)malloc(sizeof(struct AssemblyCodeRec));
    new->lineno = line;
    new->kind = instr;
    new->line.instruction = i;
    new->next = NULL;

    if (codehead == NULL) {
        codehead = new;
    
    } else {
        AssemblyCode a = codehead;
        while (a->next != NULL) {
            a = a->next;
        }
        a->next = new;
    }
    line++;
}

void instructionR(InstrKind opcode, Reg rf, Reg r1, Reg r2) {
    insertInstruction(formatR, opcode, rf, r1, r2, 0, NULL);
}

void instructionI(InstrKind opcode, Reg rf, Reg r1, int imed, char *label) {
    insertInstruction(formatI, opcode, rf, r1, $zero, imed, label);
}

void instructionSYS(InstrKind opcode, Reg rf) {
    insertInstruction(formatSYS, opcode, $zero, rf, $zero, 0, NULL);
}

void instructionJ(InstrKind opcode, int im, char *imlbl) {
    insertInstruction(formatJ, opcode, $zero, $zero, $zero, im, imlbl);
}

Reg getParamReg() {
    return (Reg) nregtemp + curparam;
}

Reg getArgReg() {
    return (Reg) nregtemp + curarg;
}

Reg getReg(char *regName) {
    for (int i = 0; i < nregisters; i++) {
        if (strcmp(regName, regNames[i]) == 0) {
            return (Reg) i;
        }
    }
    return $zero;
}

int getLabelLine(char *label) {
    AssemblyCode a = codehead;
    while (a->next != NULL) {

        if (a->kind == lbl && strcmp(a->line.label, label) == 0) {
            return a->lineno;
        }
        a = a->next;
    }
    return -1;
}

VarKind getVarKind(char *id, char *scope) {
    FunList f = funlisthead;

    while (f != NULL && strcmp(f->id, scope) != 0) {
        f = f->next;
    }

    if (f == NULL) {
        return simple;
    }

    VarList v = f->vars;
    while (v != NULL) {
        if (strcmp(v->id, id) == 0) {
            return v->kind;
        }
        v = v->next;
    }
    return simple;
}

int getFunSize(char *id) {
    FunList f = funlisthead;
    while (f != NULL && strcmp(f->id, id) != 0) {
        f = f->next;
    }

    if (f == NULL) {
        return -1;
    }
    return f->size;
}

void initCode(QuadList head) {
    instructionI(addi, $lp, $zero, lploc, NULL);
    insertFun("global");
}

void generateInstruction(QuadList l) {
    Quad q;
    Address a1, a2, a3;
    int aux;
    VarKind v;

    while (l != NULL) {
        q = l->quad;
        a1 = q.addr1;
        a2 = q.addr2;
        a3 = q.addr3;
        FunList g = funlisthead;

        switch (q.op) {

        case opMOV:
            instructionI(mov, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
            break;

        case opPUT:
            instructionI(put, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
            break;

        case opADD:
            instructionR(add, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opSUB:
            instructionR(sub, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opMULT:
            instructionR(mult, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opDIV:
            instructionR(divi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opSLE:
            instructionR(sle, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opSGE:
            instructionR(sge, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opAND:
            instructionR(and, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opOR:
            instructionR(or, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
            break;

        case opBGT:
            instructionI(bgt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
            break;

        case opBLT:
            instructionI(blt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
            break;

        case opBEQ:
            instructionI(beq, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
            break;

        case opBNE:
            instructionI(bne, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
            break;

        case opATRIB:
            instructionI(mov, getReg(a1.contents.var.name), getReg(a2.contents.var.name), 0, NULL);
            break;

        case opALLOC:
            if (a2.contents.val == 1) {
                insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, simple);
            
            } else {
                insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, vector);
            }
            break;

        case opADDI:
            instructionI(addi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
            break;

        case opLOAD:
            aux = getMemLoc(a2.contents.var.name, a2.contents.var.scope);
            if (aux == -1) { // caso a variável for global
                aux = getMemLoc(a2.contents.var.name, "global");
                instructionI(ldi, getReg(a1.contents.var.name), none, aux, NULL);
            
            } else{
                instructionI(ldr, getReg(a1.contents.var.name), $lp, aux, NULL);
            }
            break;

        case opVEC:
            if (getVarKind(a2.contents.var.name, a2.contents.var.scope) == address) {
                instructionI(ldr, getReg(a1.contents.var.name), $lp, getMemLoc(a2.contents.var.name, a2.contents.var.scope), NULL);
                instructionR(add, getReg(a3.contents.var.name), getReg(a3.contents.var.name), getReg(a1.contents.var.name));
                instructionI(ldr, getReg(a1.contents.var.name), getReg(a3.contents.var.name), 0, NULL);
            
            } else {
                aux = getMemLoc(a2.contents.var.name, a2.contents.var.scope);
                if (aux == -1) { // caso a variável for global
                    aux = getMemLoc(a2.contents.var.name, "global");
                    instructionI(ldr, getReg(a1.contents.var.name), getReg(a3.contents.var.name), aux, NULL);
                
                } else {
                    instructionI(ldr, getReg(a1.contents.var.name), getReg(a3.contents.var.name), aux, NULL);
                }
            }
            break;

        case opSTORE:
            aux = getMemLoc(a2.contents.var.name, a2.contents.var.scope);
            if (aux == -1) { // caso seja uma variavel global
                aux = getMemLoc(a2.contents.var.name, "global");

                if (a3.kind == Empty) { // caso não seja um vetor global
                    instructionI(sti, getReg(a1.contents.var.name), none, aux, NULL);
                
                } else if (a3.kind == IntConst) {
                    aux += a3.contents.val - 1;
                    instructionI(sti, getReg(a1.contents.var.name), none, aux, NULL);
                
                } else {
                    instructionI(str, getReg(a1.contents.var.name), getReg(a3.contents.var.name), aux, NULL);
                }
            
            } else if (a3.kind == Empty) { // caso não seja um vetor local
                instructionI(str, getReg(a1.contents.var.name), $lp, aux, NULL);
            
            } else if (a3.kind == IntConst) { // caso seja um vetor local
                aux += a3.contents.val - 1;
                instructionI(str, getReg(a1.contents.var.name), $lp, aux, NULL);
            } else {
                instructionR(add, getReg(a3.contents.var.name), getReg(a3.contents.var.name), $lp);
                instructionI(str, getReg(a1.contents.var.name), getReg(a3.contents.var.name), aux, NULL);
            }

            if (switch_SO > QUANTUM && !SO) {
                instructionSYS(ctso, none);
            }
            break;

        case opGOTO:
            instructionJ(jmp, -1, a1.contents.var.name);
            break;

        case opRET:
            if (a1.kind == String) {
                instructionI(mov, $ret, getReg(a1.contents.var.name), 0, NULL);
            } else{
                instructionI(mov, $ret, $zero, a1.contents.val, NULL);
            }

            if (strcmp(a1.contents.var.scope, "main") != 0) {
                instructionJ(jst, 0, NULL);
            
            } else {
                instructionJ(jmp, -1, "end");
            }
            break;

        case opFUN:
            if (jmpmain == 0) {
                instructionJ(jmp, -1, "main");
                jmpmain = 1;
            }

            insertLabel(a1.contents.var.name);
            insertFun(a1.contents.var.name);
            curarg = 0;
            break;

        case opEND:
            if (strcmp(a1.contents.var.name, "main") == 0) {
                instructionJ(jmp, -1, "end");
            
            } else {
                instructionJ(jst, 0, NULL);
            }
            break;

        case opPARAM:
            instructionI(mov, getParamReg(), getReg(a1.contents.var.name), 0, NULL);
            curparam = (curparam + 1) % nregparam;
            break;

        case opCALL:
            if (strcmp(a2.contents.var.name, "input") == 0) {
                instructionSYS(in, $ret);
            
            } else if (strcmp(a2.contents.var.name, "output") == 0) {
                instructionSYS(out, $p1);
            
            } else if (strcmp(a2.contents.var.name, "loadStack") == 0) {
                instructionSYS(lstk, $p1);
            
            } else if (strcmp(a2.contents.var.name, "saveStack") == 0) {
                instructionSYS(sstk, $p1);

            } else if (strcmp(a2.contents.var.name, "load") == 0) {
                instructionI(ldr, $ret, $p1, 0, NULL);
            
            } else if (strcmp(a2.contents.var.name, "store") == 0) {
                instructionI(str, $p2, $p1, 0, NULL);

            } else if (strcmp(a2.contents.var.name, "nop") == 0) {
                instructionR(nop, $zero, $zero, $zero);
            
            } else {
                aux = getFunSize(a2.contents.var.scope);
                instructionI(addi, $lp, $lp, aux, NULL);
                instructionJ(jal, -1, a2.contents.var.name);
                instructionI(subi, $lp, $lp, aux, NULL);
            }

            narg = a3.contents.val;
            curparam = 0;
            break;

        case opARG:
            insertVar(a3.contents.var.scope, a1.contents.var.name, 1, checkType(l));
            FunList f = funlisthead;
            instructionI(str, getArgReg(), $lp, getMemLoc(a1.contents.var.name, a1.contents.var.scope), NULL);
            curarg++;
            break;

        case opLABEL:
            insertLabel(a1.contents.var.name);
            break;

        case opHLT:
            insertLabel("end");
            instructionSYS(hlt, none);
            break;

        default:
            break;
        }
        l = l->next;
    }
}

void createInstructions(QuadList head) {
    QuadList l = head;
    generateInstruction(l);
    AssemblyCode a = codehead;

    while (a != NULL) {
        if (a->kind == instr)  {
            switch (a->line.instruction.opcode) { // Atualiza labels de desvios

            case jmp:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            case bne:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            case beq:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            case blt:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            case bgt:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            case jal:
                a->line.instruction.imed = getLabelLine(a->line.instruction.label);
                break;

            default:
                break;
            }
        }
        a = a->next;
    }
}

void printAssembly() {
    AssemblyCode a = codehead;
    if (PrintCode) {
        printf(N_AZ "\nC- Assembly Code\n" RESET);
    }

    while (a != NULL) {
        if (a->kind == instr) {
            switch (a->line.instruction.format) {

            case formatR:
                fprintf(listing, "%s %s %s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], regNames[a->line.instruction.reg3]);
                break;

            case formatI:
                if (a->line.instruction.opcode == sti || a->line.instruction.opcode == ldi) {
                    fprintf(listing, "%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                
                } else if (a->line.instruction.opcode == put) {
                    fprintf(listing, "%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                
                } else {
                    fprintf(listing, "%s %s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], a->line.instruction.imed);
                }
                break;

            case formatSYS:
                if (a->line.instruction.opcode == hlt || a->line.instruction.opcode == ctso) {
                    fprintf(listing, "%s;\n", InstrNames[a->line.instruction.opcode]);
                
                } else if (a->line.instruction.opcode == in || a->line.instruction.opcode == out || a->line.instruction.opcode == sstk || a->line.instruction.opcode == lstk) {
                    fprintf(listing, "%s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg2]);
                
                } else {
                    fprintf(listing, "%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                }
                break;

            case formatJ:
                if (a->line.instruction.opcode == jst) {
                    fprintf(listing, "%s;\n", InstrNames[a->line.instruction.opcode]);

                } else {
                    fprintf(listing, "%s %d;\n", InstrNames[a->line.instruction.opcode], a->line.instruction.imed);
                }
                break;

            default:
                break;
            }
        
        } else {
            fprintf(listing, "//%s\n", a->line.label);
        }
        a = a->next;
    }

    if (PrintCode) {
        printf(N_VERD "Código assembly criado com sucesso!\n");
    }
}

/* Função inicial da geração de código assembly */
void generateAssembly(QuadList head) {
    line = init_code;
    initCode(head);
    createInstructions(head);
    if (PrintCode) {
        printAssembly();
    }
}

AssemblyCode getAssembly() {
    return codehead;
}

int getSize() {
    return line - 1;
}