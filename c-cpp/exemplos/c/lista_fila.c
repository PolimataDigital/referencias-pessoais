#include "../verificar.h"
#include <stdbool.h>

typedef struct No {
    int valor;
    struct No *proximo;
} No;

static bool inserir_inicio(No **cabeca, int valor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return false;
    *novo = (No){.valor = valor, .proximo = *cabeca};
    *cabeca = novo;
    return true;
}

static bool remover_primeiro(No **cabeca, int valor) {
    No **ligacao = cabeca;
    while (*ligacao != NULL && (*ligacao)->valor != valor)
        ligacao = &(*ligacao)->proximo;
    if (*ligacao == NULL) return false;
    No *removido = *ligacao;
    *ligacao = removido->proximo;
    free(removido);
    return true;
}

static void destruir_lista(No **cabeca) {
    while (*cabeca != NULL) {
        No *proximo = (*cabeca)->proximo;
        free(*cabeca);
        *cabeca = proximo;
    }
}

enum { CAPACIDADE = 4 };
typedef struct {
    int dados[CAPACIDADE];
    size_t frente;
    size_t tamanho;
} Fila;

static bool enfileirar(Fila *f, int valor) {
    if (f->tamanho == CAPACIDADE) return false;
    size_t fim = (f->frente + f->tamanho) % CAPACIDADE;
    f->dados[fim] = valor;
    ++f->tamanho;
    return true;
}

static bool desenfileirar(Fila *f, int *saida) {
    if (f->tamanho == 0 || saida == NULL) return false;
    *saida = f->dados[f->frente];
    f->frente = (f->frente + 1) % CAPACIDADE;
    --f->tamanho;
    return true;
}

int main(void) {
    No *cabeca = NULL;
    REF_CHECK(!remover_primeiro(&cabeca, 9));
    REF_CHECK(inserir_inicio(&cabeca, 1));
    REF_CHECK(inserir_inicio(&cabeca, 2));
    REF_CHECK(inserir_inicio(&cabeca, 3));
    REF_CHECK(remover_primeiro(&cabeca, 2));
    REF_CHECK(cabeca->valor == 3 && cabeca->proximo->valor == 1);
    REF_CHECK(remover_primeiro(&cabeca, 3));
    REF_CHECK(cabeca->valor == 1);
    destruir_lista(&cabeca);
    REF_CHECK(cabeca == NULL);

    Fila fila = {0};
    int valor = -1;
    REF_CHECK(!desenfileirar(&fila, &valor));
    for (int i = 0; i < CAPACIDADE; ++i) REF_CHECK(enfileirar(&fila, i));
    REF_CHECK(!enfileirar(&fila, 99));
    REF_CHECK(desenfileirar(&fila, &valor) && valor == 0);
    REF_CHECK(enfileirar(&fila, 4)); /* wrap no buffer */
    for (int i = 1; i <= CAPACIDADE; ++i)
        REF_CHECK(desenfileirar(&fila, &valor) && valor == i);
    REF_CHECK(!desenfileirar(&fila, &valor));
    return ref_concluir();
}
