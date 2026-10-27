#include "../verificar.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int *dados;
    size_t tamanho;
    size_t capacidade;
} Vetor;

/* Vetor inicializado e invariante valida; falha preserva o objeto. */
static bool vetor_reservar(Vetor *v, size_t capacidade) {
    if (capacidade <= v->capacidade) return true;
    if (capacidade > SIZE_MAX / sizeof *v->dados) return false;
    int *novo = realloc(v->dados, capacidade * sizeof *v->dados);
    if (novo == NULL) return false;
    v->dados = novo;
    v->capacidade = capacidade;
    return true;
}

static bool vetor_adicionar(Vetor *v, int valor) {
    const size_t maximo = SIZE_MAX / sizeof *v->dados;
    if (v->tamanho == maximo) return false;
    if (v->tamanho == v->capacidade) {
        size_t nova = v->capacidade == 0 ? 4 : v->capacidade;
        if (v->capacidade != 0)
            nova = nova > maximo / 2 ? maximo : nova * 2;
        if (nova > maximo) nova = maximo;
        if (!vetor_reservar(v, nova)) return false;
    }
    v->dados[v->tamanho++] = valor;
    return true;
}

static bool vetor_remover(Vetor *v, size_t indice) {
    if (indice >= v->tamanho) return false;
    for (size_t i = indice; i + 1 < v->tamanho; ++i)
        v->dados[i] = v->dados[i + 1];
    --v->tamanho;
    return true;
}

static void vetor_destruir(Vetor *v) {
    free(v->dados);
    *v = (Vetor){0};
}

int main(void) {
    Vetor v = {0};
    REF_CHECK(!vetor_remover(&v, 0));
    REF_CHECK(vetor_reservar(&v, 0));
    for (int i = 0; i < 1000; ++i) REF_CHECK(vetor_adicionar(&v, i));
    REF_CHECK(v.tamanho == 1000 && v.capacidade >= v.tamanho);
    for (size_t i = 0; i < v.tamanho; ++i) REF_CHECK(v.dados[i] == (int)i);
    REF_CHECK(vetor_remover(&v, 0));
    REF_CHECK(v.dados[0] == 1 && v.tamanho == 999);
    REF_CHECK(vetor_remover(&v, v.tamanho - 1));
    REF_CHECK(v.dados[v.tamanho - 1] == 998);
    int *antes = v.dados;
    size_t capacidade_antes = v.capacidade;
    /* Overflow de tamanho, sem tentar alocacao enorme nesse alvo. */
    if (sizeof(int) > 1) {
        REF_CHECK(!vetor_reservar(&v, SIZE_MAX));
        REF_CHECK(v.dados == antes && v.capacidade == capacidade_antes);
    }
    REF_CHECK(!vetor_remover(&v, v.tamanho));
    vetor_destruir(&v);
    REF_CHECK(v.dados == NULL && v.tamanho == 0 && v.capacidade == 0);
    vetor_destruir(&v);
    return ref_concluir();
}
