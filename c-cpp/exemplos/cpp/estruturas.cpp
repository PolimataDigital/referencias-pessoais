#include "../verificar.h"
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

class DSU {
public:
    explicit DSU(std::size_t n) : pai_(n), tamanho_(n, 1) {
        std::iota(pai_.begin(), pai_.end(), std::size_t{0});
    }
    std::size_t raiz(std::size_t x) {
        if (x >= pai_.size()) throw std::out_of_range("vertice");
        while (x != pai_[x]) {
            pai_[x] = pai_[pai_[x]]; // compressao por path halving
            x = pai_[x];
        }
        return x;
    }
    bool unir(std::size_t a, std::size_t b) {
        a = raiz(a); b = raiz(b);
        if (a == b) return false;
        if (tamanho_[a] < tamanho_[b]) std::swap(a, b);
        pai_[b] = a;
        tamanho_[a] += tamanho_[b]; // no maximo o numero de elementos
        return true;
    }
private:
    std::vector<std::size_t> pai_, tamanho_;
};

/* Soma: cliente garante que acumulados e atualizacoes cabem em long long.
   Interface usa [l,r), indices externos de 0 a n-1. */
class Fenwick {
public:
    explicit Fenwick(std::size_t n) {
        if (n == std::numeric_limits<std::size_t>::max())
            throw std::length_error("tamanho");
        arvore_.resize(n + 1, 0);
    }
    void adicionar(std::size_t indice, long long delta) {
        if (indice >= tamanho()) throw std::out_of_range("indice");
        for (std::size_t i = indice + 1; i < arvore_.size();) {
            arvore_[i] += delta;
            const std::size_t passo = i & (~i + 1);
            if (passo >= arvore_.size() - i) break;
            i += passo;
        }
    }
    long long prefixo(std::size_t fim) const {
        if (fim > tamanho()) throw std::out_of_range("fim");
        long long soma = 0;
        for (std::size_t i = fim; i > 0; i -= i & (~i + 1)) soma += arvore_[i];
        return soma;
    }
    long long intervalo(std::size_t inicio, std::size_t fim) const {
        if (inicio > fim) throw std::invalid_argument("intervalo");
        return prefixo(fim) - prefixo(inicio);
    }
    std::size_t tamanho() const { return arvore_.size() - 1; }
private:
    std::vector<long long> arvore_;
};

int main() {
    DSU dsu(6);
    REF_CHECK(dsu.unir(0, 1));
    REF_CHECK(dsu.unir(1, 2));
    REF_CHECK(dsu.raiz(0) == dsu.raiz(2));
    REF_CHECK(!dsu.unir(2, 0));
    REF_CHECK(dsu.raiz(3) != dsu.raiz(0));
    bool erro = false;
    try { (void)dsu.raiz(6); } catch (const std::out_of_range&) { erro = true; }
    REF_CHECK(erro);

    std::vector<long long> valores{3, -1, 7, 0, 9};
    Fenwick f(valores.size());
    for (std::size_t i = 0; i < valores.size(); ++i) f.adicionar(i, valores[i]);
    for (std::size_t inicio = 0; inicio <= valores.size(); ++inicio) {
        for (std::size_t fim = inicio; fim <= valores.size(); ++fim) {
            long long esperado = 0;
            for (std::size_t i = inicio; i < fim; ++i) esperado += valores[i];
            REF_CHECK(f.intervalo(inicio, fim) == esperado);
        }
    }
    f.adicionar(2, -2);
    REF_CHECK(f.intervalo(2, 3) == 5);
    REF_CHECK(f.prefixo(5) == 16);
    REF_CHECK(Fenwick(0).prefixo(0) == 0);
    erro = false;
    try { f.adicionar(5, 1); } catch (const std::out_of_range&) { erro = true; }
    REF_CHECK(erro);
    return ref_concluir();
}
