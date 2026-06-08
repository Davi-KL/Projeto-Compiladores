#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "symtable.h"
#include "tokens.h"

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
    printf("%-6s %-30s %s\n", "COD", "LEXEMA", "POS");
    printf("%-6s %-30s %s\n", "---", "------", "---");

    Token t;
    do {
        t = proximo_token();
        if (t.posicao == -1)
            printf("%-6d %-30s -\n",  t.codigo, t.lexema);
        else
            printf("%-6d %-30s %d\n", t.codigo, t.lexema, t.posicao);
    } while (t.codigo != EOF_TOKEN);

    if (fp != stdin) fclose(fp);
    return 0;
}
