#include "../verificar.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* Decimal com espacos nas extremidades; saida preservada no erro. */
static bool parse_int(const char *texto, int *saida) {
    if (texto == NULL || saida == NULL) return false;
    errno = 0;
    char *fim;
    long valor = strtol(texto, &fim, 10);
    if (fim == texto || errno == ERANGE || valor < INT_MIN || valor > INT_MAX)
        return false;
    while (isspace((unsigned char)*fim)) ++fim;
    if (*fim != '\0') return false;
    *saida = (int)valor;
    return true;
}

static bool somar_int(int a, int b, int *saida) {
    if (saida == NULL) return false;
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
        return false;
    *saida = a + b;
    return true;
}

static int comparar(const void *pa, const void *pb) {
    const int a = *(const int *)pa;
    const int b = *(const int *)pb;
    return (a > b) - (a < b);
}

/* a ordenado e valido para n elementos; NULL aceito somente com n == 0. */
static size_t lower_bound_int(const int *a, size_t n, int x) {
    size_t inicio = 0, fim = n;
    while (inicio < fim) {
        size_t meio = inicio + (fim - inicio) / 2;
        if (a[meio] < x) inicio = meio + 1;
        else fim = meio;
    }
    return inicio;
}

int main(void) {
    int valor = 123;
    REF_CHECK(parse_int("  -42 \n", &valor) && valor == -42);
    REF_CHECK(parse_int("+7", &valor) && valor == 7);
    REF_CHECK(parse_int("0", &valor) && valor == 0);
    REF_CHECK(!parse_int("", &valor));
    REF_CHECK(!parse_int("  ", &valor));
    REF_CHECK(!parse_int("12resto", &valor));
    REF_CHECK(!parse_int("99999999999999999999999999999999999999999999999999", &valor));
    REF_CHECK(!parse_int(NULL, &valor));
    REF_CHECK(!parse_int("1", NULL));
    REF_CHECK(valor == 0);
    char limite[sizeof(int) * CHAR_BIT + 3];
    int escrito = snprintf(limite, sizeof limite, "%d", INT_MAX);
    REF_CHECK(escrito > 0 && (size_t)escrito < sizeof limite);
    REF_CHECK(parse_int(limite, &valor) && valor == INT_MAX);
    escrito = snprintf(limite, sizeof limite, "%d", INT_MIN);
    REF_CHECK(escrito > 0 && (size_t)escrito < sizeof limite);
    REF_CHECK(parse_int(limite, &valor) && valor == INT_MIN);

    REF_CHECK(somar_int(20, 22, &valor) && valor == 42);
    REF_CHECK(!somar_int(INT_MAX, 1, &valor) && valor == 42);
    REF_CHECK(!somar_int(INT_MIN, -1, &valor));
    REF_CHECK(somar_int(INT_MAX, INT_MIN, &valor) && valor == INT_MAX + INT_MIN);
    REF_CHECK(!somar_int(1, 2, NULL));

    int a[] = {INT_MAX, 3, -4, 3, INT_MIN, 0};
    const size_t n = sizeof a / sizeof a[0];
    qsort(a, n, sizeof a[0], comparar);
    const int esperado[] = {INT_MIN, -4, 0, 3, 3, INT_MAX};
    for (size_t i = 0; i < n; ++i) REF_CHECK(a[i] == esperado[i]);
    REF_CHECK(lower_bound_int(NULL, 0, 7) == 0);
    REF_CHECK(lower_bound_int(a, n, 3) == 3);
    REF_CHECK(lower_bound_int(a, n, INT_MAX) == n - 1);
    const int unico[] = {2};
    REF_CHECK(lower_bound_int(unico, 1, 3) == 1);
    REF_CHECK(lower_bound_int(unico, 1, 1) == 0);
    /* Oraculo independente: busca linear para todos esses alvos. */
    for (int x = -8; x <= 8; ++x) {
        size_t esperado_pos = 0;
        while (esperado_pos < n && a[esperado_pos] < x) ++esperado_pos;
        REF_CHECK(lower_bound_int(a, n, x) == esperado_pos);
    }
    return ref_concluir();
}
