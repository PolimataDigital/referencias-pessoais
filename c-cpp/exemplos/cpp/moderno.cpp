#include "../verificar.h"
#include <concepts>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<std::integral T>
constexpr bool par(T valor) noexcept { return valor % 2 == 0; }
static_assert(par(12));
static_assert(!par(7u));

class Produto {
public:
    Produto(std::string nome, int estoque)
        : nome_(std::move(nome)), estoque_(estoque) {
        if (nome_.empty() || estoque_ < 0) throw std::invalid_argument("produto");
    }
    int estoque() const noexcept { return estoque_; }
    std::string_view nome() const noexcept { return nome_; }
    bool retirar(int n) noexcept {
        if (n < 0 || n > estoque_) return false;
        estoque_ -= n;
        return true;
    }
private:
    std::string nome_;
    int estoque_{};
};

struct Recurso {
    int& destruicoes;
    ~Recurso() { ++destruicoes; }
};

int main() {
    Produto p("livro", 5);
    REF_CHECK(p.nome() == "livro");
    REF_CHECK(p.retirar(2) && p.estoque() == 3);
    REF_CHECK(!p.retirar(4) && p.estoque() == 3);
    REF_CHECK(!p.retirar(-1));
    auto dono = std::make_unique<Produto>("caderno", 8);
    Produto* observador = dono.get();
    auto outro = std::move(dono);
    REF_CHECK(!dono && outro.get() == observador);
    REF_CHECK(observador->estoque() == 8);

    int destruicoes = 0;
    try {
        Recurso recurso{destruicoes};
        throw std::runtime_error("falha simulada");
    } catch (const std::runtime_error&) { /* RAII ja liberou o recurso */ }
    REF_CHECK(destruicoes == 1);

    const std::vector<int> v{1, 2, 3, 4, 5};
    auto pares = v | std::views::filter([](int n) { return par(n); });
    int soma = 0;
    for (int n : pares) soma += n;
    REF_CHECK(soma == 6);
    return ref_concluir();
}
