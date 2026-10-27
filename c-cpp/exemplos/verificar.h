#ifndef REF_VERIFICAR_H
#define REF_VERIFICAR_H
#include <stdio.h>
#include <stdlib.h>

static unsigned long ref_total = 0;
static void ref_verificar(int condicao, const char *expressao, int linha) {
    ++ref_total;
    if (!condicao) {
        fprintf(stderr, "FALHA linha %d: %s\n", linha, expressao);
        exit(EXIT_FAILURE);
    }
}
#define REF_CHECK(expr) ref_verificar((expr), #expr, __LINE__)
static int ref_concluir(void) {
    printf("OK: %lu verificacoes\n", ref_total);
    return EXIT_SUCCESS;
}
#endif
