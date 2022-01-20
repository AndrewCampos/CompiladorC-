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
TreeNode *newStmtNode(StmtKind);

/**
 * Cria um novo nó de expressão para a construção da árvore sintática.
 * 
 * Parâmetros:
 * - kind: Tipo do novo nó.
 */
TreeNode *newExpNode(ExpKind);

/**
 * Aloca e faz uma nova cópia de uma string existente.
 * 
 * Parâmetros:
 * - string: String a ser copiada.
 */
char *copyString( char * );

/**
 * Insere uma string em outra nos locais indicados.
 * 
 * Parâmetros:
 * - string1: String a ser complementada.
 * - string2: String a ser inserida
 * 
 * Retorna:
 * String resultante da mesclagem das strings.
 */
char *composeString(char *, char *);

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
void nameFiles(char *);

/**
 * Abre os arquivos gerados pela compilação e os popula com as funções de impressão de cada parte módulo do compilador.
 */
void makeFiles();

/**
 * Imprime uma mensagem de erro formatada.
 * 
 * Parâmetros:
 * - message: Mensagem a ser apresentada.
 * - step: Etapa da compilação que se encontra o erro.
 * - line: Linha do código (se existir) que se encontra o erro.
 */
void printError(char *, CompilationStep, int );

/**
 * Imprime uma mensagem de aviso formatada.
 * 
 * Parâmetros:
 * - message: Mensagem a ser apresentada.
 * - line: Linha do código (se existir) que se encontra o aviso.
 */
void printWarning(char *, int);

/**
 * Imprime uma mensagem de sucesso formatada.
 * 
 * Parâmetros:
 * - message: Mensagem a ser apresentada.
 * - flag: Indicador que controla o formato da mensagem.
 */
void printSucess(char *, PrintFlag);

    /**
 * @brief Confere se o nó se trata de uma função reservada do sistema.
 * 
 * @param t: Nó atual da árvore sintática a ser analisado.
 * 
 * @return 1 caso seja uma função reservada, 0 caso contrário.
 */
int isReservedFunction(TreeNode *t);

#endif