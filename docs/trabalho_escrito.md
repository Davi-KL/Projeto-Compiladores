# Analisador Léxico para Linguagem Portugol

---

## Capa

**Instituição:** Centro Universitário de Brasília — CEUB  
**Faculdade:** FATECS — Faculdade de Tecnologia e Ciências Sociais Aplicadas  
**Curso:** Ciência da Computação  
**Disciplina:** Compiladores  
**Professor:** Leonardo Pol  
**Aluno:** Davi  
**Semestre:** 1º Semestre de 2026  
**Título:** TCD — Projeto A: Implementação de um Analisador Léxico para Linguagem Portugol  

---

## Sumário

1. [Introdução](#1-introdução)
2. [Embasamento Teórico](#2-embasamento-teórico)
   - 2.1 O que é um Compilador
   - 2.2 Análise Léxica
   - 2.3 Tokens, Lexemas e Padrões
   - 2.4 Autômatos Finitos e Expressões Regulares
   - 2.5 Tabela de Símbolos
   - 2.6 Tratamento de Palavras Reservadas
3. [Especificação da Linguagem Portugol](#3-especificação-da-linguagem-portugol)
4. [Projeto do Analisador](#4-projeto-do-analisador)
5. [Implementação](#5-implementação)
6. [Testes e Resultados](#6-testes-e-resultados)
7. [Conclusão](#7-conclusão)
8. [Referências](#8-referências)
9. [Anexos — Código Fonte](#9-anexos--código-fonte)

---

## 1. Introdução

A construção de compiladores é uma das áreas mais fundamentais da Ciência da Computação. Compiladores são programas que traduzem código escrito em uma linguagem de alto nível para uma representação de mais baixo nível, como código de máquina ou bytecode. O estudo de seus mecanismos internos desenvolve habilidades essenciais de programação, como análise formal de linguagens, reconhecimento de padrões e estruturação modular de sistemas complexos.

Este trabalho foi desenvolvido como parte do TCD — Projeto A da disciplina de Compiladores do curso de Ciência da Computação do CEUB/FATECS, sob orientação do professor Leonardo Pol. O objetivo é implementar a primeira fase de um compilador: o **analisador léxico** (também chamado de *scanner* ou *tokenizador*) para uma linguagem didática do tipo Portugol.

O analisador léxico tem a responsabilidade de ler o código-fonte caractere por caractere e agrupá-los em unidades significativas chamadas **tokens**, como palavras reservadas, identificadores, operadores e constantes. Esses tokens são então entregues, um por vez, ao analisador sintático (parser), que compõe a fase seguinte do compilador.

A linguagem alvo é um subconjunto de Portugol — uma pseudolinguagem em português utilizada para ensino de algoritmos — com suporte a declarações de variáveis inteiras, estruturas condicionais, laços de repetição e operações de entrada e saída.

A implementação foi feita na linguagem C (padrão C99), sem uso de bibliotecas externas além da biblioteca padrão (libc), seguindo uma arquitetura modular com separação clara de responsabilidades entre os módulos de tokenização, tabela de símbolos e programa principal.

---

## 2. Embasamento Teórico

### 2.1 O que é um Compilador

Um compilador é um programa que recebe como entrada um texto escrito em uma linguagem fonte e produz como saída um texto equivalente em uma linguagem alvo. O processo de compilação é tipicamente dividido em fases sequenciais:

1. **Análise Léxica:** lê o texto fonte e produz uma sequência de tokens.
2. **Análise Sintática:** consome os tokens e verifica se a estrutura gramatical é válida, construindo uma árvore sintática.
3. **Análise Semântica:** percorre a árvore sintática verificando coerência de tipos e outras regras de significado.
4. **Geração de código intermediário:** transforma a árvore anotada em uma representação independente de máquina.
5. **Otimização:** melhora o código intermediário.
6. **Geração de código:** produz o código final na linguagem alvo.

Este trabalho abrange exclusivamente a **fase 1 — análise léxica**.

### 2.2 Análise Léxica

O analisador léxico (ou *lexer*) é o componente do compilador que realiza a leitura do programa fonte e o divide em unidades atômicas de significado: os tokens. Ele atua como uma interface entre o texto bruto e o restante do compilador.

Do ponto de vista do parser, o lexer funciona como uma função chamada repetidamente que, a cada chamada, devolve o próximo token da entrada. Isso permite que o parser não precise se preocupar com detalhes como espaços em branco, quebras de linha ou a distinção entre caracteres individuais e agrupamentos.

Responsabilidades do analisador léxico:

- Ignorar espaços em branco e quebras de linha.
- Reconhecer palavras reservadas, identificadores, constantes e operadores.
- Reportar erros léxicos (caracteres inválidos).
- Manter rastreamento de linha e coluna para mensagens de erro precisas.
- Preencher a tabela de símbolos com identificadores encontrados.

### 2.3 Tokens, Lexemas e Padrões

Três conceitos centrais da análise léxica devem ser distinguidos:

- **Token:** par *(categoria, valor)* que representa uma unidade léxica. A categoria é um código inteiro (ex.: `100` para identificador); o valor é, opcionalmente, um índice na tabela de símbolos.
- **Lexema:** sequência de caracteres do código-fonte que corresponde a um token. Por exemplo, `inicio` é o lexema do token de código `1`.
- **Padrão:** regra que descreve o conjunto de lexemas que pertencem a uma categoria. Identificadores seguem o padrão `[a-zA-Z_][a-zA-Z0-9_]*`; constantes inteiras seguem `[0-9]+`.

### 2.4 Autômatos Finitos e Expressões Regulares

Os padrões léxicos são formalmente descritos por **expressões regulares**, e o reconhecedor correspondente é um **autômato finito determinístico (AFD)**. Um AFD é uma máquina de estados com:

- Um conjunto finito de estados.
- Um estado inicial.
- Um conjunto de estados de aceitação.
- Uma função de transição que, dado um estado e um símbolo de entrada, indica o próximo estado.

Na prática, o scanner deste projeto implementa manualmente o AFD por meio de um laço com desvios condicionais (`if`/`switch`), o que é equivalente e mais eficiente para uma linguagem pequena do que gerar o AFD automaticamente com ferramentas como `lex`/`flex`.

O reconhecimento de tokens de dois caracteres (como `<=`, `>=`, `<>` e `<-`) exige **lookahead de um caractere**: ao ler `<`, o scanner lê o próximo caractere para decidir qual dos quatro tokens emitir e, se nenhum se encaixar, devolve o caractere lido de volta ao buffer.

### 2.5 Tabela de Símbolos

A tabela de símbolos é uma estrutura de dados mantida pelo compilador para armazenar informações sobre os identificadores e constantes encontrados no programa fonte. Sua principal utilidade é evitar que o parser precise lidar diretamente com strings — ele referencia entradas pelo índice numérico, o que é mais eficiente.

Neste projeto a tabela de símbolos foi implementada como um **array dinâmico com busca linear**, suficiente para a escala de programas Portugol. São mantidas três tabelas separadas:

- **Tabela de identificadores:** variáveis declaradas pelo programador.
- **Tabela de constantes inteiras:** literais numéricos como `0`, `5`, `42`.
- **Tabela de literais string:** cadeias de caracteres como `"ola mundo"`.

A separação em tabelas distintas permite que o parser futuro saiba imediatamente o tipo da constante pelo código do token, sem precisar inspecionar a tabela.

Cada inserção verifica se o lexema já existe (busca linear) antes de adicionar, garantindo que cada símbolo distinto ocupa exatamente uma entrada, independentemente de quantas vezes apareça no programa.

### 2.6 Tratamento de Palavras Reservadas

Palavras reservadas (como `inicio`, `fim`, `se`, `então`) são tratadas com uma estratégia específica recomendada pela literatura:

1. O scanner lê a sequência de caracteres como se fosse um identificador comum.
2. Após montar o lexema, consulta uma **tabela de palavras reservadas** (array estático com pares `{lexema, código}`).
3. Se o lexema for encontrado na tabela, emite o token da palavra reservada (com código entre 1 e 17).
4. Caso contrário, é tratado como identificador de usuário (código 100) e inserido na tabela de símbolos.

Essa abordagem é preferível a tratar palavras reservadas como casos especiais no meio do reconhecimento de identificadores, pois mantém o código limpo e facilita a adição de novas palavras reservadas.

---

## 3. Especificação da Linguagem Portugol

### 3.1 Tipos de Dados

A linguagem suporta apenas o tipo **`inteiro`**, equivalente ao `int` da linguagem C.

### 3.2 Constantes

- **Numéricas inteiras:** sequências de dígitos decimais (ex.: `5`, `0`, `42`).
- **Literais string:** sequências de caracteres delimitadas por aspas duplas (ex.: `"digite um valor para a:"`). Não há sequências de escape.

### 3.3 Declaração de Variáveis

```
inteiro:<identificador>;
```

Exemplo: `inteiro:a;`

Uma variável por declaração.

### 3.4 Operadores

| Categoria   | Operadores                          |
|-------------|-------------------------------------|
| Aritméticos | `+`  `-`  `*`  `/`  `(`  `)`        |
| Lógicos     | `E`  `OU`  `NÃO`  `(`  `)`          |
| Relacionais | `>`  `<`  `>=`  `<=`  `=`  `<>`    |
| Atribuição  | `<-`                                |

O operador `=` é **relacional** (igualdade), não atribuição. Atribuição é feita com `<-`. O operador `<>` representa "diferente de".

### 3.5 Comandos

**Estrutura de programa:**
```
inicio
   <lista de comandos>
fim
```

**Condicional:**
```
se <expressão lógica>
   então
      <lista de comandos>
   senão
      <lista de comandos>
fim_se
```
O bloco `senão` é opcional.

**Laço:**
```
para <inicialização> até <valor> passo <incremento>
   <lista de comandos>
fim_para
```
O `passo` é opcional (padrão = 1).

**Entrada e saída:**
- `leia(<variável>)` — lê um valor inteiro.
- `escreva(<valor>)` — exibe um valor.
- `imprima(<valor>)` — exibe um valor ou string.

Cada comando termina com `;`.

### 3.6 Exemplos da Linguagem

**Exemplo 1 — condicional:**
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

**Exemplo 2 — laço:**
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

---

## 4. Projeto do Analisador

### 4.1 Tabela de Tokens

Os tokens da linguagem são classificados em quatro grupos e identificados por códigos inteiros:

**Palavras reservadas (1–17):**

| Código | Lexema     |
|-------:|------------|
| 1      | `inicio`   |
| 2      | `fim`      |
| 3      | `inteiro`  |
| 4      | `se`       |
| 5      | `então`    |
| 6      | `senão`    |
| 7      | `fim_se`   |
| 8      | `para`     |
| 9      | `até`      |
| 10     | `passo`    |
| 11     | `fim_para` |
| 12     | `leia`     |
| 13     | `imprima`  |
| 14     | `escreva`  |
| 15     | `E`        |
| 16     | `OU`       |
| 17     | `NÃO`      |

**Identificadores e constantes (100–102):**

| Código | Categoria                |
|-------:|--------------------------|
| 100    | Identificador            |
| 101    | Constante inteira        |
| 102    | Constante literal/string |

**Operadores e pontuação (200–215):**

| Código | Lexema | Descrição               |
|-------:|--------|-------------------------|
| 200    | `+`    | Soma                    |
| 201    | `-`    | Subtração               |
| 202    | `*`    | Multiplicação           |
| 203    | `/`    | Divisão                 |
| 204    | `(`    | Abre parêntese          |
| 205    | `)`    | Fecha parêntese         |
| 206    | `>`    | Maior que               |
| 207    | `<`    | Menor que               |
| 208    | `>=`   | Maior ou igual          |
| 209    | `<=`   | Menor ou igual          |
| 210    | `=`    | Igualdade (relacional)  |
| 211    | `<>`   | Diferente de            |
| 212    | `<-`   | Atribuição              |
| 213    | `:`    | Dois-pontos             |
| 214    | `;`    | Ponto e vírgula         |
| 215    | `,`    | Vírgula                 |

**Tokens especiais:**

| Código | Nome        |
|-------:|-------------|
| 0      | EOF (fim)   |
| -1     | ERRO léxico |

### 4.2 Arquitetura Modular

O projeto é dividido em quatro módulos com responsabilidades bem definidas:

| Módulo          | Arquivo(s)          | Responsabilidade                                   |
|-----------------|---------------------|----------------------------------------------------|
| Tokens          | `tokens.h`          | Definições dos códigos de token (macros)           |
| Tabela de símbolos | `symtable.h/.c`  | Tabelas de ident./const. e tabela de palavras res. |
| Lexer           | `lexer.h/.c`        | Scanner: `proximo_token()` e estado interno        |
| Driver          | `main.c`            | Simula o parser, imprime tokens até EOF            |

### 4.3 Algoritmo do Scanner

O algoritmo central é a função `proximo_token()`, que implementa um autômato finito manualmente:

```
proximo_token():
    1. Pular espaços, tabs e quebras de linha (atualizando linha/coluna)
    2. Se EOF: devolver EOF_TOKEN

    3. Se o caractere inicia identificador (letra, '_' ou byte UTF-8 >= 0xC2):
       a. Ler enquanto for [a-zA-Z0-9_] ou byte de continuação UTF-8
       b. Consultar tabela de palavras reservadas
       c. Se reservada: devolver token da palavra reservada (posição = -1)
       d. Senão: inserir na tab_ident, devolver IDENTIFICADOR com índice

    4. Se o caractere é dígito:
       a. Ler enquanto for dígito
       b. Inserir na tab_inteiro, devolver CONST_INTEIRA com índice

    5. Se o caractere é '"':
       a. Ler até próximo '"' (incluindo as aspas no lexema)
       b. Se EOF antes do fechamento: reportar erro de string não fechada
       c. Inserir na tab_literal, devolver CONST_LITERAL com índice

    6. Operadores de um caractere (+, -, *, /, (, ), =, :, ;, ,):
       a. Devolver o token correspondente diretamente

    7. Operadores com lookahead (>, <):
       a. '>' seguido de '=': devolver '>=' (código 208)
       b. '>' sozinho: devolver '>' (código 206)
       c. '<' seguido de '=': devolver '<=' (código 209)
       d. '<' seguido de '>': devolver '<>' (código 211)
       e. '<' seguido de '-': devolver '<-' (código 212)
       f. '<' sozinho: devolver '<' (código 207)

    8. Qualquer outro caractere:
       a. Imprimir "ERRO LEXICO: linha L, coluna C, caractere 'X'"
       b. Consumir o caractere e devolver token ERRO
       c. (recuperação: o scanner continua para o próximo token)
```

### 4.4 Estratégia para UTF-8

A linguagem Portugol usa palavras com acentos (`então`, `senão`, `até`, `NÃO`). A entrada é tratada como UTF-8 byte a byte: bytes de início de sequência multibyte (valores `>= 0xC2`) e bytes de continuação (`0x80`–`0xBF`) são aceitos dentro de identificadores e palavras reservadas. A comparação com a tabela de palavras reservadas usa `strcmp`, que compara bytes, funcionando corretamente para UTF-8.

### 4.5 Tratamento de Erros

Ao encontrar um caractere inválido, o lexer:
1. Imprime a mensagem de erro em `stderr` com linha e coluna.
2. Consome o caractere problemático.
3. Devolve um token `ERRO` (código -1) ao chamador.
4. Continua o processamento normalmente, permitindo que múltiplos erros sejam reportados em uma única execução.

---

## 5. Implementação

### 5.1 Escolhas em C

A implementação usa **C99** com as flags `-Wall -Wextra -Wpedantic` para garantir código limpo e portável. Nenhuma biblioteca externa foi usada além da libc padrão (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`).

**Lookahead:** o lexer mantém um buffer de um caractere (`char_devolvido`) que simula o `ungetc` de forma controlada, permitindo que o scanner "devolva" um caractere quando o lookahead não formou um token de dois caracteres.

**Tabela de símbolos dinâmica:** implementada com `malloc`/`realloc` e capacidade inicial de 64 entradas, dobrando quando necessário. Cada entrada é uma string alocada com `strdup`.

**Convenções de nomenclatura:**
- Nomes em português para conceitos do domínio da linguagem: `proximo_token`, `eh_palavra_reservada`, `tab_ident`.
- Nomes em inglês para conceitos genéricos de programação: `buf`, `len`, `pos`, `t`.

### 5.2 Interface da Função Principal

```c
Token proximo_token(void);
```

Retorna uma estrutura `Token` com:
- `codigo` — código inteiro do token reconhecido.
- `posicao` — índice na tabela correspondente; `-1` se não aplicável.
- `lexema[256]` — texto original do token.
- `linha`, `coluna` — posição no arquivo fonte para diagnóstico.

### 5.3 Entrada

O programa aceita duas formas de entrada:
```
./lexer programa.txt     # argumento posicional
./lexer < programa.txt   # redirecionamento de stdin
```

### 5.4 Formato de Saída

Uma linha por token:
```
COD    NOME           LEXEMA                         POS
---    ----           ------                         ---
<cod>  <nome>         <lexema>                       <pos|->
```

- **COD** — código inteiro do token.
- **NOME** — nome simbólico do token (ex.: `TK_INICIO`, `TK_IDENT`, `EOF_TOKEN`).
- **LEXEMA** — texto original lido do programa fonte.
- **POS** — índice na tabela de símbolos; `-` para palavras reservadas e operadores.

---

## 6. Testes e Resultados

Os testes foram executados com `bash tests/run_tests.sh`, que compara a saída do lexer com arquivos de referência em `tests/expected/`.

### 6.1 Exemplo 1 — Condicional

**Entrada (`tests/exemplo1.txt`):**
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

**Saída:**
```
COD    NOME           LEXEMA                         POS
---    ----           ------                         ---
1      TK_INICIO      inicio                         -
3      TK_INTEIRO     inteiro                        -
213    TK_DPONTOS     :                              -
100    TK_IDENT       a                              0
214    TK_PVIRG       ;                              -
13     TK_IMPRIMA     imprima                        -
204    TK_ABRE_P      (                              -
102    TK_CONST_LIT   "digite um valor para a:"      0
205    TK_FECHA_P     )                              -
214    TK_PVIRG       ;                              -
12     TK_LEIA        leia                           -
204    TK_ABRE_P      (                              -
100    TK_IDENT       a                              0
205    TK_FECHA_P     )                              -
214    TK_PVIRG       ;                              -
4      TK_SE          se                             -
100    TK_IDENT       a                              0
210    TK_IGUAL       =                              -
101    TK_CONST_INT   5                              0
5      TK_ENTAO       então                          -
14     TK_ESCREVA     escreva                        -
214    TK_PVIRG       ;                              -
6      TK_SENAO       senão                          -
14     TK_ESCREVA     escreva                        -
7      TK_FIM_SE      fim_se                         -
2      TK_FIM         fim                            -
0      EOF_TOKEN      EOF                            -
```

Observações:
- O identificador `a` aparece cinco vezes e sempre com `POS = 0` (mesmo índice na tabela).
- As palavras reservadas acentuadas (`então`, `senão`) são reconhecidas corretamente via comparação UTF-8 byte a byte.

### 6.2 Exemplo 2 — Laço

**Entrada (`tests/exemplo2.txt`):**
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

**Saída:**
```
COD    NOME           LEXEMA                         POS
---    ----           ------                         ---
1      TK_INICIO      inicio                         -
3      TK_INTEIRO     inteiro                        -
213    TK_DPONTOS     :                              -
100    TK_IDENT       b                              0
214    TK_PVIRG       ;                              -
3      TK_INTEIRO     inteiro                        -
213    TK_DPONTOS     :                              -
100    TK_IDENT       c                              1
214    TK_PVIRG       ;                              -
13     TK_IMPRIMA     imprima                        -
12     TK_LEIA        leia                           -
204    TK_ABRE_P      (                              -
100    TK_IDENT       c                              1
205    TK_FECHA_P     )                              -
214    TK_PVIRG       ;                              -
8      TK_PARA        para                           -
100    TK_IDENT       b                              0
210    TK_IGUAL       =                              -
101    TK_CONST_INT   0                              0
9      TK_ATE         até                            -
100    TK_IDENT       c                              1
10     TK_PASSO       passo                          -
101    TK_CONST_INT   2                              1
13     TK_IMPRIMA     imprima                        -
204    TK_ABRE_P      (                              -
100    TK_IDENT       b                              0
205    TK_FECHA_P     )                              -
214    TK_PVIRG       ;                              -
11     TK_FIM_PARA    fim_para                       -
2      TK_FIM         fim                            -
0      EOF_TOKEN      EOF                            -
```

Observações:
- `b` e `c` recebem índices 0 e 1 respectivamente na tabela de identificadores.
- Nas reutilizações posteriores (`leia(c)`, `para b=0`, `imprima(b)`), os índices são reutilizados corretamente.
- `até` (com acento) é reconhecida como palavra reservada de código 9.

### 6.3 Resultado Geral da Suíte de Testes

| Teste               | Resultado |
|---------------------|-----------|
| `exemplo1`          | PASS      |
| `exemplo2`          | PASS      |
| `operadores`        | PASS      |
| `strings`           | PASS      |
| `numeros`           | PASS      |
| `erros`             | PASS      |
| `vazio`             | PASS      |
| `so_espacos`        | PASS      |
| `string_nao_fechada`| PASS      |
| **Total**           | **9/9**   |

---

## 7. Conclusão

Este trabalho implementou com sucesso um analisador léxico completo para uma linguagem do tipo Portugol em C99. Os principais aprendizados obtidos foram:

**Sobre análise léxica:** o scanner é a interface mais próxima do texto fonte no compilador. Detalhes aparentemente simples — como o tratamento de caracteres multibyte (UTF-8), o lookahead para operadores de dois caracteres e a recuperação de erros sem interrupção — exigem atenção cuidadosa para funcionarem corretamente em todos os casos.

**Sobre tabelas de símbolos:** a separação da tabela em três subtabelas (identificadores, inteiros, literais) e a estratégia de inserção sem duplicatas prepara a base para o analisador sintático, que poderá referenciar símbolos por índice numérico de forma eficiente.

**Sobre palavras reservadas:** a abordagem de reconhecer como identificador e depois consultar uma tabela auxiliar é mais robusta e extensível do que tratar palavras reservadas como casos especiais no próprio reconhecimento de caracteres.

**Limitações identificadas:**
- O buffer de lexema é limitado a 255 bytes; lexemas maiores são truncados silenciosamente.
- A busca linear na tabela de símbolos tem complexidade O(n), aceitável para programas pequenos mas não escalável.
- Não há suporte a comentários (o enunciado não os menciona; qualquer caractere não previsto gera erro léxico).

**Próximos passos:** a fase seguinte é o **analisador sintático** (parser), que consumirá os tokens produzidos por este léxico e verificará se a sequência de tokens obedece à gramática da linguagem Portugol, construindo uma árvore sintática abstrata para uso nas fases subsequentes.

---

## 8. Referências

- AHO, Alfred V.; LAM, Monica S.; SETHI, Ravi; ULLMAN, Jeffrey D. **Compiladores: Princípios, Técnicas e Ferramentas**. 2. ed. São Paulo: Pearson Addison-Wesley, 2008. (Dragon Book)
- LOUDEN, Kenneth C. **Compiladores: Princípios e Práticas**. São Paulo: Pioneira Thomson Learning, 2004.
- POL, Leonardo. **Material de aula — Disciplina de Compiladores**. CEUB/FATECS, 2026.
- ISO/IEC 9899:1999. **Programming Languages — C** (C99 Standard).

---

## 9. Anexos — Código Fonte

### 9.1 `src/tokens.h`

```c
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
```

### 9.2 `src/symtable.h`

```c
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
```

### 9.3 `src/symtable.c`

```c
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
```

### 9.4 `src/lexer.h`

```c
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
```

### 9.5 `src/lexer.c`

```c
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
```

### 9.6 `src/main.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "symtable.h"
#include "tokens.h"

static const char *nome_token(int codigo) {
    switch (codigo) {
        case EOF_TOKEN:    return "EOF_TOKEN";
        case ERRO:         return "ERRO";
        case TK_INICIO:    return "TK_INICIO";
        case TK_FIM:       return "TK_FIM";
        case TK_INTEIRO:   return "TK_INTEIRO";
        case TK_SE:        return "TK_SE";
        case TK_ENTAO:     return "TK_ENTAO";
        case TK_SENAO:     return "TK_SENAO";
        case TK_FIM_SE:    return "TK_FIM_SE";
        case TK_PARA:      return "TK_PARA";
        case TK_ATE:       return "TK_ATE";
        case TK_PASSO:     return "TK_PASSO";
        case TK_FIM_PARA:  return "TK_FIM_PARA";
        case TK_LEIA:      return "TK_LEIA";
        case TK_IMPRIMA:   return "TK_IMPRIMA";
        case TK_ESCREVA:   return "TK_ESCREVA";
        case TK_E:         return "TK_E";
        case TK_OU:        return "TK_OU";
        case TK_NAO:       return "TK_NAO";
        case TK_IDENT:     return "TK_IDENT";
        case TK_CONST_INT: return "TK_CONST_INT";
        case TK_CONST_LIT: return "TK_CONST_LIT";
        case TK_MAIS:      return "TK_MAIS";
        case TK_MENOS:     return "TK_MENOS";
        case TK_MULT:      return "TK_MULT";
        case TK_DIV:       return "TK_DIV";
        case TK_ABRE_P:    return "TK_ABRE_P";
        case TK_FECHA_P:   return "TK_FECHA_P";
        case TK_MAIOR:     return "TK_MAIOR";
        case TK_MENOR:     return "TK_MENOR";
        case TK_MAIOR_I:   return "TK_MAIOR_I";
        case TK_MENOR_I:   return "TK_MENOR_I";
        case TK_IGUAL:     return "TK_IGUAL";
        case TK_DIFER:     return "TK_DIFER";
        case TK_ATRIB:     return "TK_ATRIB";
        case TK_DPONTOS:   return "TK_DPONTOS";
        case TK_PVIRG:     return "TK_PVIRG";
        case TK_VIRG:      return "TK_VIRG";
        default:           return "?";
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
```
