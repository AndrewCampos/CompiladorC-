#include "globals.h"
#include "util.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>

#define INDIF -1

static int location = iniDataMem; // Contador para alocação de endereços de memória.
char *escopo = "global"; // Escopo atual da análise.
int check_return = FALSE;

void UpdateScope(TreeNode *t) {
    if (t != NULL && t->kind.exp == FunDeclK) {
        escopo = t->attr.name;
        if (getFunType(escopo) == INTTYPE && check_return == TRUE) {
            if (checkReturn(escopo) == -1) {
                printError(composeString("Retorno da função %s inexistente.",t->attr.name), Semantics, t->lineno);
                Error = TRUE;
            }
        }
    }
}

/**
 * Percorre a árvore sintática recursivamente tanto em pré quanto em pós ordem.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 * - preProc: Ponteiro para função de percorrimento em pré-ordem.
 * - postProc: Ponteiro para função de percorrimento em pós-ordem.
 */
static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *)) {
    if (t != NULL) {
        UpdateScope(t);
        preProc(t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
                traverse(t->child[i], preProc, postProc);
        }
        if (t->child[0] != NULL && t->kind.exp == FunDeclK) {
            escopo = "global";
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

/**
 * Procedimento do tipo do-nothing para o percorrimento em pré-ordem ou pós-ordem da árvore.
 *
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 */
static void nullProc(TreeNode *t) {
    if (t == NULL) {
        return;
    } else {
        return;
    }
}

/**
 * Confere se o nó se trata de uma função reservada do sistema.
 * 
 * Parâmetros:
 * t: Nó atual da árvore sintática a ser analisado.
 * 
 * Retorna:
 * 1 caso seja uma função reservada, 0 caso contrário.
 */
int isReservedFunction(TreeNode *t) {
    return (strcmp(t->attr.name, "input") == 0)
        || (strcmp(t->attr.name, "output") == 0)
        || (strcmp(t->attr.name, "sysWake") == 0)
        || (strcmp(t->attr.name, "sysSleep") == 0)
        || (strcmp(t->attr.name, "loadStack") == 0)
        || (strcmp(t->attr.name, "saveStack") == 0);
}

/**
 * Insere identificadores armazenados no nó referenciado na tabela de símbolos.
 *
 * Parâmetro:
 * - t: Nó atual da árvore sintática a ser analisado. 
 */
static void insertNode(TreeNode *t) {
    dataTypes TIPO = NULLL;

    switch (t->nodekind) {
    case StmtK:

        switch (t->kind.stmt) {
        case ReturnVOID:
            if (getFunType(escopo) == INTTYPE) {
                printError("Retorno da função incompatível.", Semantics, t->lineno);
                Error = TRUE;
            }
            st_insert("return", t->lineno, 0, escopo, INTTYPE, NULLL, RETT, t->vet);
            break;

        case ReturnINT:
            if (getFunType(escopo) == VOIDTYPE) {
                printError("Retorno da função incompatível.", Semantics, t->lineno);
                Error = TRUE;
            }
            st_insert("return", t->lineno, 0, escopo, INTTYPE, NULLL, RETT, t->vet);
            break;

        default:
            break;
        }
        break;

    case ExpK:

        switch (t->kind.exp) {
        case VarDeclK:
            st_insert(t->attr.name, t->lineno, location++, escopo, INTTYPE, TIPO, VAR, t->vet);
            if (st_lookup(t->attr.name, escopo) == -1)
                /* não encontrado na tabela, inserir*/
                st_insert(t->attr.name, t->lineno, location++, escopo, INTTYPE, TIPO, VAR, t->vet);
            break;

        case VetorK:
            st_insert(t->attr.name, t->lineno, location++, escopo, INTTYPE, TIPO, VET, t->vet);
            location += t->child[1]->attr.val - 1;
            break;

        case FunDeclK:
            location = 1;
            if (strcmp(t->child[1]->attr.name, "VOID") == 0) {
                TIPO = VOIDTYPE;
            } else {
                TIPO = INTTYPE;
            }

            if (st_lookup(t->attr.name, escopo) == -1) {
                /* não encontrado na tabela, inserir*/
                st_insert(t->attr.name, t->lineno, INDIF, "global", t->type, TIPO, FUN, t->vet);
            } else {
                /* encontrado na tabela, erro semântico */
                printError(composeString("Múltiplas declarações da função \"%s\".", t->attr.name), Semantics, t->lineno);
                Error = TRUE;
            }
            break;

        case VarParamK:
            st_insert(t->attr.name, t->lineno, location++, escopo, INTTYPE, TIPO, PVAR, t->vet);
            break;

        case VetParamK:
            st_insert(t->attr.name, t->lineno, location++, escopo, INTTYPE, TIPO, PVET, t->vet);
            break;

        case IdK:
            if (t->add != 1) {
                if (st_lookup(t->attr.name, escopo) == -1) {
                    printError(composeString("A variável \"%s\" não foi declarada.", t->attr.name), Semantics, t->lineno);
                    Error = TRUE;
                } else {
                    if (t->child[0] != NULL) {
                        st_insert(t->attr.name, t->lineno, 0, escopo, INTTYPE, TIPO, VET, t->vet);
                    } else {
                        st_insert(t->attr.name, t->lineno, 0, escopo, INTTYPE, TIPO, VAR, t->vet);
                    }
                }
            }
            break;

        case AtivK:
            if (st_lookup(t->attr.name, escopo) == -1 && !isReservedFunction(t)) {
                printError(composeString("A função \"%s\" não foi declarada.", t->attr.name), Semantics, t->lineno);
                Error = TRUE;
            } else {
                if (t->params == getNumParam(t->attr.name) || isReservedFunction(t)) {
                    st_insert(t->attr.name, t->lineno, INDIF, escopo, getFunType(t->attr.name), TIPO, CALL, t->vet);
                } else {
                    printError(composeString("Número de parâmetros para a função \"%s\" incompatível.", t->attr.name),
                        Semantics, t->lineno);
                    Error = TRUE;
                }
            }
            break;
            
        default:
            break;
        }
        break;

    default:
        break;
    }
}

void buildSymtab(TreeNode *syntaxTree) {
    traverse(syntaxTree, insertNode, nullProc);
    busca_main();
    printSucess("Tabela de símbolos construida.", HeaderOnly);

    if (TraceAnalyze) {
        printf(AZ "Checando Tipos...\n" RESET);
    }

    check_return = TRUE;
    typeCheck(syntaxTree);

    if (TraceAnalyze && Error != TRUE) {
        printf(N_AZ "                               Tabela de Simbolos:\n" RESET);
        printSymTab(listing);
    }
}

/**
 * Checa se não ocorre uma atribuição do retorno de uma função do tipo VOID.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado
 * 
 * Retorna:
 * 1 Caso exista uma atribuição do retorno tipo VOID, 0 caso contrário.
 */
int isInvalidTypes(TreeNode *child) {
    return child->kind.exp == AtivK && getFunType(child->attr.name) == VOIDTYPE;
}

/**
 * Verifica se o retorno de uma função é compatível com a operação realizada.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado
 * 
 * Retorna:
 * 1 caso o retorno seja incompatível, 0 caso contrário.
 */
int isInvalidReturn(TreeNode *t) {
    TreeNode *child_1 = t->child[1];
    return isInvalidTypes(t->child[0]) || isInvalidTypes(t->child[1]);
}

void checkNode(TreeNode *t) {
    switch (t->nodekind) {
    case ExpK:

        switch (t->kind.exp) {
        case OpK:
            if ((t->child[0] == NULL) || (t->child[1] == NULL)) {
                break;
            }

            if (isInvalidReturn(t)) {
                printError("Uma função com retorno VOID não pode ser um operando.", Semantics, t->lineno);
                Error = TRUE;
            }
            break;

        case AtivK:
            if (((t->params > 0) && (getFunStmt(t->attr.name)) == VOIDTYPE)) {
                printError("Inserção de parâmetros em uma função do tipo VOID.", Semantics, t->lineno);
                Error = TRUE;
            }
            break;

        default:
            break;
        }
        break;

    case StmtK:
        switch (t->kind.stmt) {
        case AssignK:
            if ((t->child[1] == NULL)) {
                break;
            }

            if (isInvalidTypes(t->child[1])) {
                printError("Uma função com retorno VOID não pode ser atribuída a uma variavel.", Semantics, t->lineno);
                Error = TRUE;
            }
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void typeCheck(TreeNode *syntaxTree) {
    traverse(syntaxTree, checkNode, nullProc);
}