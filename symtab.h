#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* SIZE é o tamanho da tabela hash */
#define SIZE 211

/* SHIFT é a potência de 2 usada como multiplicador da função hash */
#define SHIFT 4

/* a lista de números de linha do código fonte no qual uma variável é referenciada */
typedef struct LineListRec { 
  int lineno;
  struct LineListRec * next;
} * LineList;

/* O registro na lista de baldes para cada variável, incluindo nome, local de memória atribuído
 e a lista de números de linha em que aparece no código-fonte
 */
typedef struct BucketListRec { 
  char * name;
  dataTypes RetType;
  dataTypes StmtType;
  IDTypes IType;
  char* escopo;
  LineList lines;
  int memloc ; /* posição da memória para variável */
  int vet;
  int tam;
  struct BucketListRec * next;
} * BucketList;

/* A tabela Hash */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and memory locations into the symbol table
 loc = memory location is inserted only the first time, otherwise ignored */
 void st_insert( char * name, int lineno, int op, char* escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet);

/* Função st_lookup retorna a posição da memória de uma variável ou -1 se não encontrar */
int st_lookup ( char * name, char * escopo);

/*Função para verificar se o retorno da função tipo INT está presente*/
int checkReturn(char* escopo);

/* Função para conferir a posição de memoria alocada para uma variável */
int getMemLoc(char* nome, char* escopo);

/* Função para conferir o tipo da variável (Var ou Vet) */
IDTypes getVarType(char* nome, char* escopo);

/* O procedimento printSymTab imprime uma lista formatada do conteúdo da tabela de símbolos no
 arquivo de listagem */
void printSymTab(FILE * listing);

/* Busca declaração da função main() */
void busca_main ();

/* Retorna o tipo de retorno da função */
dataTypes getFunType(char* nome);

/* Retorna a quantidade de parametros da função */
int getNumParam(char *nome);

/* Incrementa o contador de parametros da função */
void insereParam(char *nome);
#endif