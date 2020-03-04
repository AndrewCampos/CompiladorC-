#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE é o tamanho da tabela hash */
#define SIZE 211

/* SHIFT é a potência de 2 usada como multiplicador da função hash */
#define SHIFT 4

/* A função hash */
static int hash ( char * key ){
  int temp = 0;
  int i = 0;
  while (key[i] != '\0'){ 
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

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
  dataTypes Dtype;
  IDTypes IType;
  char* escopo;
  LineList lines;
  int memloc ; /* posição da memória para variável */
  int vet;
  struct BucketListRec * next;
} * BucketList;

/* A tabela Hash */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int op, char* escopo, dataTypes DType, IDTypes IType, int vet ){
  int h = hash(name);
  BucketList l =  hashTable[h];
  if(IType == CALL){
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->vet = vet;
    l->memloc = op;
    l->IType = IType;
    l->Dtype = DType;
    l->escopo = escopo;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l;
  }

  // Procura a ultima declaração com o mesmo nome
  while ((l != NULL) && ((strcmp(name,l->name) != 0))){
    l = l->next;
  }

  //Para inserir: não achou outra declaração, se achou verificar se o escopo é DIF e não é uma função
  if ( l == NULL || (op != 0 && l->escopo != escopo && l->escopo != "global" && l->IType != FUN && l->IType != CALL)) { /* variável não está na tabela ainda */
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->vet = vet;
    l->memloc = op;
    l->IType = IType;
    l->Dtype = DType;
    l->escopo = escopo;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l;
  }
  else if( (l->IType == FUN  && IType == VAR) || (l->IType == CALL  && IType == VAR)){
    fprintf(listing,"Erro: Nome da variavel '%s' já utilizada como nome de função.[%d]\n",name, lineno);
    Error = TRUE;
  }
  else if( l->escopo == escopo && op != 0 ){
    fprintf(listing,"Erro: Variavel '%s' já declarada neste escopo.[%d]\n",name, lineno);
    Error = TRUE;
  }
  else if(l->escopo != escopo && (strcmp(l->escopo,"global") != 0) ){
    //procura por variavel global antes de supor que não existe
    fprintf(listing,"Erro: Variavel '%s' já declarada no escopo global.[%d]\n",name, lineno);
    Error = TRUE;
    /*while ((l != NULL)){
      if((strcmp(l->escopo, "global")==0)&& ((strcmp( name,l->name) == 0))){
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
        break;
      }
      l = l->next;
    }
    if(l == NULL){
      fprintf(listing,"Erro: Variavel '%s' não declarada neste escopo.[%d]\n",name, lineno);
      Error = TRUE;
    }*/
  }
  else if(op == 0)
  {
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

int st_lookup ( char * name, char * escopo){
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(escopo,l->escopo) != 0)){
    l = l->next;}
  if (l == NULL) return -1;
  else {
    return 0;
    }
}

/*Função que confere se a main() foi declarada */
void busca_main () {
  int h = hash("main");
  BucketList l =  hashTable[h];
  while ((l != NULL) && ((strcmp("main",l->name) != 0 || l->IType == VAR)))
    l = l->next;
  if (l == NULL) {
    fprintf(listing,"Erro: Função main não declarada\n");
    Error = TRUE;
  }
}
dataTypes getFunType(char* nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(nome,l->name) != 0))
    l = l->next;

  if (l == NULL) return -1;
  else return l->Dtype;
}

void printSymTab(FILE * listing) {
  int i;
  fprintf(listing,"Nome           Escopo  Tipo ID  Tipo dado  Num da linha\n");
  fprintf(listing,"-------------  ------  -------  ---------  ------------\n");
  for (i=0;i<SIZE;++i) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      while (l != NULL) {
        LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-6s  ",l->escopo);
        char* id, *data;
        switch(l->IType){
          case VAR:
            id = "var";
          break;
          case FUN:
             id = "fun";
          break;
          case CALL:
             id = "call";
          break;
          case VET:
            id= "vet";
          break;
          default:
          break;
        }
        switch(l->Dtype){
          case INTTYPE:
            data= "INT";
          break;
          case VOIDTYPE:
            data= "VOID";
          break;
          case NULLL:
            data = "null";
          break;
          default:
          break;
        }
        fprintf(listing,"%-7s  ",id);
        fprintf(listing,"%-8s  ",data);
        while (t != NULL) {
          fprintf(listing,"%3d; ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */