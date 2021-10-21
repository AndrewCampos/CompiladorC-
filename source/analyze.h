#ifndef _ANALYZE_H_
#define _ANALYZE_H_
#include "globals.h"

/**
 * Atualiza o escopo atual da compilação.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 */
void UpdateScope(TreeNode * t);

/**
 * Constrói a tabela de símbolos pela passagem de pré-ordem da árvore sintática.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 */
void buildSymtab(TreeNode*);

/**
 * Percorre a árvore sintática em pós-ordem para fazer a checagem de tipos.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 */
void typeCheck(TreeNode*);

/**
 * Faz a checagem de tipos para o nó referenciado.
 * 
 * Parâmetros:
 * - t: Nó atual da árvore sintática a ser analisado.
 */
void checkNode(TreeNode*);

#endif