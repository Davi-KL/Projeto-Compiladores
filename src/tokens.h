#ifndef TOKENS_H
#define TOKENS_H

/* Token especial */
#define EOF_TOKEN   0
#define ERRO       -1

/* Palavras reservadas (1–17) */
#define TK_INICIO   1
#define TK_FIM      2
#define TK_INTEIRO  3
#define TK_SE       4
#define TK_ENTAO    5
#define TK_SENAO    6
#define TK_FIM_SE   7
#define TK_PARA     8
#define TK_ATE      9
#define TK_PASSO   10
#define TK_FIM_PARA 11
#define TK_LEIA    12
#define TK_IMPRIMA 13
#define TK_ESCREVA 14
#define TK_E       15
#define TK_OU      16
#define TK_NAO     17

/* Identificadores e constantes */
#define TK_IDENT       100
#define TK_CONST_INT   101
#define TK_CONST_LIT   102

/* Operadores e pontuação */
#define TK_MAIS    200   /* +  */
#define TK_MENOS   201   /* -  */
#define TK_MULT    202   /* *  */
#define TK_DIV     203   /* /  */
#define TK_ABRE_P  204   /* (  */
#define TK_FECHA_P 205   /* )  */
#define TK_MAIOR   206   /* >  */
#define TK_MENOR   207   /* <  */
#define TK_MAIOR_I 208   /* >= */
#define TK_MENOR_I 209   /* <= */
#define TK_IGUAL   210   /* =  */
#define TK_DIFER   211   /* <> */
#define TK_ATRIB   212   /* <- */
#define TK_DPONTOS 213   /* :  */
#define TK_PVIRG   214   /* ;  */
#define TK_VIRG    215   /* ,  */

#endif /* TOKENS_H */
