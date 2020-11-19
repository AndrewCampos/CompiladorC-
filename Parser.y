%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; /* armazena a arvore sintática para retorno posterior*/
static int yylex(void);
int yyerror(char *message);
char *scope = "";
char * idtype = "";
char * datatype = "";
TreeNode * teste;
static char *savedname = "";
int flag = 0;
int params = 0;
%}

//Alfabeto de Tokens recebidos pelo Parser
%start init
%token IF ELSE WHI RET VOID
%token INT
%token ATR PEV ACH FCH ACO FCO MAIO MENO MAIG MEIG DIF IGL VIRG TB LINE SPACE NL
%token FIM ERR
%token ID NUM 
%left SOM SUB
%left MUL DIV
%token APR FPR
%nonassoc FPR
%nonassoc ELSE

//Para mostrar o valor semântico do token quando for debugar o parser
%printer { fprintf (yyoutput, "’%d’", $$); } NUM

//Expressões Regulares
%%

init:  lista-dec { savedTree = $1; }
;

lista-dec:  lista-dec declaracao {
            YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                   t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
          }| declaracao { $$ = $1; }
;

declaracao:  var-dec { $$ = $1;} | fun-dec { $$ = $1;}
;

var-dec:  tipo ID PEV {
            $$ = newExpNode(VarDeclK);
            $$->attr.name = copyString(id);
            $$->child[0] = $1;
            $$->type = $1->type;
            $$->scope= scope;
            $$->kind.exp = VarDeclK;
            $$->lineno = lineno;
          }
    | tipo fun-id ACO tam FCO PEV {
            $$ = newExpNode(VetorK);
            $$->attr.name = copyString(id);
            $$->child[0] = $1;
            $$->child[1] = $4;
            $$->type = $1->type;
            $$->scope= scope;
            $$->kind.exp = VetorK;
            $$->lineno = lineno;
          }
    | error {yyerror("");}
;

tam: NUM {
            $$ = newExpNode(ConstK);
            $$->type = INTTYPE;
            $$->attr.name = NULL;
            $$->attr.val = atoi(tokenString);
            }

tipo: INT {
            $$ = newExpNode(TypeK);
            $$->attr.name = "INT";
            $$->type = INTTYPE;
            $$->kind.exp = TypeK; }
    | VOID {
            $$ = newExpNode(TypeK);
            $$->attr.name = "VOID";
            $$->type = VOIDTYPE;
            $$->kind.exp = TypeK; }
;

fun-id:  ID {
            $$ = newExpNode(IdK);
            $$->attr.name = copyString(id);
            $$->kind.exp = IdK; }
;
fun-dec: tipo fun-id APR parametros FPR escopo{
              $$ = newExpNode(FunDeclK);
              $$->kind.exp = FunDeclK;
              $$->attr.name = $2->attr.name;
              $$->child[0] = $1;
              $$->type = $1->type;
              $$->child[1] = $4;
              $$->child[2] = $6;
              $$->lineno = $2->lineno;
            }
;

parametros: VOID {
            $$ = newExpNode(TypeK);
            $$->attr.name = "VOID";
            $$->size = 0;
            $$->child[0] = NULL;
            $$->lineno = lineno;
          } 
        | lista-parametros { $$ = $1; }
;

lista-parametros: lista-parametros VIRG lista-parametros {
                YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                       t = t->sibling;
                  t->sibling = $3;
                  $$ = $1;
                }
                else $$ = $3;
              } | tipo-parametro { $$ = $1; }
;

tipo-parametro: tipo ID {
          $$ = newExpNode(VarParamK);
          $$->attr.name = copyString(id);
          $$->kind.exp = VarParamK;
          $$->size = 0;
          $$->lineno = lineno;
          $$->type = $1->type;
          $$->child[0] = $1;
        } | tipo ID ACO FCO{
         $$ = newExpNode(VetParamK);
          $$->child[0] = $1;
          $$->attr.name = copyString(id);
          $$->kind.exp = VetParamK;
          $$->size = 0;
          $$->lineno = lineno;
          $$->type = $1->type;
          $$->child[0] = $1;
        }
