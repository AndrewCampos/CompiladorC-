#ifndef _ANALYZE_H_
#define _ANALYZE_H_
#include "globals.h"

/**
 * @brief Atualiza o escopo atual da compilação.
 * 
 * @param t: Nó atual da árvore sintática a ser analisado.
 */
void UpdateScope(TreeNode * t);

/**
 * @brief Constrói a tabela de símbolos pela passagem de pré-ordem da árvore sintática.
 * 
 * @param t: Nó atual da árvore sintática a ser analisado.
 */
void buildSymtab(TreeNode*);

/**
 * @brief Percorre a árvore sintática em pós-ordem para fazer a checagem de tipos.
 * 
 * @param t: Nó atual da árvore sintática a ser analisado.
 */
void typeCheck(TreeNode*);

/**
 * @brief Chama checagens de tipo para cada nó da árvore sintática.
 * 
 * @param t: Nó atual da árvore sintática a ser analisado.
 */
void checkNode(TreeNode*);

#endif