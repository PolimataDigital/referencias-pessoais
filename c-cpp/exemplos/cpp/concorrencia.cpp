#include "../verificar.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

int main() {
    std::atomic<unsigned long long> contador{0};
    std::mutex trava;
    int protegido = 0;
    {
        std::vector<std::jthread> trabalhadores;
        for (int t = 0; t < 4; ++t) {
            trabalhadores.emplace_back([&contador, &trava, &protegido] {
                for (int i = 0; i < 1000; ++i) {
                    contador.fetch_add(1, std::memory_order_relaxed);
                    std::lock_guard<std::mutex> lock(trava);
                    ++protegido;
                }
            });
        }
    } // jthread faz join; objetos capturados continuam vivos.
    REF_CHECK(contador.load(std::memory_order_relaxed) == 4000);
    REF_CHECK(protegido == 4000); // leitura depois dos joins
    return ref_concluir();
}
