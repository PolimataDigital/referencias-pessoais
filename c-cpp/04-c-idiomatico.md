# 04 · C idiomático: funções, módulos e APIs

[Índice](../referencia-c-cpp.md) · Intermediário a avançado · C17 salvo indicação

## Declarar, definir e separar

O header declara o contrato público. O `.c` define a implementação. Cada `.c` é compilado separadamente; o linker resolve referências entre objetos. Não inclua arquivos `.c` para “resolver” símbolos ausentes.

```c
/* operacoes.h */
#ifndef REF_OPERACOES_H
#define REF_OPERACOES_H

int comparar_int(int a, int b);

#endif
```

```c
/* operacoes.c */
#include "operacoes.h"

int comparar_int(int a, int b) {
    return (a > b) - (a < b); /* evita overflow de a - b */
}
```

| Palavra/recurso | Uso | Armadilha |
|---|---|---|
| `static` em função no `.c` | Ligação interna, implementação privada | Não a torna função de classe |
| `static` em variável local | Duração estática, mantém valor entre chamadas | Estado compartilhado e concorrência |
| `extern` | Declara objeto definido em outra unidade | Não duplicar definição em cada header |
| `static inline` em header C | Pequena função interna por unidade | `inline` isolado tem regras de linkage específicas em C |
| Include guard | Evitar inclusão repetida na unidade | Não resolve múltiplas definições entre unidades |
| `#pragma once` | Alternativa suportada por compiladores comuns | Não faz parte do padrão ISO C/C++ |

Headers devem incluir o que usam. Prefira prefixo de projeto para nomes públicos (`estoque_abrir`), pois C não tem namespaces. Evite identificadores reservados como nomes começando com `__` ou `_` seguido de maiúscula.

## Struct, enum e union

```c
typedef struct {
    int id;
    double preco;
} Produto;

Produto p = {.id = 7, .preco = 12.5}; /* designadores: C99+ */

typedef enum { VALOR_INT, VALOR_REAL } TipoValor;
typedef struct {
    TipoValor tipo;
    union { int inteiro; double real; } dado;
} Valor;

Valor v = {.tipo = VALOR_INT, .dado.inteiro = 42};
```

Mantenha discriminador e membro ativo coerentes. Não trate a `union` como conversão numérica. Regras de leitura de representação diferem entre C e C++; para formato externo, serialize explicitamente. Estruturas podem ter padding; `memcmp` não é comparação semântica geral de structs.

Em C++20, designadores existem para agregados, mas são mais restritos: seguem a ordem de declaração e não aceitam todas as formas C (como designadores de índice). `struct` e `class` C++ diferem principalmente no acesso e na herança padrão, não na possibilidade de ter métodos.

## Contrato de função: tamanho, posse e erro

Uma API de buffer deve declarar:

1. Se entrada pode ser `NULL`, e em que condições.
2. Se o tamanho é bytes, elementos ou capacidade incluindo terminador.
3. Se o destino pode se sobrepor à origem.
4. O estado do destino e parâmetros de saída em caso de falha.
5. Quem libera cada recurso retornado.

```c
typedef enum { REF_OK, REF_ARGUMENTO, REF_SEM_MEMORIA } RefStatus;

/* Copia n inteiros. Aceita origem == NULL somente se n == 0.
   No sucesso, *saida pertence ao chamador (free); no erro, *saida é NULL. */
RefStatus copiar(const int *origem, size_t n, int **saida);
```

Não use `errno` como estado global genérico sem contrato. Consulte-o somente quando a função o documenta e indica falha; algumas conversões exigem zerá-lo antes da chamada. Um valor não zero antigo não prova que a última operação falhou.

## Tipo opaco

```c
/* cache.h: apenas formato da interface */
typedef struct Cache Cache;
Cache *cache_criar(void);       /* NULL no erro; cria um dono */
void cache_destruir(Cache *c);  /* definir se aceita NULL */
int cache_buscar(const Cache *c, int chave, int *valor);
```

