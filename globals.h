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

/* Cores para o terminal */
#define PRT "\e[0;30m" // Texto normal preto
#define VERM "\e[0;31m" // Texto normal vermelho
#define VERD "\e[0;32m" // Texto normal verde
#define AMAR "\e[0;33m" // Texto normal amarelo
#define AZ "\e[0;34m" // Texto normal azul
#define MAG "\e[0;35m" // Texto normal magenta
#define CIAN "\e[0;36m" // Texto normal ciano
#define BRC "\e[0;37m" // Texto normal branco

#define N_PRT "\e[1;30m" // Texto negrito preto
#define N_VERM "\e[1;31m" // Texto negrito vermelho
#define N_VERD "\e[1;32m" // Texto negrito verde
#define N_AMAR "\e[1;33m" // Texto negrito amarelo
#define N_AZ "\e[1;34m" // Texto negrito azul
#define N_MAG "\e[1;35m" // Texto negrito magenta
#define N_CIAN "\e[1;36m" // Texto negrito ciano
#define N_BRC "\e[1;37m" // Texto negrito branco

#define SUB_PRT "\e[4;30m"  // Texto sublinhado preto
#define SUB_VERM "\e[4;31m"  // Texto sublinhado vermelho
#define SUB_VERD "\e[4;32m"  // Texto sublinhado verde
#define SUB_AMAR "\e[4;33m"  // Texto sublinhado amarelo
#define SUB_AZ "\e[4;34m"  // Texto sublinhado azul
#define SUB_MAG "\e[4;35m"  // Texto sublinhado magenta
#define SUB_CIAN "\e[4;36m"  // Texto sublinhado ciano
#define SUB_BRC "\e[4;37m"  // Texto sublinhado branco

#define RESET "\e[0m"    // Texto padrão

#define nregisters 32 // numero de registradores
#define nregtemp 20 // numero de registradores temporários
#define nregparam 10 // numero de registradores de parametros
#define iniDataMem 361 // endereço da primeira posição da memória de dados
#define lploc 461 // endereço do inicio das variaveis locais

/* Yacc/Bison gera os próprios valores inteiros para os tokens */
typedef int TokenType;
typedef int FlagType;

extern FILE* source; /* arquivo texto do código fonte */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,WhileK,AssignK,ReturnINT,ReturnVOID} StmtKind;
typedef enum {OpK,ConstK,IdK,VarDeclK,VetDeclK,FunDeclK,AtivK,TypeK,VetorK,VarParamK,VetParamK} ExpKind;

/* ExpType é usado para checagem de tipo */
typedef enum {Void,Integer,Boolean} ExpType;
typedef enum {INTTYPE, VOIDTYPE, NULLL} dataTypes;
typedef enum {VAR, FUN, CALL, VET, RETT} IDTypes;

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


TreeNode * syntaxTree;

/* Caminhos dos arquivos a serem gerados pela compilação do arquivo */
char *ArvSint; // Caminho para a árvore sintática
char *TabSimb; // Caminho para a tabela de simbolos
char *interCode; // Caminho para o codigo intermediário
char *assCode; // Caminho para o codigo assembly
char *binCode; // caminho para o codigo binário

extern FlagType TraceScan;
extern FlagType TraceParse;
extern FlagType TraceAnalyze;
extern FlagType TraceCode;
extern FlagType PrintCode;
extern FlagType Error;
#endif