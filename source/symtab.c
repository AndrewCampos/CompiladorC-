#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

typedef struct ParamSymtab {
    char *name;
    int lineno;
    int op;
    char *escopo;
    dataTypes RetType;
    dataTypes StmtType;
    IDTypes IType;
    int vet;
} ParamSymtab;

/**
 * Constrói estrutura de parâmetros com informações do nó atual.
 */
ParamSymtab buildParams(char *name, int lineno, int op, char *escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet) {
    ParamSymtab params;
    params.name = name;
    params.lineno = lineno;
    params.op = op;
    params.escopo = escopo;
    params.RetType = RetType;
    params.StmtType = StmtType;
    params.IType = IType;
    params.vet = vet;
    return params;
}

void insereHash(ParamSymtab params, BucketList list, int hash) {
    if (list == NULL) {
        list = (BucketList)malloc(sizeof(struct BucketListRec));
        list->name = params.name;
        list->lines = (LineList)malloc(sizeof(struct LineListRec));
        list->lines->lineno = lineno;
        list->vet = params.vet;
        list->memloc = params.op;
        list->IType = params.IType;
        list->RetType = params.RetType;
        list->StmtType = params.StmtType;
        list->escopo = params.escopo;
        list->lines->next = NULL;
        list->next = hashTable[hash];
        hashTable[hash] = list;
    } else {
        LineList t = list->lines;

        while (t->next != NULL) {
            t = t->next;
        }
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

/**
 * A função de cálculo do índice de hash.
 * 
 * Parâmetros:
 * - key: Chave para ser convertida em um índice hash.
 * 
 * Retorna:
 * O valor do índice hash calculado.
 */
static int hash(char *key) {
    int temp = 0;
    int i = 0;

    while (key[i] != '\0') {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/**
 * Insere novo nó na tabela de simbolos.
 */
void st_insert(char *name, int lineno, int op, char *escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet) {
    int h = hash(name);
    BucketList l = hashTable[h];
    ParamSymtab params = buildParams(name, lineno, op, escopo, RetType, StmtType, IType, vet);

    if (IType == CALL) {
        while ((l != NULL)) {
            if (((strcmp(name, l->name) == 0))) {
                if (l->IType == CALL) {
                    break;
                }
            }
            l = l->next;
        }

        insereHash(params, l, h);
        return;
    }

    // Procura a ultima declaração com o mesmo nome
    while ((l != NULL) && ((strcmp(name, l->name) != 0))) {
        l = l->next;
    }

    //Para inserir: não achou outra declaração, se achou verificar se o escopo é DIF e não é uma função
    if (l == NULL || (op != 0 && l->escopo != escopo && l->IType != FUN && l->IType != CALL) || IType == RETT) { /* variável não está na tabela ainda */
        if (l != NULL && strcmp(l->name, name) == 0 && strcmp(l->escopo, "global") == 0) {
            fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Variável '%s' já declarada no escopo global.\n", lineno, name);
            Error = TRUE;
        } else {
            l = (BucketList)malloc(sizeof(struct BucketListRec));
            l->name = name;
            l->lines = (LineList)malloc(sizeof(struct LineListRec));
            l->lines->lineno = lineno;
            l->vet = vet;
            if (IType != RETT) {
                l->memloc = op;
            } else {
                l->memloc = -1;
            }
            l->IType = IType;
            l->RetType = RetType;
            l->StmtType = StmtType;
            l->escopo = escopo;
            l->lines->next = NULL;
            l->next = hashTable[h];
            hashTable[h] = l;
            if (IType == PVET || IType == PVAR) {
                insereParam(escopo);
            }
        }
    } else if ((l->IType == FUN && IType == VAR) || (l->IType == CALL && IType == VAR)) {
        fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Nome da variável '%s' já utilizada como nome de função.\n", lineno, name);
        Error = TRUE;
    } else if (l->escopo == escopo && op != 0) {
        fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Variável '%s' já declarada neste escopo.\n", lineno, name);
        Error = TRUE;
    } else if (l->escopo != escopo) {
        while ((l != NULL)) {
            if ((strcmp(l->escopo, "global") == 0) && ((strcmp(name, l->name) == 0))) {
                LineList t = l->lines;
                while (t->next != NULL)
                    t = t->next;
                t->next = (LineList)malloc(sizeof(struct LineListRec));
                t->next->lineno = lineno;
                t->next->next = NULL;
                break;
            }
            l = l->next;
        } if (l == NULL) {
            fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Variável '%s' não declarada neste escopo.\n", lineno, name);
            Error = TRUE;
        }
    } else if (op == 0) {
        LineList t = l->lines;
        if ((l->IType != VAR && l->IType != PVAR && l->IType != PVET) && IType == VAR) {
            fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Vetor '%s' usado como variável.\n", lineno, name);
            Error = TRUE;
            return;
        } else if ((l->IType != VET && l->IType != PVET) && IType == VET) {
            fprintf(listing, N_VERM "[%d] Erro semântico!" RESET " Variável '%s' usada como vetor.\n", lineno, name);
            Error = TRUE;
            return;
        }

        while (t->next != NULL) {
            t = t->next;
        }
        t->next = (LineList)malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
    }
}

int st_lookup(char *name, char *escopo) {
    int h = hash(name);
    BucketList l = hashTable[h];

    while ((l != NULL) && (strcmp(name, l->name) != 0) && (strcmp(escopo, l->escopo) != 0)) {
        l = l->next;
    }
    
    if (l == NULL) {
        return -1;
    } else {
        return 0;
    }
}

void busca_main() {
    int h = hash("main");
    BucketList l = hashTable[h];
    while ((l != NULL) && ((strcmp("main", l->name) != 0 || l->IType == VAR))) {
        l = l->next;
    }

    if (l == NULL) {
        fprintf(listing, N_VERM "     Erro semântico!" RESET " Função main não declarada\n");
        Error = TRUE;
    }
}

dataTypes getFunStmt(char *nome) {
    int h = hash(nome);
    BucketList l = hashTable[h];
    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (l->IType == FUN) {
                break;
            }
        }
        l = l->next;
    }

    if (l == NULL) {
        return -1;
    } else {
        return l->StmtType;
    }
}

int checkReturn(char *escopo) {
    char nome[6] = "return";
    int h = hash(nome);
    BucketList l = hashTable[h];

    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (strcmp(escopo, l->escopo) == 0) {
                return 1;
            }
        }
        l = l->next;
    }
    return -1;
}

