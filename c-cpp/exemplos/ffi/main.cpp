#include "api.h"
#include "../verificar.h"
#include <limits>

int main() {
    int saida = 0;
    REF_CHECK(ref_somar(20, 22, &saida) == 1 && saida == 42);
    REF_CHECK(ref_somar(std::numeric_limits<int>::max(), 1, &saida) == 0);
    REF_CHECK(saida == 42);
    REF_CHECK(ref_somar(std::numeric_limits<int>::min(), -1, &saida) == 0);
    REF_CHECK(ref_somar(1, 2, nullptr) == 0);
    return ref_concluir();
}
