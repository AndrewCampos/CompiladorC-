#ifndef _SCAN_H_
#define _SCAN_H_

/** 
 * @brief MAXTOKENLEN é o tamanho máximo de um token.
 */
#define MAXTOKENLEN 40

/**
 * @brief tokenString é o array que armazena o lexema de cada token.
 */
extern char tokenString[MAXTOKENLEN+1];
extern char id[MAXTOKENLEN+1];
extern char func[MAXTOKENLEN+1];

/**
 * @brief Função getToken retorna o próximo token no arquivo fonte.
 * */
TokenType getToken(void);

#endif