;

escopo: ACH dec-locais lista-dec-locais FCH {
                YYSTYPE t = $2;
                  if (t != NULL){
                    while (t->sibling != NULL)
                       t = t->sibling;
                    t->sibling = $3;
                    $$ = $2;
                  }
                  else $$ = $3;
              }
    | ACH FCH {}
    | ACH dec-locais FCH { $$ = $2; }
    | ACH lista-dec-locais FCH { $$ = $2; }
;

dec-locais: dec-locais var-dec {
              YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                     t = t->sibling;
                  t->sibling = $2;
                  $$ = $1;
                }
                else $$ = $2;
            }
    | var-dec { $$ = $1; }
;

lista-dec-locais: lista-dec-locais dec-interna {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
            }
    | dec-interna { $$ = $1; }
;
 
dec-interna: exp-dec { $$ = $1; }
    | escopo { $$ = $1; }
    | sel-dec { $$ = $1; }
    | iteracao-dec { $$ = $1; }
    | retorno-dec { $$ = $1; }
;

exp-dec: exp PEV { $$ = $1; }| PEV {}
;

sel-dec: IF APR exp FPR dec-interna {
            $$ = newStmtNode(IfK);
            $$->attr.name = "IF";
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->lineno = lineno;
            $$->kind.stmt = IfK;
          }
    | IF APR exp FPR dec-interna ELSE dec-interna{
            $$ = newStmtNode(IfK);
            $$->attr.name = "IF";
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
            $$->scope= $3->scope;
            $$->lineno = lineno;
            $$->kind.stmt = IfK;
          }
;

iteracao-dec: WHI APR exp FPR dec-interna {
          $$ = newStmtNode(WhileK);
          $$->attr.name = "WHILE";
          $$->child[0] = $3;
          $$->child[1] = $5;
          $$->scope = $3->scope;
          $$->lineno = lineno;
          $$->kind.stmt = WhileK;
        }
;

retorno-dec: RET exp PEV{
                $$ = newStmtNode(ReturnINT);
                $$->child[0] = $2;
                $$->lineno = lineno;
            }| RET PEV { $$ = newStmtNode(ReturnVOID); }
;

exp: var ATR exp {
        $$ = newStmtNode(AssignK);
        $$->kind.stmt = AssignK;
        $$->attr.name= $1->attr.name;
        $$->scope = scope;
        $$->child[0] = $1;
        $$->child[1] = $3;
        $$->lineno = lineno;
      }| exp-simples { $$ = $1; }
;

var: ID {
        $$ = newExpNode(IdK);
        $$->attr.name = copyString(id);
        $$->lineno = lineno;
        $$->child[0] = NULL;
  } |fun-id ACO exp FCO {
        $$ = newExpNode(IdK);
        $$->attr.name = $1->attr.name;
        $$->child[0] = $3;
        $$->lineno = lineno;
        }
;

exp-simples: exp-soma relacional exp-soma {
                  $$ = newStmtNode(AssignK);
                  $$ = $2;
                  $$->child[0] = $1;
                  $$->child[1] = $3;
                  $$->scope = scope;
              } | exp-soma { $$ = $1; }
;

relacional: IGL {
                $$ = newExpNode(OpK);
                $$->attr.op = IGL;
                $$->lineno = lineno;
              } | MENO {
                $$ = newExpNode(OpK);
                $$->attr.op = MENO;
                $$->lineno = lineno;
              }| MAIO {
                $$ = newExpNode(OpK);
                $$->attr.op = MAIO;
                $$->lineno = lineno;
              }| MAIG {
                $$ = newExpNode(OpK);
                $$->attr.op = MAIG;
                $$->lineno = lineno;
              }| MEIG {
                $$ = newExpNode(OpK);
                $$->attr.op = MEIG;
                $$->lineno = lineno;
              }| DIF {
                $$ = newExpNode(OpK);
                $$->attr.op = DIF;
                $$->lineno = lineno;
              }
