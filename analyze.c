#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>

/* counter for variable memory locations */
static int location = 0;
char* escopo = "global";
int check_return = FALSE;

void UpdateScope(TreeNode * t){
  if (t != NULL && t->kind.exp == FunDeclK){
    escopo = t->attr.name;
    if(getFunType(escopo) == INTTYPE && check_return == TRUE){
      if(checkReturn(escopo) == -1)
        printf(VERMELHO"[%d} Erro Semantico!"BRANCO" Retorno da funcao '%s' inexistente.\n",t->lineno,escopo);
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
      case AssignK:
        if (st_lookup(t->attr.name, escopo) == -1){
          /* não encontrado na tabela, variavel não declarada */
            //fprintf(listing,"Erro Semantico: A variavel '%s' não foi declarada. [%d]\n", t->attr.name, t->lineno);
            Error = TRUE;
          }
        break;
      case ReturnVOID:
        if(getFunType(escopo) == INTTYPE)
          printf(VERMELHO"[%d] Erro Semantico!"BRANCO" Retorno da função '%s' incompatível.\n",t->lineno,escopo);
        st_insert("return",t->lineno,location++,escopo,INTTYPE, NULLL, RETT, t->vet); 
        break;
      case ReturnINT:
        if(getFunType(escopo) == VOIDTYPE)
          printf(VERMELHO"[%d] Erro Semantico!"BRANCO" Retorno da função '%s' incompatível.",t->lineno,escopo);
        st_insert("return",t->lineno,location++,escopo,INTTYPE, NULLL, RETT, t->vet); 
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
        case FunDeclK:
          if(strcmp(t->child[1]->attr.name,"VOID") == 0) TIPO = VOIDTYPE;
          else TIPO = INTTYPE;
          if (st_lookup(t->attr.name,escopo) == -1){
            /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,location++, "global",t->type, TIPO,FUN, t->vet);}
          else
          /* encontrado na tabela, erro semântico */
            fprintf(listing,VERMELHO"[%d] Erro Semantico:"BRANCO" Multiplas declarações da função '%s'.\n",t->lineno,t->attr.name);
          break;
        case ParamK:
              st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VAR, t->vet);
          break;
        case VetorK:
           if (st_lookup(t->attr.name, escopo) == -1)
          /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,location++, escopo,INTTYPE, TIPO, VAR , t->vet);
          else
          /* encontrado na tabela, verificar escopo */
            st_insert(t->attr.name,t->lineno,0, escopo,INTTYPE, TIPO, VAR, t->vet);
          break;
        case IdK:
          if(t->add != 1){
            if (st_lookup(t->attr.name, escopo) == -1){
              fprintf(listing,VERMELHO"[%d] Erro Semântico!"BRANCO" A variavel '%s' não foi declarada.\n",t->lineno,t->attr.name);
              Error = TRUE;
            }
            else {
              st_insert(t->attr.name,t->lineno,0,escopo,INTTYPE, TIPO,FUN, t->vet);
            }
          }
          break;
        case AtivK:
          //st_insert(t->attr.name,t->lineno,location++,escopo,NULLL,CALL, t->vet);/*se eu coloco 0 dá certo sem o erro do xyz não declarado*/
          if (st_lookup(t->attr.name, escopo) == -1 && strcmp(t->attr.name, "input")!=0 && strcmp(t->attr.name, "output")!=0){
            fprintf(listing,VERMELHO"[%d] Erro Semântico!"BRANCO" A função '%s' não foi declarada.\n",t->lineno,t->attr.name);
            //st_insert(t->attr.name,t->lineno,0,escopo,NULLL,CALL, t->vet);
            Error = TRUE;
          }
          else {
            st_insert(t->attr.name,t->lineno,location++,escopo,/*VOIDTYPE*/getFunType(t->attr.name), TIPO,CALL, t->vet);
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
  printf("\n                               Tabela de Simbolos:\n");
  fprintf(listing,"---------------------------------------------------------------------------------\n");
  printSymTab(listing);
  
}

static void typeError(TreeNode * t, char * message){
  fprintf(listing,"Erro Semantico: %s [%d]\n",message,t->lineno);
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
              typeError(t,VERMELHO"Erro semântico!"BRANCO" Uma funcao com retorno VOID não pode ser um operando.");
        break;
      case AtivK:
        if (((t->params > 0) && (getFunStmt(t->attr.name)) == VOIDTYPE))
              typeError(t,VERMELHO"Erro semântico! "BRANCO"Insercao de parametros a uma função do tipo VOID.");
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
            typeError(t,VERMELHO"Erro semântico! "BRANCO"Uma funcao com retorno VOID não pode ser atribuida a uma variavel.");
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