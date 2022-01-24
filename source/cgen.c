#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "parse.h"
#include "analyze.h"
#include "util.h"

/* tmpOffset is the memory offset for temps
   It is decremented each time a temp is
   stored, and incremeted when loaded again */
static int tmpOffset = 0;

/* prototype for internal recursive code generator */
static void cGen(TreeNode *tree);

QuadList head = NULL;

int location = 0;
int mainLocation;
int memLoc;

int nlabel = 0;
int ntemp = 1;
int nparams = -1;

Address aux;
Address var;
Address offset;
Address empty;

FlagType Imediato = FALSE;

char var_escopo[30] = "global";

const char *OpKindNames[] = {"add", "sub", "mult", "div", "blt", "lequal", "bgt", "grequal", "beq", "bne", "and", "or", "atrib",
                             "alloc", "addi", "subi", "load", "store", "vec", "goto", "iff", "ret", "fun", "end",
                             "param", "call", "arg", "label", "hlt", "mov", "put"};

void emitComment(char *c) {
    if (TraceCode) {
        fprintf(listing, "// %s\n", c);
    }
    printf("%s\n", c);
}

void quad_insert(OpKind op, Address addr1, Address addr2, Address addr3) {
    Quad quad;
    quad.op = op;
    quad.addr1 = addr1;
    quad.addr2 = addr2;
    quad.addr3 = addr3;
    QuadList new = (QuadList)malloc(sizeof(struct QuadListRec));
    new->location = location;
    new->quad = quad;
    new->next = NULL;

    if (head == NULL) {
        head = new;
    
    } else {
        QuadList q = head;

        while (q->next != NULL) {
            q = q->next;
        }

        q->next = new;
    }
    location++;
}

int quad_update(int loc, Address addr1, Address addr2, Address addr3) {
    QuadList q = head;

    while (q != NULL) {

        if (q->location == loc) {
            break;
        }

        q = q->next;
    }

    if (q == NULL) {
        return 0;

    } else {
        
        if (addr1.kind != Empty) {
            q->quad.addr1 = addr1;
        }

        if (addr2.kind != Empty) {
            q->quad.addr2 = addr2;
        }
        
        if (addr3.kind != Empty) {
            q->quad.addr3 = addr3;
        }

        return 1;
    }
}

char *newLabel() {
    char *label = (char *) malloc((nlabel_size + 3) * sizeof(char));
    sprintf(label, "L%d", nlabel);
    nlabel++;
    return label;
}

char *newTemp() {
    char *temp = (char *) malloc((ntemp_size + 3) * sizeof(char));
    sprintf(temp, "$r%d", ntemp);
    ntemp = (ntemp % (nregtemp - 1)) + 1;
    return temp;
}

Address addr_createEmpty() {
    Address addr;
    addr.kind = Empty;
    addr.contents.var.name = NULL;
    addr.contents.var.scope = NULL;
    return addr;
}

Address addr_createIntConst(int val) {
    Address addr;
    addr.kind = IntConst;
    addr.contents.val = val;
    return addr;
}

Address addr_createString(char *name, char *scope) {
    Address addr;
    addr.kind = String;
    addr.contents.var.name = (char *) malloc(strlen(name) * sizeof(char));
    strcpy(addr.contents.var.name, name);

    if (scope == NULL) {
        addr.contents.var.scope = (char *)malloc(strlen(name) * sizeof(char));
        strcpy(addr.contents.var.scope, name);
    
    } else {
        addr.contents.var.scope = (char *)malloc(strlen(scope) * sizeof(char));
        strcpy(addr.contents.var.scope, scope);
    }

    return addr;
}