;

exp-soma: exp-soma soma termo {
            $$ = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->scope = scope;
            $$->lineno = lineno;
       }| termo { $$ = $1; }
;

soma: SOM {
         $$ = newExpNode(OpK);
         $$->attr.op = SOM;
         $$->lineno = lineno;
   }| SUB {
         $$ = newExpNode(OpK);
         $$->attr.op = SUB;
         $$->lineno = lineno;
       }
;

termo: termo mult fator {
              $$ = $2;
              $$->scope = scope;
              $$->child[0] = $1;
              $$->child[1] = $3;
              $$->lineno = lineno;
    }| fator { $$ = $1; }
;

mult: MUL {
         $$ = newExpNode(OpK);
         $$->attr.op = MUL;
         $$->lineno = lineno;
   }| DIV {
         $$ = newExpNode(OpK);
         $$->attr.op = DIV;
         $$->lineno = lineno;
       }
;

fator: APR exp FPR { $$ = $2; } 
     | var { $$ = $1; }
     | ativacao { $$ = $1; params = 0; }
     | NUM {
            $$ = newExpNode(ConstK);
            $$->type = INTTYPE;
            $$->attr.name = "teste";
            $$->attr.val = atoi(tokenString);
            }
;

ativacao: fun-id APR args FPR {
          $$ = newExpNode(AtivK);
          $$->kind.exp = AtivK;
          $$->attr.name = $1->attr.name;
          $$->child[0] = $3;
          $$->params = params;
          $$->lineno = lineno;
       }| fun-id APR FPR{
           $$ = newExpNode(AtivK);
           $$->kind.exp = AtivK;
           $$->attr.name = $1->attr.name;
           $$->params = params;
           $$->lineno = lineno;
         }
;

args: args VIRG exp {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $3;
                params ++;
                $$ = $1;
              }
              else $$ = $3;
   }| exp { 
           params ++;
           $$ = $1; }
;

%%

int yyerror(char *message){
    if(yychar == -2) return 0;
    Error = TRUE;

    if(yychar == ERR) printf(N_VERM"[%d] Erro léxico!"RESET" Lexema: ", lineno);
    else printf(N_VERM"[%d] Erro sintático!"RESET" Token: ", lineno);
    switch (yychar){
        case IF:   printf("%s\n",tokenString);break;
        case RET:  printf("%s\n",tokenString);break;
        case ELSE: printf("%s\n",tokenString);break;
        case INT:  printf("%s\n",tokenString);break;
        case WHI:  printf("%s\n",tokenString);break;
        case VOID: printf("%s\n",tokenString);break;
        case IGL:  printf("==\n"); break;
        case ATR:  printf("=\n"); break;
        case DIF:  printf("!=\n"); break;
        case MENO: printf("<\n"); break;
        case MAIO: printf(">\n"); break;
        case MEIG: printf("<=\n"); break;
        case MAIG: printf(">=\n"); break;
        case APR:  printf("(\n"); break;
        case FPR:  printf(")\n"); break;
        case ACO:  printf("[\n"); break;
        case FCO:  printf("]\n"); break;
        case ACH:  printf("{\n"); break;
        case FCH:  printf("}\n"); break;
        case PEV:  printf(";\n"); break;
        case VIRG: printf(",\n"); break;
        case SOM:  printf("+\n"); break;
        case SUB:  printf("-\n"); break;
        case MUL:  printf("*\n"); break;
        case DIV:  printf("/\n"); break;
        case FIM:  printf("EOF\n"); break;
        case NUM:  printf("NUM, valor = %s\n",tokenString);break;
        case ID:   printf("ID, lexema = %s\n",tokenString);break;
        case ERR:  printf("%s\n",tokenString);break;
        default: /* should never happen */
          fprintf(listing,"Token desconhecido: %d\n",yychar);
   }
    return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}