O `.c` define `struct Cache { ... };`. Isso esconde layout e centraliza invariantes. O chamador não faz `sizeof(Cache)` nem acessa campos. Quem cria deve oferecer a função de destruição, especialmente entre bibliotecas/CRTs diferentes.

## Callback com contexto

```c
#include <stddef.h>

typedef void (*Visitante)(int valor, void *contexto);

/* Exige v válido para n elementos e callback não nulo. */
void visitar(const int *v, size_t n, Visitante callback, void *contexto) {
    for (size_t i = 0; i < n; ++i) callback(v[i], contexto);
}

static void contar_positivos(int valor, void *contexto) {
    size_t *contador = contexto;
    if (valor > 0) ++*contador;
}
```

`void *` não transporta informação de tipo: os dois lados devem combinar o tipo e a validade do contexto. Callback assíncrono exige que o contexto sobreviva até o fim do uso. Em C++ uma lambda sem captura pode converter para ponteiro de função compatível; uma lambda com captura precisa de outro mecanismo/contexto.

## Limpeza centralizada

`goto` para um ponto de limpeza pode simplificar funções C com várias aquisições:

```c
/* Fragmento: headers stdio.h e stdlib.h */
int processar_arquivo(const char *caminho) {
    int resultado = -1;
    FILE *arquivo = NULL;
    char *buffer = NULL;
    arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) goto fim;
    buffer = malloc(4096);
    if (buffer == NULL) goto fim;
    /* processamento: conferir leituras e definir resultado */
    resultado = 0;
fim:
    free(buffer);
    if (arquivo != NULL && fclose(arquivo) != 0) resultado = -1;
    return resultado;
}
```

Inicialize os recursos antes dos saltos e libere em ordem inversa à aquisição quando houver dependências. Em C++, RAII normalmente elimina essa necessidade.

## Macros, genéricos e verificação estática

```c
/* Evitar: argumentos podem ser avaliados duas vezes. */
#define REF_QUADRADO(x) ((x) * (x))
/* REF_QUADRADO(i++) é incorreto: não use efeitos colaterais aqui. */

static inline int quadrado_int(int x) { return x * x; } /* sem overflow */
static inline double quadrado_double(double x) { return x * x; }

/* C11: associação de tipo; só a expressão selecionada é chamada. */
#define ref_quadrado(x) _Generic((x), \
    int: quadrado_int, double: quadrado_double)(x)
```

`_Generic` não é template C++; enumere os tipos admitidos. Macros com várias instruções usam `do { ... } while (0)` para formar uma unidade sintática, mas isso não elimina riscos de captura de nomes/avaliação. Prefira funções quando possível.

`_Static_assert(condicao, "mensagem")` (C11) valida constante em compilação. `_Alignof(T)` consulta alinhamento. `offsetof(T, campo)` (`<stddef.h>`) consulta offset apropriado; não torna o layout portátil.

## Arrays flexíveis e baixo nível

```c
/* C99+: último membro, sem tamanho; inclui stddef.h */
struct Bloco {
    size_t tamanho;
    unsigned char bytes[];
};
```

Uma alocação pode reservar `sizeof(struct Bloco) + n` bytes, após verificar `n <= SIZE_MAX - sizeof(struct Bloco)`. Guarde `tamanho = n`, não copie esperando que `sizeof` inclua a cauda. C++ padrão não oferece membro de array flexível.

Bit-fields são úteis para certos layouts locais, mas ordem/alocação não constituem formato de rede portátil. Prefira máscaras e serialização explícita. `volatile` pode ser pertinente a acesso a hardware segundo a plataforma; não fornece exclusão mútua nem protocolo de memória entre threads.

`setjmp`/`longjmp` não substituem uma política de erros estruturada. Há restrições sobre variáveis automáticas modificadas e, em C++, saltos que evitariam destrutores podem ser UB. Sinais do sistema também não autorizam chamar qualquer função de biblioteca no handler.