/* O procedimento genStmt() gera código em um nó do tipo declaração */
static void genStmt(TreeNode *tree) {
    TreeNode *p1, *p2, *p3;
    Address addr1, addr2, addr3;
    Address aux1, aux2;
    int loc1, loc2, loc3;
    char *label;
    char *temp;

    switch (tree->kind.stmt) {
    case IfK:
        if (TraceCode) {
            emitComment("-> if");
        }
        
        p1 = tree->child[0]; //arg
        p2 = tree->child[1]; //if true
        p3 = tree->child[2]; //if false
        // condicao if
        cGen(p1);
        addr1 = aux;
        // if false
        loc1 = location - 1;
        // if true
        cGen(p2);
        //goes to end
        loc2 = location;
        quad_insert(opGOTO, empty, empty, empty); //jump else
        // end if
        label = newLabel();
        quad_insert(opLABEL, addr_createString(label, var_escopo), empty, empty);
        // label para fim do if
        quad_update(loc1, empty, empty, addr_createString(label, var_escopo));
        // else
        cGen(p3);

        if (p3 != NULL) {
            // goes to the end
            loc3 = location;
            //quad_insert(opGOTO, empty, empty, empty); //sair else
        }

        label = newLabel();
        // final
        quad_insert(opLABEL, addr_createString(label, var_escopo), empty, empty);
        quad_update(loc2, addr_createString(label, var_escopo), empty, empty);

        if (p3 != NULL) {
            quad_update(loc3, addr_createString(label, var_escopo), empty, empty);
        }

        if (TraceCode) {
            emitComment("<- if");
        }
        break;

    case WhileK:
        if (TraceCode) {
            emitComment("-> while");
        }

        p1 = tree->child[0]; // argumento
        p2 = tree->child[1]; // corpo
        // inicio do while
        label = newLabel();
        quad_insert(opLABEL, addr_createString(label, var_escopo), empty, empty); //cria a label vazia
        // condicao while
        cGen(p1);
        addr1 = aux;
        // caso a condição seja falsa
        loc1 = location - 1;
        // while
        cGen(p2); //body
        loc3 = location;
        quad_insert(opGOTO, addr_createString(label, var_escopo), empty, empty); // refaz o while enquanto é verdadeiro
        // final
        label = newLabel();
        quad_insert(opLABEL, addr_createString(label, var_escopo), empty, empty);
        //if condition is false comes to here
        quad_update(loc1, empty, empty, addr_createString(label, var_escopo)); // atualiza label com a localização do fim do while

        if (TraceCode) {
            emitComment("<- while");
        }
        break;

    case AssignK:
        if (TraceCode) {
            emitComment("-> atrib");
        }

        p1 = tree->child[0]; // var
        p2 = tree->child[1]; // corpo
        // var
        cGen(p1);
        addr1 = aux;
        aux1 = var;
        aux2 = offset;
        // exp
        cGen(p2);
        addr2 = aux;
        quad_insert(opATRIB, addr1, addr2, empty);
        quad_insert(opSTORE, addr1, aux1, aux2);

        if (TraceCode) {
            emitComment("<- atrib");
        }
        break;

    case ReturnINT:
        if (TraceCode) {
            emitComment("-> returnVOID");
        }

        p1 = tree->child[0];
        cGen(p1);
        addr1 = aux;
        quad_insert(opRET, addr1, empty, empty);

        if (TraceCode) {
            emitComment("<- returnINT");
        }
        break;

    case ReturnVOID:
        if (TraceCode) {
            emitComment("-> returnVOID");
        }

        addr1 = empty;
        quad_insert(opRET, empty, empty, empty);

        if (TraceCode) {
            emitComment("<- return");
        }
        break;

    default:
        break;
    }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp(TreeNode *tree) {
    TreeNode *p1, *p2, *p3;
    Address addr1, addr2, addr3;
    int loc1, loc2, loc3;
    char *label;
    char *temp;
    char *s = "";

    switch (tree->kind.exp) {
    case ConstK:
        if (TraceCode) {
            emitComment("-> Const");
        }

        temp = newTemp();
        addr1 = addr_createIntConst(tree->attr.val);
        aux = addr_createString(temp, var_escopo);
        Imediato = TRUE; // teste ................
        quad_insert(opPUT, aux, addr_createString("$zero", var_escopo), addr1);

        if (TraceCode) {
            emitComment("<- Const");
        }
        break;

    case IdK:
        if (TraceCode) {
            emitComment("-> Id");
        }
        aux = addr_createString(tree->attr.name, var_escopo);
        p1 = tree->child[0];

        if (p1 != NULL) { // caso seja um vetor
            temp = newTemp();
            addr1 = addr_createString(temp, var_escopo);
            addr2 = aux;
            cGen(p1);
            quad_insert(opVEC, addr1, addr2, aux);
            var = addr2;
            offset = aux;
            aux = addr1;
        
        } else { // caso não seja vetor
            temp = newTemp();
            addr1 = addr_createString(temp, var_escopo);
            quad_insert(opLOAD, addr1, aux, empty);
            var = aux;
            offset = empty;
            aux = addr1;
        }

        if (TraceCode) {
            emitComment("<- Id");
        }
        break;

    case FunDeclK:
        strcpy(var_escopo, tree->attr.name);

        if (TraceCode){
            emitComment(" -> Fun");
        }

        // if main
        if (strcmp(tree->attr.name, "main") == 0) {
            mainLocation = location;
        }

        if (!isReservedFunction(tree)) {
            quad_insert(opFUN, addr_createString(tree->attr.name, var_escopo), empty, empty);
            // Parametros da função
            p1 = tree->child[1];
            cGen(p1);
            // Corpo da função
            p2 = tree->child[2];
            cGen(p2);
            quad_insert(opEND, addr_createString(tree->attr.name, var_escopo), empty, empty);
            strcpy(var_escopo, "global");
        }

        if (TraceCode) {
            emitComment("<- Fun");
        }
        break;

    case AtivK:
        if (TraceCode) {
            emitComment("-> Call");
        }

        // é um parametro
        nparams = tree->params;
        p1 = tree->child[0];

        while (p1 != NULL) {

            if (p1->kind.exp == IdK) {
                if (getVarType(p1->attr.name, var_escopo) == PVET) {
                    temp = newTemp();
                    aux = addr_createString(temp, var_escopo);
                    quad_insert(opADDI, aux, addr_createString("$lp", var_escopo), addr_createIntConst(getMemLoc(p1->attr.name, var_escopo)));
                
                } else {
                    cGen(p1);
                }
           
            } else {
                cGen(p1);
            }

            quad_insert(opPARAM, aux, empty, empty);
            nparams--;
            p1 = p1->sibling;
        }

        nparams = -1;
        if (getFunType(tree->attr.name) == INTTYPE) {
            aux = addr_createString("$ret", var_escopo);
            quad_insert(opCALL, aux, addr_createString(tree->attr.name, var_escopo), addr_createIntConst(tree->params));
        
        } else {
            if (strcmp(tree->attr.name, "input") == 0 || strcmp(tree->attr.name, "load") == 0) {
                aux = addr_createString("$ret", var_escopo);
            }
            
            quad_insert(opCALL, empty, addr_createString(tree->attr.name, var_escopo), addr_createIntConst(tree->params));
        }

        if (TraceCode) {
            emitComment("<- Call");
        }
        break;

    case VarParamK:
        if (TraceCode) {
            emitComment("-> Param");
        }

        quad_insert(opARG, addr_createString(tree->attr.name, var_escopo), empty, addr_createString(var_escopo, var_escopo));
        
        if (TraceCode) {
            emitComment("<- Param");
        }
        break;

    case VetParamK:
        if (TraceCode) {
            emitComment("-> Param");
            }

        quad_insert(opARG, addr_createString(tree->attr.name, var_escopo), empty, addr_createString(var_escopo, var_escopo));

        if (TraceCode) {
            emitComment("<- Param");
        }
        break;

    case VarDeclK:
        memLoc = getMemLoc(tree->attr.name, var_escopo) + iniDataMem;
        if (TraceCode) {
            emitComment("-> Var ");
        }

        if (memLoc >= 0) {
            quad_insert(opALLOC, addr_createString(tree->attr.name, var_escopo), addr_createIntConst(1), addr_createString(var_escopo, var_escopo));
        
        } else {
            printf(N_VERM "Erro ao alocar a variável '%s'! - memLoc: %d\n" RESET, tree->attr.name, memLoc);
            Error = TRUE;
            return;
        }

        if (TraceCode) {
            emitComment("<- Var");
        }
        break;

    case VetorK:
        memLoc = getMemLoc(tree->attr.name, var_escopo);

        if (TraceCode) {
            emitComment("-> Vet ");
        }

        if (memLoc >= 0) {
            quad_insert(opALLOC, addr_createString(tree->attr.name, var_escopo), addr_createIntConst(tree->child[1]->attr.val), addr_createString(var_escopo, var_escopo));
        
        } else {
            printf(N_VERM "Erro ao alocar o vetor '%s'! - memLoc: %d\n" RESET, tree->attr.name, memLoc);
            Error = TRUE;
            return;
        }

        if (TraceCode) {
            emitComment("<- Vet");
        }
        break;

    case OpK:
        if (TraceCode) {
            emitComment("-> Op");
        }

        p1 = tree->child[0];
        p2 = tree->child[1];
        cGen(p1);
        addr1 = aux;
        cGen(p2);
        addr2 = aux;
        temp = newTemp();
        aux = addr_createString(temp, var_escopo);

        switch (tree->attr.op) {
        case SOM:
            quad_insert(opADD, aux, addr1, addr2);
            break;

        case SUB:
            quad_insert(opSUB, aux, addr1, addr2);
            break;

        case MUL:
            quad_insert(opMULT, aux, addr1, addr2);
            break;

        case DIV:
            quad_insert(opDIV, aux, addr1, addr2);
            break;

        case MENO:
            quad_insert(opSGE, aux, addr1, addr2);
            addr1 = addr_createString(newTemp(), var_escopo);
            addr2 = addr_createIntConst(1);
            quad_insert(opPUT, addr1, addr_createString("$zero", var_escopo), addr2);
            quad_insert(opBEQ, aux, addr1, empty);
            break;

        case MEIG:
            quad_insert(opBGT, addr1, addr2, empty);
            break;

        case MAIO:
            quad_insert(opSLE, aux, addr1, addr2);
            addr1 = addr_createString(newTemp(), var_escopo);
            addr2 = addr_createIntConst(1);
            quad_insert(opPUT, addr1, addr_createString("$zero", var_escopo), addr2);
            quad_insert(opBEQ, aux, addr1, empty);
            break;

        case MAIG:
            quad_insert(opBLT, addr1, addr2, empty);
            break;

        case IGL:
            quad_insert(opBNE, addr1, addr2, empty);
            break;

        case DIF:
            quad_insert(opBEQ, addr1, addr2, empty);
            break;

        default:
            emitComment(N_VERM "Erro ao criar quádrupla!" RESET " Operador desconhecido.");
            break;
        }

        if (TraceCode) {
            emitComment("<- Op");
        }
        break;

    default:
        break;
    }
}

/* Procedimento recursivo que gera o código intermediário pela árvore sintática */
static void cGen(TreeNode *tree) {
    if (TraceCode) {
        emitComment(AMAR "Inicio cGen:" RESET);

        if (tree == NULL) {
            emitComment("NULO");
        }
    }

    if (tree != NULL) {
        switch (tree->nodekind) {
        case StmtK:
            if (TraceCode) {
                emitComment("-> Stmt");
            }

            genStmt(tree);
            break;

        case ExpK:
            if (TraceCode) {
                emitComment("-> Exp");
            }

            genExp(tree);
            break;

        default:
            break;
        }

        if (nparams == -1 || nparams == 0) {
            cGen(tree->sibling);
        }
    }
}

void printCode(QuadList head) {
    QuadList q = head;
    Address a1, a2, a3;

    while (q != NULL) {
        a1 = q->quad.addr1;
        a2 = q->quad.addr2;
        a3 = q->quad.addr3;
        fprintf(listing, "(%s, ", OpKindNames[q->quad.op]);

        switch (a1.kind) {
        case Empty:
            fprintf(listing, "_");
            break;

        case IntConst:
            fprintf(listing, "%d", a1.contents.val);
            break;

        case String:
            fprintf(listing, "%s", a1.contents.var.name);
            break;

        default:
            break;
        }

        fprintf(listing, ", ");

        switch (a2.kind) {
        case Empty:
            fprintf(listing, "_");
            break;

        case IntConst:
            fprintf(listing, "%d", a2.contents.val);
            break;

        case String:
            fprintf(listing, "%s", a2.contents.var.name);
            break;

        default:
            break;
        }

        fprintf(listing, ", ");

        switch (a3.kind) {
        case Empty:
            fprintf(listing, "_");
            break;

        case IntConst:
            fprintf(listing, "%d", a3.contents.val);
            break;

        case String:
            fprintf(listing, "%s", a3.contents.var.name);
            break;

        default:
            break;
        }

        fprintf(listing, ")\n");
        q = q->next;
    }
}

/* Procedimento que percorre a arvore sintática a fim de criar o código intermediário */
void codeGen(TreeNode *syntaxTree) {
    empty = addr_createEmpty();
    cGen(syntaxTree);
    quad_insert(opHLT, empty, empty, empty);

    if (PrintCode) {
        printf(N_AZ "\nCódigo Intermediário:\n" RESET);
        printCode(head);
        fprintf(listing, N_VERD "Código intermediário criado com sucesso!\n\n" RESET);
    }
}

QuadList getIntermediate() {
    return head;
}