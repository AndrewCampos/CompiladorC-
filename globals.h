#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "Parser.tab.h"

/* ENDFILE é definido implicitamente pelo Yacc/Bison, e não está incluso no arquivo tab.h file */
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = o número de palavras reservadas */
#define MAXRESERVED 8

/* Yacc/Bison gera os próprios valores inteiros para os tokens */
typedef int TokenType;

extern FILE* source; /* arquivo texto do código fonte */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,WhileK,AssignK,ReturnK} StmtKind;
typedef enum {OpK,ConstK,IdK,VarDeclK,VetDeclK,FunDeclK,AtivK,TypeK,VetorK,ParamK} ExpKind;

/* ExpType é usado para checagem de tipo */
typedef enum {Void,Integer,Boolean} ExpType;
typedef enum {INTTYPE, VOIDTYPE, NULLL} dataTypes;
typedef enum {VAR, FUN, CALL, VET} IDTypes;

#define MAXCHILDREN 3

typedef struct treeNode {
  struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     int size;
     int add;
     int already_seem;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp;} kind;
     union { TokenType op;
             int val;
             char * name; } attr;
     char * idname;
     char *  scope;
     char * idtype;
     char * datatype;
     int vet;
     int declared;
     int params;
     dataTypes type; /* para checagem de tipo das exps */
   } TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE faz com que as informações do token sejam impressas no arquivo de listagem,
pois cada token é reconhecido pelo scanner */
extern int TraceScan;

/* TraceParse = TRUE faz com que a árvore sintática seja impressa no arquivo de listagem na
forma linearizada (usando recuos para filhos)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE faz com que inserções e pesquisas da tabela de símbolos sejam relatadas
ao arquivo de listagem */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written to the TM code file as code is generated */
extern int TraceCode;

/* Error = TRUE evita passagens adicionais se ocorrer um erro */
extern int Error;
#endif