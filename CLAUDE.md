# CLAUDE.md — Analisador Léxico Portugol (TCD Compiladores)

> Documento de contexto para o Claude trabalhar neste projeto. Resume o enunciado, define decisões de design, estrutura do repositório e checklists de implementação. Sempre que houver dúvida, **o enunciado original do professor (PDF) é a fonte da verdade**.

---

## 1. Contexto do projeto

**Disciplina:** Compiladores — CEUB / FATECS — Ciência da Computação
**Professor:** Leonardo Pol
**Avaliação:** TCD — Projeto A (Laboratório de pesquisa de Compiladores)
**Modalidade:** Trabalho **individual**. Cópias/plágio são anulados.
**Linguagens permitidas pelo enunciado:** C/C++ ou Java.
**Linguagem escolhida:** **C** (padrão C99).

### Objetivo

Implementar um **analisador léxico** para uma linguagem tipo **Portugol** que:

1. Identifica os tokens da linguagem.
2. Constrói uma **tabela de símbolos** para uso futuro pelo analisador sintático.
3. Devolve **um token por vez**, sob demanda (como será chamado pelo parser).
4. Para cada token reconhecido, retorna o **código do token** e, quando aplicável, a **posição na tabela de símbolos**.
5. Trata **palavras reservadas** via tabela auxiliar: lê-se como identificador e testa-se se é palavra-reservada com função que devolve `-1` se não for, ou o **código da palavra reservada** se for.
6. Possui um token especial de **fim de arquivo (EOF)**.
7. `main` simula o parser: chama o léxico repetidamente e imprime `(código, posição_na_tabela)` até EOF.

---

## 2. Especificação da linguagem (Portugol do enunciado)

### 2.1 Tipos de dados
- Apenas **`inteiro`** (equivalente a `int` do C).

### 2.2 Constantes
- **Numéricas inteiras** (ex.: `5`, `0`, `42`).
- **Literais / strings** (ex.: `"digite um valor para a:"`).

> Não existem variáveis reais nem string. Composição de texto com valores numéricos é feita por **vários `imprima` em sequência**.

### 2.3 Declaração de variáveis

```
<tipo>:<identificador>;
```

Exemplo: `inteiro:a;`
A declaração é feita **uma a uma** (múltiplas variáveis na mesma linha é opcional — **vamos suportar apenas uma por declaração** para simplificar).

### 2.4 Operadores

| Categoria   | Operadores                       |
|-------------|----------------------------------|
| Aritméticos | `+`  `-`  `/`  `*`  `(`  `)`     |
| Lógicos     | `E`  `OU`  `NÃO`  `(`  `)`       |
| Relacionais | `>`  `<`  `>=`  `<=`  `=`  `<>` |
| Atribuição  | `<-`                             |

> Atenção: `=` aqui é **operador relacional de igualdade** (não atribuição). Atribuição é `<-`.
> `<>` é "diferente de".
> `<=`, `>=`, `<>`, `<-` são tokens de **dois caracteres** — o scanner precisa fazer lookahead.

### 2.5 Comandos da linguagem

**Estrutura geral:**
```
inicio
   <lista_cmd>
fim
```

**Condicional:**
```
se <exp_lógica>
   então
      <lista_CMD>
   senão
      <lista_CMD>
fim_se
```
> `senão` é **opcional**.

**Laço:**
```
para <inic> até <valor> passo <incremento>
   <lista_cmd>
fim_para
```
> `passo` é opcional; default = `1`.

**E/S (palavras reservadas embutidas):**
- `escreva(<valor_inteiro>)` / `escreva(<variável>)` / `escreva(<string>)`
- `leia(<variável>)`
- `imprima` — também aparece no exemplo do enunciado (aceita variável, constante literal ou inteira).

> **Cada comando termina com `;`**.

### 2.6 Exemplos do enunciado (devem ser tokenizados sem erro)

**Exemplo 1:**
```
inicio
   inteiro:a;
   imprima ("digite um valor para a:");
   leia(a);
   se a = 5
      então
         escreva ;
      senão
         escreva
   fim_se
fim
```

**Exemplo 2:**
```
inicio
   inteiro:b;
   inteiro:c;
   imprima
   leia(c);
   para b=0 até c passo 2
      imprima(b);
   fim_para
fim
```

> Os exemplos do PDF não são 100% rigorosos em pontuação (ex.: faltam `;` em alguns lugares). O **léxico não cobra sintaxe** — ele só quebra a entrada em tokens. Validação de gramática é problema do parser (próximo trabalho).

---

## 3. Tokens — tabela de códigos

Códigos definidos no projeto. **Mantenha esta tabela sincronizada com `tokens.h`.**

### 3.1 Palavras reservadas
| Código | Lexema       |
|-------:|--------------|
| 1      | `inicio`     |
| 2      | `fim`        |
| 3      | `inteiro`    |
| 4      | `se`         |
| 5      | `então`      |
| 6      | `senão`      |
| 7      | `fim_se`     |
| 8      | `para`       |
| 9      | `até`        |
| 10     | `passo`      |
| 11     | `fim_para`   |
| 12     | `leia`       |
| 13     | `imprima`    |
| 14     | `escreva`    |
| 15     | `E`          |
| 16     | `OU`         |
| 17     | `NÃO`        |

### 3.2 Identificadores e constantes
| Código | Categoria                    |
|-------:|------------------------------|
| 100    | `IDENTIFICADOR`              |
| 101    | `CONST_INTEIRA`              |
| 102    | `CONST_LITERAL` (string)     |

### 3.3 Operadores e pontuação
| Código | Lexema      | Descrição              |
|-------:|-------------|------------------------|
| 200    | `+`         | soma                   |
| 201    | `-`         | subtração              |
| 202    | `*`         | multiplicação          |
| 203    | `/`         | divisão                |
| 204    | `(`         | abre parêntese         |
| 205    | `)`         | fecha parêntese        |
| 206    | `>`         | maior                  |
| 207    | `<`         | menor                  |
| 208    | `>=`        | maior-igual            |
| 209    | `<=`        | menor-igual            |
| 210    | `=`         | igualdade (relacional) |
| 211    | `<>`        | diferente              |
| 212    | `<-`        | atribuição             |
| 213    | `:`         | dois-pontos (declaração) |
| 214    | `;`         | fim de comando         |
| 215    | `,`         | vírgula (reserva, caso necessário) |

### 3.4 Tokens especiais
| Código | Nome        |
|-------:|-------------|
| 0      | `EOF_TOKEN` |
| -1     | `ERRO`      |

---

## 4. Decisões de design

- **Linguagem:** C99. Compilar com `gcc -std=c99 -Wall -Wextra -Wpedantic`.
- **Estrutura:** modular, separando responsabilidades:
  - `tokens.h` — enum/macros de códigos de token.
  - `symtable.h/.c` — tabela de símbolos (identificadores) + tabela de palavras reservadas.
  - `lexer.h/.c` — função `proximo_token()` e estado do scanner.
  - `main.c` — driver que simula o parser.
- **Entrada:** programa aceita **argumento posicional** (`./lexer programa.txt`) **ou** lê de **stdin** se nenhum argumento for passado (`./lexer < programa.txt`).
- **Encoding:** o enunciado usa palavras com acento (`então`, `senão`, `até`, `NÃO`). Tratar a entrada como **UTF-8** byte-a-byte; o matching de palavras reservadas compara bytes literais (UTF-8). Documentar isso no README.
- **Case sensitivity:** palavras reservadas em minúsculas são case-sensitive (`inicio`, `fim`, `se`, …). Operadores lógicos `E`, `OU`, `NÃO` são em **maiúsculas** conforme enunciado.
- **Comentários:** o enunciado não menciona — **não tratar** (qualquer caractere não previsto vira erro léxico).
- **Strings:** delimitadas por aspas duplas `"..."`. Sem escapes (não há menção no enunciado).
- **Identificadores:** `[a-zA-Z_][a-zA-Z0-9_]*`. Acentos só são aceitos dentro de palavras reservadas conhecidas.
- **Tabela de símbolos:** armazena apenas identificadores do usuário (não palavras reservadas). Implementação simples com array dinâmico + busca linear (suficiente para a escala do trabalho). Cada identificador novo é inserido uma vez e reutilizado.
- **Tabela de palavras reservadas:** array estático com `{lexema, código}`. Função `eh_palavra_reservada(const char *lex)` retorna o **código** se for reservada, `-1` caso contrário (conforme enunciado).
- **Posição na tabela:**
  - Para identificadores: índice (0-based) na tabela de símbolos.
  - Para constantes literais/inteiras: também guardar em tabela e devolver índice (facilita o sintático depois).
  - Para palavras reservadas e operadores: **não há posição em tabela** — imprimir `-` ou `-1`.
- **Tratamento de erro:** ao encontrar caractere inesperado, imprimir `ERRO LEXICO: linha L, coluna C, caractere 'X'`, devolver token `ERRO` e **seguir adiante** (skip do caractere) para reportar múltiplos erros numa só execução.
- **Rastreio de linha/coluna:** o lexer mantém contadores para mensagens de erro úteis.

### Saída padrão (formato sugerido)

Uma linha por token:
```
<codigo> <lexema> <posicao_tabela>
```
Exemplo (trecho do exemplo 1):
```
1   inicio    -
3   inteiro   -
213 :         -
100 a         0
214 ;         -
13  imprima   -
204 (         -
102 "digite um valor para a:" 0
205 )         -
214 ;         -
0   EOF       -
```

---

## 5. Estrutura do repositório

```
.
├── CLAUDE.md                  # este arquivo
├── README.md                  # instruções de build/uso para humanos
├── Makefile
├── src/
│   ├── tokens.h
│   ├── symtable.h
│   ├── symtable.c
│   ├── lexer.h
│   ├── lexer.c
│   └── main.c
├── tests/
│   ├── exemplo1.txt           # exemplo 1 do enunciado
│   ├── exemplo2.txt           # exemplo 2 do enunciado
│   ├── operadores.txt         # cobre todos os operadores
│   ├── strings.txt            # strings simples e com pontuação
│   ├── erros.txt              # caracteres inválidos para testar recuperação
│   ├── expected/              # saídas esperadas
│   │   ├── exemplo1.out
│   │   ├── exemplo2.out
│   │   └── ...
│   └── run_tests.sh           # roda tudo e faz diff com expected/
└── docs/
    └── trabalho_escrito.md    # esqueleto da monografia (capa, fundamentação, código)
```

---

## 6. Makefile (alvos esperados)

- `make` — compila `bin/lexer`.
- `make run ARGS="tests/exemplo1.txt"` — compila e executa.
- `make test` — roda `tests/run_tests.sh`.
- `make clean` — remove `bin/` e objetos.

Flags padrão: `-std=c99 -Wall -Wextra -Wpedantic -O2`.

---

## 7. API interna (contratos)

### `lexer.h`
```c
typedef struct {
    int  codigo;       // código do token (ver tokens.h)
    int  posicao;      // índice na tabela apropriada; -1 se não aplicável
    char lexema[256];  // texto original do token
    int  linha;
    int  coluna;
} Token;

void  lexer_init_arquivo(FILE *fp);     // configura fonte de entrada
Token proximo_token(void);              // devolve o próximo token; EOF_TOKEN no fim
```

### `symtable.h`
```c
int  st_inserir_ident(const char *lex);     // retorna índice (insere se novo)
int  st_inserir_literal(const char *lex);   // retorna índice (literais únicos)
int  st_inserir_inteiro(const char *lex);   // retorna índice
int  eh_palavra_reservada(const char *lex); // código se for, -1 se não
void st_imprimir(FILE *fp);                 // dump da tabela ao final (opcional)
```

---

## 8. Algoritmo do scanner (esqueleto)

Baseado no "analisador léxico simples" mencionado no enunciado. Em pseudocódigo:

```
proximo_token():
    pular espaços, tabs, quebras de linha (atualizando linha/coluna)
    se EOF: devolver EOF_TOKEN

    c = peek()
    se c é letra ou '_':
        ler enquanto for [a-zA-Z0-9_]
        cod = eh_palavra_reservada(lexema)
        se cod != -1: devolver Token(cod, -1, lexema)
        senão: pos = st_inserir_ident(lexema); devolver Token(IDENT, pos, lexema)

    se c é dígito:
        ler enquanto for dígito
        pos = st_inserir_inteiro(lexema); devolver Token(CONST_INTEIRA, pos, lexema)

    se c == '"':
        ler até próxima '"' (erro se EOF antes); incluir as aspas no lexema (ou não — documentar)
        pos = st_inserir_literal(lexema); devolver Token(CONST_LITERAL, pos, lexema)

    se c é operador/pontuação:
        tratar lookahead para '<=', '>=', '<>', '<-'
        devolver Token correspondente

    senão:
        reportar erro léxico; consumir c; devolver Token(ERRO, -1, lexema)
```

> Atenção especial ao `<`: pode ser `<`, `<=`, `<>` ou `<-`. Faça lookahead de **um** caractere.

---

## 9. Casos de teste mínimos

1. **`exemplo1.txt`** — exemplo 1 do enunciado, na íntegra.
2. **`exemplo2.txt`** — exemplo 2 do enunciado, na íntegra.
3. **`operadores.txt`** — sequência `+ - * / ( ) > < >= <= = <> <- : ; E OU NÃO`.
4. **`strings.txt`** — `imprima("ola"); imprima("teste com numero 5");`.
5. **`numeros.txt`** — `0 1 42 1000`.
6. **`mistura.txt`** — atribuição, comparações, loops aninhados.
7. **`erros.txt`** — caracteres inesperados (`@`, `#`, `&`) para testar recuperação.
8. **`vazio.txt`** — arquivo vazio (só deve devolver `EOF_TOKEN`).
9. **`so_espacos.txt`** — só whitespace.
10. **`string_nao_fechada.txt`** — `"sem fim` — testar erro.

`tests/run_tests.sh` deve rodar cada um, comparar com `tests/expected/*.out` via `diff` e imprimir PASS/FAIL.

---

## 10. Esqueleto do trabalho escrito (`docs/trabalho_escrito.md`)

Estrutura mínima exigida pelo enunciado: **Capa, embasamento teórico e código fonte**. Sugestão expandida:

1. **Capa** — instituição, curso, disciplina, professor, aluno, semestre, título.
2. **Sumário**.
3. **Introdução** — contexto da disciplina, objetivo do TCD, escopo (analisador léxico para Portugol).
4. **Embasamento teórico**
   - O que é um compilador (visão geral das fases).
   - Análise léxica: papel, entrada/saída, relação com o parser.
   - Tokens, lexemas e padrões.
   - Autômatos finitos e expressões regulares.
   - Tabela de símbolos: propósito e estratégias de implementação.
   - Tratamento de palavras reservadas.
5. **Especificação da linguagem Portugol adotada** — copiar a seção 2 deste CLAUDE.md.
6. **Projeto do analisador**
   - Tabela de tokens (seção 3).
   - Arquitetura modular (seção 5).
   - Algoritmo do scanner (seção 8).
   - Estratégia para tabela de símbolos e palavras reservadas.
   - Tratamento de erros.
7. **Implementação** — comentários sobre escolhas em C, organização dos arquivos.
8. **Testes e resultados** — mostrar entrada e saída dos exemplos 1 e 2.
9. **Conclusão** — o que foi aprendido, limitações, próximos passos (parser).
10. **Referências** — Aho, Lam, Sethi, Ullman (Dragon Book); Louden; notas de aula.
11. **Anexos** — código fonte completo impresso.

---

## 11. Critérios de aceitação (Definition of Done)

- [ ] Compila limpo com `-Wall -Wextra -Wpedantic` (zero warnings).
- [ ] Reconhece todos os tokens listados na seção 3.
- [ ] Trata corretamente os operadores de 2 caracteres (`<=`, `>=`, `<>`, `<-`).
- [ ] Distingue palavras reservadas de identificadores via `eh_palavra_reservada`.
- [ ] Tabela de símbolos insere identificador novo apenas uma vez e devolve o mesmo índice em ocorrências repetidas.
- [ ] Devolve `EOF_TOKEN` no fim do arquivo.
- [ ] `main` imprime `(código, posição_na_tabela)` por token, conforme exigido.
- [ ] Suporta entrada por argumento de linha de comando E por stdin.
- [ ] Reporta erros léxicos com linha/coluna e continua a análise.
- [ ] Exemplos 1 e 2 do enunciado tokenizam sem erro léxico.
- [ ] `make test` passa em todos os casos.
- [ ] Trabalho escrito (`docs/trabalho_escrito.md`) preenchido e pronto para impressão.

---

## 12. Instruções operacionais para o Claude

Quando for me ajudar neste projeto:

1. **Antes de codar**, releia este CLAUDE.md inteiro e a seção 3 (tabela de tokens) em particular.
2. **Mudou um código de token?** Atualize aqui E em `tokens.h` no mesmo commit.
3. **Mudou a especificação da linguagem?** Atualize a seção 2 antes de tocar no código.
4. **Não invente regras** que não estão no enunciado. Se algo for ambíguo (ex.: comentários, escapes em string), assuma o mais simples, documente em "Decisões de design" (seção 4) e me avise.
5. **Não use bibliotecas externas** (só libc). É didático.
6. **Estilo de código:** nomes em português quando se referirem a conceitos do enunciado (`proximo_token`, `eh_palavra_reservada`, `tabela_simbolos`); nomes em inglês para conceitos genéricos (`buf`, `pos`, `len`).
7. **Comente em português** — facilita a defesa oral.
8. **Não acrescente features fora do escopo** (ex.: pré-processador, otimizações). Foco no léxico.
9. Antes de declarar "pronto", rode o checklist da seção 11.
