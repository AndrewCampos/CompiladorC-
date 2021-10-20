#ifndef _UTIL_H_
#define _UTIL_H_

/**
 * Converte para String e imprime no arquivo apontado por "listing" cada token gerado pela análise léxica.
 * 
 * Parâmetros:
 * - token: Código do token gerado pelo analizador léxico.
 * - tokenString: String contendo o nome do token a ser imprimido (Caso se trate de uma palavra reservada, número ou ID).
 */
void printToken( TokenType, const char* );

/**
 * Função newStmtNode cria um novo nó de declaração para a árvore sintática.
 * 
 * Parâmetros:
 * - kind: Tipo do novo nó.
 */
TreeNode * newStmtNode(StmtKind);

/**
 * Cria um novo nó de expressão para a construção da árvore sintática.
 * 
 * Parâmetros:
 * - kind: Tipo do novo nó.
 */
TreeNode * newExpNode(ExpKind);

/**
 * Aloca e faz uma nova cópia de uma string existente.
 * 
 * Parâmetros:
 * - string: String a ser copiada.
 */
char * copyString( char * );

/**
 * O procedimento printTree printa uma árvore sintática para o arquivo de listagem usando identação para indicar
 * subárvores.
 * 
 * Parâmetros:
 * - tree: Endereço do topo da árvore.
 */
void printTree( TreeNode * );

/**
 * Cria uma strings para cada extensão de arquivo gerado pelo compilador com o nome do arquivo compilado, sua respectiva
 * extensão e pasta de destino.
 * 
 * Parâmetros:
 * - nome: Nome do arquivo compilado.
 */
void nomeiaArquivos(char *nome);

/**
 * Abre os arquivos gerados pela compilação e os popula com as funções de impressão de cada parte módulo do compilador.
 */
void criaArquivos();

#endif