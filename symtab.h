#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

/* Procedure st_insert inserts line numbers and memory locations into the symbol table
 * loc = memory location is inserted only the first time, otherwise ignored */
 void st_insert( char * name, int lineno, int loc, char* escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet);

/* Função st_lookup retorna a posição da memória de uma variável ou -1 se não encontrar */
int st_lookup ( char * name, char * escopo);

/*Função para verificar se o retorno da função tipo INT está presente*/
int checkReturn(char* escopo);

/* O procedimento printSymTab imprime uma lista formatada do conteúdo da tabela de símbolos no
 arquivo de listagem */
void printSymTab(FILE * listing);

void busca_main ();
dataTypes getFunType(char* nome);
#endif