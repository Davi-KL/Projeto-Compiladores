#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define LEXEMA_MAX 256

typedef struct {
    int  codigo;            /* código do token (ver tokens.h)          */
    int  posicao;           /* índice na tabela; -1 se não aplicável   */
    char lexema[LEXEMA_MAX];/* texto original do token                 */
    int  linha;
    int  coluna;
} Token;

/* Configura a fonte de entrada antes de chamar proximo_token */
void  lexer_init(FILE *fp);

/* Devolve o próximo token; retorna EOF_TOKEN ao fim do arquivo */
Token proximo_token(void);

#endif /* LEXER_H */
