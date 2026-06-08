#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "tokens.h"
#include "symtable.h"

/* ---------- estado interno do scanner ---------- */

static FILE *fonte      = NULL;
static int   linha_atual = 1;
static int   col_atual   = 0;
static int   char_devolvido = -1; /* buffer de lookahead (unget) */

/* ---------- primitivas de leitura ---------- */

/* Lê o próximo byte da entrada, atualiza linha/coluna */
static int ler_char(void) {
    int c;
    if (char_devolvido != -1) {
        c = char_devolvido;
        char_devolvido = -1;
    } else {
        c = fgetc(fonte);
    }
    if (c == '\n') { linha_atual++; col_atual = 0; }
    else if (c != EOF) col_atual++;
    return c;
}

/* Devolve um caractere para ser relido */
static void devolver_char(int c) {
    char_devolvido = c;
    /* Ajusta contadores (simplificado: só corrige para não-newline) */
    if (c != '\n' && c != EOF) col_atual--;
}

/* ---------- inicialização ---------- */

void lexer_init(FILE *fp) {
    fonte          = fp;
    linha_atual    = 1;
    col_atual      = 0;
    char_devolvido = -1;
}

/* ---------- funções auxiliares de classificação ---------- */

/* Verifica se o byte inicia (ou continua) um caractere UTF-8 multibyte */
static int eh_byte_utf8_continuacao(int c) {
    return (c & 0xC0) == 0x80;
}

/*
 * Verifica se c pode iniciar um identificador ou palavra reservada.
 * Aceita ASCII letra/underscore OU byte de início de sequência UTF-8
 * (0xC2..0xF4) — necessário para "então", "senão", "até", "NÃO".
 */
static int inicia_ident(int c) {
    return isalpha(c) || c == '_' || (unsigned char)c >= 0xC2;
}

/* Verifica se c pode continuar um identificador */
static int continua_ident(int c) {
    return isalnum(c) || c == '_' || eh_byte_utf8_continuacao(c)
           || (unsigned char)c >= 0xC2;
}

/* ---------- construção de token de erro ---------- */

static Token token_erro(int lin, int col, char car) {
    Token t;
    t.codigo   = ERRO;
    t.posicao  = -1;
    t.linha    = lin;
    t.coluna   = col;
    t.lexema[0] = car;
    t.lexema[1] = '\0';
    fprintf(stderr, "ERRO LEXICO: linha %d, coluna %d, caractere '%c'\n",
            lin, col, car);
    return t;
}

/* ---------- proximo_token ---------- */

Token proximo_token(void) {
    int c;

    /* Pula espaços e quebras de linha */
    do { c = ler_char(); } while (c == ' ' || c == '\t' || c == '\r' || c == '\n');

    int lin = linha_atual;
    int col = col_atual;

    Token t;
    t.posicao = -1;
    t.linha   = lin;
    t.coluna  = col;

    /* EOF */
    if (c == EOF) {
        t.codigo = EOF_TOKEN;
        strcpy(t.lexema, "EOF");
        return t;
    }

    /* ---- Identificadores e palavras reservadas ---- */
    if (inicia_ident(c)) {
        int len = 0;
        t.lexema[len++] = (char)c;

        while (1) {
            c = ler_char();
            if (c == EOF) break;
            if (continua_ident(c)) {
                if (len < LEXEMA_MAX - 1) t.lexema[len++] = (char)c;
            } else {
                devolver_char(c);
                break;
            }
        }
        t.lexema[len] = '\0';

        int cod = eh_palavra_reservada(t.lexema);
        if (cod != -1) {
            t.codigo  = cod;
            t.posicao = -1;
        } else {
            t.codigo  = TK_IDENT;
            t.posicao = st_inserir_ident(t.lexema);
        }
        return t;
    }

    /* ---- Constantes inteiras ---- */
    if (isdigit(c)) {
        int len = 0;
        t.lexema[len++] = (char)c;

        while (1) {
            c = ler_char();
            if (c == EOF) break;
            if (isdigit(c)) {
                if (len < LEXEMA_MAX - 1) t.lexema[len++] = (char)c;
            } else {
                devolver_char(c);
                break;
            }
        }
        t.lexema[len] = '\0';
        t.codigo  = TK_CONST_INT;
        t.posicao = st_inserir_inteiro(t.lexema);
        return t;
    }

    /* ---- Constantes literais (strings) ---- */
    if (c == '"') {
        int len = 0;
        t.lexema[len++] = '"';

        while (1) {
            c = ler_char();
            if (c == EOF) {
                t.lexema[len] = '\0';
                fprintf(stderr,
                    "ERRO LEXICO: linha %d, coluna %d, string nao fechada\n",
                    lin, col);
                t.codigo  = ERRO;
                t.posicao = -1;
                return t;
            }
            if (len < LEXEMA_MAX - 2) t.lexema[len++] = (char)c;
            if (c == '"') break;
        }
        t.lexema[len] = '\0';
        t.codigo  = TK_CONST_LIT;
        t.posicao = st_inserir_literal(t.lexema);
        return t;
    }

    /* ---- Operadores e pontuação ---- */
    switch (c) {
        case '+': t.codigo = TK_MAIS;    strcpy(t.lexema, "+");  return t;
        case '-': t.codigo = TK_MENOS;   strcpy(t.lexema, "-");  return t;
        case '*': t.codigo = TK_MULT;    strcpy(t.lexema, "*");  return t;
        case '/': t.codigo = TK_DIV;     strcpy(t.lexema, "/");  return t;
        case '(': t.codigo = TK_ABRE_P;  strcpy(t.lexema, "(");  return t;
        case ')': t.codigo = TK_FECHA_P; strcpy(t.lexema, ")");  return t;
        case '=': t.codigo = TK_IGUAL;   strcpy(t.lexema, "=");  return t;
        case ':': t.codigo = TK_DPONTOS; strcpy(t.lexema, ":");  return t;
        case ';': t.codigo = TK_PVIRG;   strcpy(t.lexema, ";");  return t;
        case ',': t.codigo = TK_VIRG;    strcpy(t.lexema, ",");  return t;

        case '>': {
            int next = ler_char();
            if (next == '=') { t.codigo = TK_MAIOR_I; strcpy(t.lexema, ">="); }
            else             { devolver_char(next); t.codigo = TK_MAIOR; strcpy(t.lexema, ">"); }
            return t;
        }

        /*
         * '<' pode ser: <  <=  <>  <-
         * Lê um byte adiante para decidir.
         */
        case '<': {
            int next = ler_char();
            if (next == '=') { t.codigo = TK_MENOR_I; strcpy(t.lexema, "<="); }
            else if (next == '>') { t.codigo = TK_DIFER; strcpy(t.lexema, "<>"); }
            else if (next == '-') { t.codigo = TK_ATRIB; strcpy(t.lexema, "<-"); }
            else { devolver_char(next); t.codigo = TK_MENOR; strcpy(t.lexema, "<"); }
            return t;
        }

        default:
            return token_erro(lin, col, (char)c);
    }
}
