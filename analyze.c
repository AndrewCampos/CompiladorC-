#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>
#define INDIF -1

/* counter for variable memory locations */
static int location = iniDataMem;
char* escopo = "global";
int check_return = FALSE;

void UpdateScope(TreeNode * t){
  if (t != NULL && t->kind.exp == FunDeclK){
    escopo = t->attr.name;
    if(getFunType(escopo) == INTTYPE && check_return == TRUE){
      if(checkReturn(escopo) == -1){
        printf(N_VERM"[%d] Erro Semântico!"RESET" Retorno da função '%s' inexistente.\n",t->lineno,escopo);
        Error = TRUE;
      }
    }
  }
}

/* Procedure traverse is a generic recursive syntax tree traversal routine: it applies preProc in
preorder and postProc in postorder to tree pointed to by t */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{
  if (t != NULL){ 
    UpdateScope(t);
    preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    if(t->child[0]!= NULL && t->kind.exp == FunDeclK) escopo = "global";
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}
/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t){ 
  if (t==NULL) return;
  else return;
}

/* O procedimento insertNode insere identificadores armazenados em t na tabela de símbolos */
static void insertNode( TreeNode * t) {
  dataTypes TIPO = NULLL;
  switch (t->nodekind){
    case StmtK:
      switch (t->kind.stmt){

      case ReturnVOID:
        if(getFunType(escopo) == INTTYPE){
          printf(N_VERM"[%d] Erro Semântico!"RESET" Retorno da função '%s' incompatível.\n",t->lineno,escopo);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,escopo,INTTYPE, NULLL, RETT, t->vet); 
        break;

      case ReturnINT:
        if(getFunType(escopo) == VOIDTYPE){
          printf(N_VERM"[%d] Erro Semântico!"RESET" Retorno da função '%s' incompatível.\n",t->lineno,escopo);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,escopo,INTTYPE, NULLL, RETT, t->vet); 
        break;
      default:
        break;
      }
      break; 

    case ExpK:
      switch(t->kind.exp){

        case VarDeclK:
          st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VAR, t->vet); 
          if (st_lookup(t->attr.name, escopo) == -1)
          /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,location++, escopo,INTTYPE, TIPO, VAR , t->vet);
          break;

        case VetorK:
          st_insert(t->attr.name,t->lineno,location++, escopo,INTTYPE, TIPO, VET, t->vet);
          location += t->child[1]->attr.val -1;
          break;

        case FunDeclK:
          location = 1;
          if(strcmp(t->child[1]->attr.name,"VOID") == 0) TIPO = VOIDTYPE;
          else TIPO = INTTYPE;
          if (st_lookup(t->attr.name,escopo) == -1){
            /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,INDIF, "global",t->type, TIPO,FUN, t->vet);}
          else{
          /* encontrado na tabela, erro semântico */
            fprintf(listing,N_VERM"[%d] Erro Semântico:"RESET" Múltiplas declarações da função '%s'.\n",t->lineno,t->attr.name);
            Error = TRUE;
          }
          break;

        case VarParamK:
            st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VAR, t->vet);
          break;

        case VetParamK:
            st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VET, t->vet);
          break;

        case IdK:
          if(t->add != 1){
            if (st_lookup(t->attr.name, escopo) == -1){
              fprintf(listing,N_VERM"[%d] Erro Semântico!"RESET" A variável '%s' não foi declarada.\n",t->lineno,t->attr.name);
              Error = TRUE;
            }
            else {
              st_insert(t->attr.name,t->lineno,0,escopo,INTTYPE, TIPO,FUN, t->vet);
            }
          }
          break;

        case AtivK:
          if (st_lookup(t->attr.name, escopo) == -1 && strcmp(t->attr.name, "input")!=0 && strcmp(t->attr.name, "output")!=0){
            fprintf(listing,N_VERM"[%d] Erro Semântico!"RESET" A função '%s' não foi declarada.\n",t->lineno,t->attr.name);
            Error = TRUE;
          }
          else {
            st_insert(t->attr.name,t->lineno,INDIF,escopo,getFunType(t->attr.name), TIPO,CALL, t->vet);
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

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree){
  traverse(syntaxTree,insertNode,nullProc);
  busca_main();
  if (TraceAnalyze) fprintf(listing,AZ"Checando Tipos...\n"RESET);
  check_return = TRUE;
  typeCheck(syntaxTree);
  if(TraceAnalyze && Error != TRUE){
    printf(N_AZ"                               Tabela de Simbolos:\n"RESET);
    printSymTab(listing);
    }
}

static void typeError(TreeNode * t, char * message){
  fprintf(listing,N_VERM "[%d] %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
void checkNode(TreeNode * t){
  switch (t->nodekind){
  case ExpK:
      switch (t->kind.exp){
      case OpK:
        if((t->child[0] == NULL) || (t->child[1] == NULL)) break;
        if (((t->child[0]->kind.exp == AtivK) &&( getFunType(t->child[0]->attr.name)) == VOIDTYPE) ||
            ((t->child[1]->kind.exp == AtivK) && (getFunType(t->child[1]->attr.name) == VOIDTYPE)))
              typeError(t,N_VERM"Erro semântico!"RESET" Uma funcao com retorno VOID não pode ser um operando.");
        break;
      case AtivK:
        if (((t->params > 0) && (getFunStmt(t->attr.name)) == VOIDTYPE))
              typeError(t,N_VERM"Erro semântico! "RESET"Insercao de parametros a uma função do tipo VOID.");
        break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt){
        case AssignK:
          if((t->child[1] == NULL)) break;
          if (t->child[1]->kind.exp == AtivK && getFunType(t->child[1]->attr.name) == VOIDTYPE)
            typeError(t,N_VERM"Erro semântico! "RESET"Uma funcao com retorno VOID não pode ser atribuida a uma variavel.");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree){
  traverse(syntaxTree,checkNode, nullProc);

}