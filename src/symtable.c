#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "tokens.h"

/* ---------- tabela de palavras reservadas ---------- */

typedef struct {
    const char *lexema;
    int         codigo;
} PalavraReservada;

/* Comparação byte-a-byte em UTF-8 — funciona para os acentos do Portugol */
static const PalavraReservada palavras_reservadas[] = {
    { "inicio",   TK_INICIO   },
    { "fim",      TK_FIM      },
    { "inteiro",  TK_INTEIRO  },
    { "se",       TK_SE       },
    { "então",    TK_ENTAO    },
    { "senão",    TK_SENAO    },
    { "fim_se",   TK_FIM_SE   },
    { "para",     TK_PARA     },
    { "até",      TK_ATE      },
    { "passo",    TK_PASSO    },
    { "fim_para", TK_FIM_PARA },
    { "leia",     TK_LEIA     },
    { "imprima",  TK_IMPRIMA  },
    { "escreva",  TK_ESCREVA  },
    { "E",        TK_E        },
    { "OU",       TK_OU       },
    { "NÃO",      TK_NAO      },
    { NULL,       -1          }
};

int eh_palavra_reservada(const char *lex) {
    for (int i = 0; palavras_reservadas[i].lexema != NULL; i++) {
        if (strcmp(lex, palavras_reservadas[i].lexema) == 0)
            return palavras_reservadas[i].codigo;
    }
    return -1;
}

/* ---------- tabela de símbolos genérica ---------- */

#define CAP_INICIAL 64

typedef struct {
    char **entradas;
    int    tamanho;
    int    capacidade;
} Tabela;

static Tabela tab_ident   = { NULL, 0, 0 };
static Tabela tab_literal = { NULL, 0, 0 };
static Tabela tab_inteiro = { NULL, 0, 0 };

/* Garante capacidade mínima na tabela */
static void tabela_crescer(Tabela *t) {
    if (t->entradas == NULL) {
        t->capacidade = CAP_INICIAL;
        t->entradas   = malloc(t->capacidade * sizeof(char *));
        if (!t->entradas) { perror("malloc"); exit(1); }
    } else if (t->tamanho == t->capacidade) {
        t->capacidade *= 2;
        t->entradas    = realloc(t->entradas, t->capacidade * sizeof(char *));
        if (!t->entradas) { perror("realloc"); exit(1); }
    }
}

/* Busca linear; insere e devolve índice */
static int tabela_inserir(Tabela *t, const char *lex) {
    for (int i = 0; i < t->tamanho; i++) {
        if (strcmp(t->entradas[i], lex) == 0)
            return i;
    }
    tabela_crescer(t);
    t->entradas[t->tamanho] = strdup(lex);
    if (!t->entradas[t->tamanho]) { perror("strdup"); exit(1); }
    return t->tamanho++;
}

int st_inserir_ident(const char *lex)   { return tabela_inserir(&tab_ident,   lex); }
int st_inserir_literal(const char *lex) { return tabela_inserir(&tab_literal, lex); }
int st_inserir_inteiro(const char *lex) { return tabela_inserir(&tab_inteiro, lex); }

void st_imprimir(FILE *fp) {
    fprintf(fp, "\n=== Tabela de Identificadores ===\n");
    for (int i = 0; i < tab_ident.tamanho; i++)
        fprintf(fp, "  [%d] %s\n", i, tab_ident.entradas[i]);

    fprintf(fp, "\n=== Tabela de Constantes Inteiras ===\n");
    for (int i = 0; i < tab_inteiro.tamanho; i++)
        fprintf(fp, "  [%d] %s\n", i, tab_inteiro.entradas[i]);

    fprintf(fp, "\n=== Tabela de Literais ===\n");
    for (int i = 0; i < tab_literal.tamanho; i++)
        fprintf(fp, "  [%d] %s\n", i, tab_literal.entradas[i]);
}
