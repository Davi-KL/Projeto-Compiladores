#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>

/* Retorna índice do identificador (insere se não existir) */
int  st_inserir_ident(const char *lex);

/* Retorna índice da constante literal (insere se não existir) */
int  st_inserir_literal(const char *lex);

/* Retorna índice da constante inteira (insere se não existir) */
int  st_inserir_inteiro(const char *lex);

/* Retorna o código do token se for palavra reservada, -1 caso contrário */
int  eh_palavra_reservada(const char *lex);

/* Imprime a tabela de símbolos (diagnóstico opcional) */
void st_imprimir(FILE *fp);

#endif /* SYMTABLE_H */
