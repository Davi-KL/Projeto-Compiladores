#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "symtable.h"
#include "tokens.h"

static const char *nome_token(int codigo) {
    switch (codigo) {
        case EOF_TOKEN:   return "EOF_TOKEN";
        case ERRO:        return "ERRO";
        case TK_INICIO:   return "TK_INICIO";
        case TK_FIM:      return "TK_FIM";
        case TK_INTEIRO:  return "TK_INTEIRO";
        case TK_SE:       return "TK_SE";
        case TK_ENTAO:    return "TK_ENTAO";
        case TK_SENAO:    return "TK_SENAO";
        case TK_FIM_SE:   return "TK_FIM_SE";
        case TK_PARA:     return "TK_PARA";
        case TK_ATE:      return "TK_ATE";
        case TK_PASSO:    return "TK_PASSO";
        case TK_FIM_PARA: return "TK_FIM_PARA";
        case TK_LEIA:     return "TK_LEIA";
        case TK_IMPRIMA:  return "TK_IMPRIMA";
        case TK_ESCREVA:  return "TK_ESCREVA";
        case TK_E:        return "TK_E";
        case TK_OU:       return "TK_OU";
        case TK_NAO:      return "TK_NAO";
        case TK_IDENT:    return "TK_IDENT";
        case TK_CONST_INT: return "TK_CONST_INT";
        case TK_CONST_LIT: return "TK_CONST_LIT";
        case TK_MAIS:     return "TK_MAIS";
        case TK_MENOS:    return "TK_MENOS";
        case TK_MULT:     return "TK_MULT";
        case TK_DIV:      return "TK_DIV";
        case TK_ABRE_P:   return "TK_ABRE_P";
        case TK_FECHA_P:  return "TK_FECHA_P";
        case TK_MAIOR:    return "TK_MAIOR";
        case TK_MENOR:    return "TK_MENOR";
        case TK_MAIOR_I:  return "TK_MAIOR_I";
        case TK_MENOR_I:  return "TK_MENOR_I";
        case TK_IGUAL:    return "TK_IGUAL";
        case TK_DIFER:    return "TK_DIFER";
        case TK_ATRIB:    return "TK_ATRIB";
        case TK_DPONTOS:  return "TK_DPONTOS";
        case TK_PVIRG:    return "TK_PVIRG";
        case TK_VIRG:     return "TK_VIRG";
        default:          return "?";
    }
}

int main(int argc, char *argv[]) {
    FILE *fp = stdin;

    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (!fp) {
            fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", argv[1]);
            return 1;
        }
    }

    lexer_init(fp);

    /* Simula o parser: chama o léxico repetidamente até EOF */
    printf("%-6s %-14s %-30s %s\n", "COD", "NOME", "LEXEMA", "POS");
    printf("%-6s %-14s %-30s %s\n", "---", "----", "------", "---");

    Token t;
    do {
        t = proximo_token();
        if (t.posicao == -1)
            printf("%-6d %-14s %-30s -\n",  t.codigo, nome_token(t.codigo), t.lexema);
        else
            printf("%-6d %-14s %-30s %d\n", t.codigo, nome_token(t.codigo), t.lexema, t.posicao);
    } while (t.codigo != EOF_TOKEN);

    if (fp != stdin) fclose(fp);
    return 0;
}
