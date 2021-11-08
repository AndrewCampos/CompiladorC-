#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "Parser.tab.h"

#define ENDFILE 0 // Definido implicitamente pelo Yacc/Bison, e não está incluso no arquivo tab.h file

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define PRT "\e[0;30m"  // Texto normal preto
#define VERM "\e[0;31m" // Texto normal vermelho
#define VERD "\e[0;32m" // Texto normal verde
#define AMAR "\e[0;33m" // Texto normal amarelo
#define AZ "\e[0;34m"   // Texto normal azul
#define MAG "\e[0;35m"  // Texto normal magenta
#define CIAN "\e[0;36m" // Texto normal ciano
#define BRC "\e[0;37m"  // Texto normal branco

#define N_PRT "\e[1;30m"  // Texto negrito preto
#define N_VERM "\e[1;31m" // Texto negrito vermelho
#define N_VERD "\e[1;32m" // Texto negrito verde
#define N_AMAR "\e[1;33m" // Texto negrito amarelo
#define N_AZ "\e[1;34m"   // Texto negrito azul
#define N_MAG "\e[1;35m"  // Texto negrito magenta
#define N_CIAN "\e[1;36m" // Texto negrito ciano
#define N_BRC "\e[1;37m"  // Texto negrito branco

#define SUB_PRT "\e[4;30m"  // Texto sublinhado preto
#define SUB_VERM "\e[4;31m" // Texto sublinhado vermelho
#define SUB_VERD "\e[4;32m" // Texto sublinhado verde
#define SUB_AMAR "\e[4;33m" // Texto sublinhado amarelo
#define SUB_AZ "\e[4;34m"   // Texto sublinhado azul
#define SUB_MAG "\e[4;35m"  // Texto sublinhado magenta
#define SUB_CIAN "\e[4;36m" // Texto sublinhado ciano
#define SUB_BRC "\e[4;37m"  // Texto sublinhado branco

#define RESET "\e[0m" // Texto padrão

#define nregisters 32  // Número de registradores
#define nregtemp 20    // Número de registradores temporários
#define nregparam 10   // Número de registradores de parametros
#define iniDataMem 250 // Endereço da primeira posição da memória de dados
#define lploc 300      // Endereço do inicio das variaveis locais
#define MAXRESERVED 8  // Número máximo de palavras reservadas
#define MAXCHILDREN 3  // Número máximo de filhos de um nó da árvore sintática

typedef int TokenType;
typedef int FlagType;
extern FILE *source;  // Arquivo texto do código fonte
extern FILE *listing; // Arquivo de saída para impressões
extern FILE *code;    // Code text file for TM simulator
extern int lineno;    // Número da linha atual da compulação
extern int init_code; // Posicao inicial da primeira instrucao

/**
 * Etapas da fase de compilação
 */
typedef enum {
    Lexicon,
    Syntactic,
    Semantics,
    CodeGen,
    Beginning
} CompilationStep;

typedef enum {
    FullMessage,
    HeaderOnly
} PrintFlag;

/**
 * Tipo do nó da árvore sintática.
 */
typedef enum {
    StmtK,
    ExpK
} NodeKind;

/**
 * Tipo de declaração do nó.
 */
typedef enum {
    IfK,
    WhileK,
    AssignK,
    ReturnINT,
    ReturnVOID
} StmtKind;

/**
 * Tipo de expressão do nó.
 */
typedef enum {
    OpK, // Operando.
    ConstK, // Valor numérico constante.
    IdK, // Uso de variável.
    VarDeclK, // Declaração de variável.
    VetDeclK, // Declaração de vetor.
    FunDeclK, // Declaração de função.
    AtivK, // Chamada de função.
    TypeK, // Palavra reservada de tipo.
    VetorK, // Uso de parâmetro vetor.
    VarParamK, // Declaração de variável parâmetro.
    VetParamK // Declaração de vetro parâmetro.
} ExpKind;

/** 
 * Tipagem de uma expressão.
 */
typedef enum {
    Void,
    Integer,
    Boolean
} ExpType;

/**
 * Tipagem de retornos e escopos.
 */
typedef enum {
    INTTYPE,
    VOIDTYPE,
    NULLL
} dataTypes;

/**
 * Tipo do identificador.
 */
typedef enum {
    VAR,
    PVAR,
    FUN,
    CALL,
    VET,
    PVET,
    RETT
} IDTypes;

/**
 * Nó da árvore sintática.
 */
typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    int lineno;
    int size;
    int add;
    int already_seem;
    NodeKind nodekind;
    union {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    union {
        TokenType op;
        int val;
        char *name;
    } attr;
    char *idname;
    char *scope;
    char *idtype;
    char *datatype;
    int vet;
    int declared;
    int params;
    dataTypes type; /* para checagem de tipo das exps */
} TreeNode;

TreeNode *syntaxTree; // Raiz da árvore sintática.

char *ArvSint;   // Caminho para a árvore sintática
char *TabSimb;   // Caminho para a tabela de simbolos
char *interCode; // Caminho para o codigo intermediário
char *assCode;   // Caminho para o codigo assembly
char *binCode;   // caminho para o codigo binário

extern FlagType TraceScan;    // Imprimir tokens
extern FlagType TraceParse;   // Imprimir árvore sintática
extern FlagType TraceAnalyze; // Imprimir tabela de simbolos
extern FlagType TraceCode;    // Imprimir nós da geração de código
extern FlagType PrintCode;    // Imprimir os códigos gerados
extern FlagType CreateFiles;  // Criar arquivos de compilação
extern FlagType Error;        // Flag que marca a existência de erros
extern FlagType SO;           // Indica se a compilação é de um SO
#endif