dataTypes getFunType(char *nome) {
    int h = hash(nome);
    BucketList l = hashTable[h];

    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (l->IType == FUN) {
                break;
            }
        }
        l = l->next;
    }
    if (l == NULL) {
        return -1;
    } else {
        return l->RetType;
    }
}

int getMemLoc(char *nome, char *escopo) {
    int h = hash(nome);
    BucketList l = hashTable[h];

    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (strcmp(escopo, l->escopo) == 0) {
                break;
            }
        }
        l = l->next;
    }

    if (l == NULL) {
        return -1;
    } else {
        return l->memloc;
    }
}

IDTypes getVarType(char *nome, char *escopo) {
    int h = hash(nome);
    BucketList l = hashTable[h];
    if (nome == NULL) {
        return -1;
    }
    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (strcmp(escopo, l->escopo) == 0) {
                break;
            }
        }
        l = l->next;
    }

    if (l == NULL) {
        return -1;
    } else {
        return l->IType;
    }
}

int getNumParam(char *nome) {
    int h = hash(nome);
    BucketList l = hashTable[h];
    
    if (nome == NULL) {
        return -1;
    }

    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (l->IType == FUN) {
                break;
            }
        }
        l = l->next;
    }

    if (l == NULL) {
        return -1;
    } else {
        return l->tam;
    }
}

void insereParam(char *nome) {
    int h = hash(nome);
    BucketList l = hashTable[h];

    if (nome == NULL) {
        return;
    }
    
    while ((l != NULL)) {
        if (strcmp(nome, l->name) == 0) {
            if (l->IType == FUN) {
                break;
            }
        }
        l = l->next;
    }
    if (l == NULL) {
        return;
    } else {
        l->tam++;
    }
}

void printSymTab(FILE *listing) {
    int i;
    fprintf(listing, "---------------------------------------------------------------------------------\n");
    fprintf(listing, "Nome           Escopo  Tipo ID  Tipo Retorno  Tipo Param  Mem. Loc.  Num da linha\n");
    fprintf(listing, "-------------  ------  -------  ------------  ----------  ---------  ------------\n");

    for (i = 0; i < SIZE; ++i) {
        if (hashTable[i] != NULL) {
            BucketList l = hashTable[i];

            while (l != NULL) {
                LineList t = l->lines;
                fprintf(listing, "%-14s ", l->name);
                fprintf(listing, "%-6s  ", l->escopo);
                char *id, *data;
                switch (l->IType) {
                case RETT:
                    id = "ret";
                    break;
                case VAR:
                    id = "var";
                    break;
                case PVAR:
                    id = "pvar";
                    break;
                case FUN:
                    id = "fun";
                    break;
                case CALL:
                    id = "call";
                    break;
                case VET:
                    id = "vet";
                    break;
                case PVET:
                    id = "pvet";
                    break;
                default:
                    break;
                }

                switch (l->RetType) {
                case INTTYPE:
                    data = "INT";
                    break;
                case VOIDTYPE:
                    data = "VOID";
                    break;
                case NULLL:
                    data = "null";
                    break;
                default:
                    break;
                }
                fprintf(listing, "%-7s  ", id);
                fprintf(listing, "%-12s  ", data);

                switch (l->StmtType) {
                case INTTYPE:
                    data = "INT";
                    break;
                case VOIDTYPE:
                    data = "VOID";
                    break;
                case NULLL:
                    data = "null";
                    break;
                default:
                    break;
                }
                fprintf(listing, "%-10s ", data);
                if (l->memloc >= 0) {
                    fprintf(listing, "    %-3d     ", l->memloc);
                } else {
                    fprintf(listing, "    -       ");
                }

                while (t != NULL) {
                    fprintf(listing, "%3d; ", t->lineno);
                    t = t->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
    }
}