#include "api.h"
#include <limits.h>
#include <stddef.h>

int ref_somar(int a, int b, int *saida) {
    if (saida == NULL) return 0;
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) return 0;
    *saida = a + b;
    return 1;
}
