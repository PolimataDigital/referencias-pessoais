#include "../verificar.h"
#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

using Vertice = std::size_t;
using Grafo = std::vector<std::vector<Vertice>>;
using Distancias = std::vector<std::optional<long long>>;

static Distancias bfs(const Grafo& adj, Vertice origem) {
    if (origem >= adj.size()) throw std::out_of_range("origem");
    if (adj.size() > static_cast<unsigned long long>(std::numeric_limits<long long>::max()))
        throw std::length_error("grafo muito grande");
    for (const auto& vizinhos : adj)
        for (Vertice v : vizinhos)
            if (v >= adj.size()) throw std::out_of_range("vizinho");
    Distancias dist(adj.size());
    std::queue<Vertice> fila;
    dist[origem] = 0;
    fila.push(origem);
    while (!fila.empty()) {
        const Vertice u = fila.front(); fila.pop();
        for (Vertice v : adj[u]) {
            if (dist[v]) continue;
            dist[v] = *dist[u] + 1;
            fila.push(v);
        }
    }
    return dist;
}

struct Aresta { Vertice destino; long long peso; };
using GrafoPesado = std::vector<std::vector<Aresta>>;

/* Rejeita todo peso negativo. Se um candidato explorado transborda,
   lanca overflow_error; nao confunde distancia maxima com inalcançavel. */
static Distancias dijkstra(const GrafoPesado& adj, Vertice origem) {
    if (origem >= adj.size()) throw std::out_of_range("origem");
    for (const auto& vizinhos : adj) {
        for (const auto& e : vizinhos) {
            if (e.destino >= adj.size()) throw std::out_of_range("destino");
            if (e.peso < 0) throw std::invalid_argument("peso negativo");
        }
    }
    using Estado = std::pair<long long, Vertice>;
    std::priority_queue<Estado, std::vector<Estado>, std::greater<Estado>> fila;
    Distancias dist(adj.size());
    dist[origem] = 0;
    fila.emplace(0, origem);
    while (!fila.empty()) {
        const auto [d, u] = fila.top(); fila.pop();
        if (!dist[u] || d != *dist[u]) continue;
        for (const auto& e : adj[u]) {
            if (d > std::numeric_limits<long long>::max() - e.peso)
                throw std::overflow_error("distancia fora da faixa");
            const long long nova = d + e.peso;
            if (!dist[e.destino] || nova < *dist[e.destino]) {
                dist[e.destino] = nova;
                fila.emplace(nova, e.destino);
            }
        }
    }
    return dist;
}

int main() {
    Grafo adj{{1, 2}, {2}, {0, 3}, {}, {}};
    const auto b = bfs(adj, 0);
    REF_CHECK(b[0] == 0 && b[1] == 1 && b[2] == 1 && b[3] == 2);
    REF_CHECK(!b[4]);
    REF_CHECK(bfs(Grafo{{0}}, 0)[0] == 0);
    bool falhou = false;
    try { (void)bfs(Grafo{}, 0); } catch (const std::out_of_range&) { falhou = true; }
    REF_CHECK(falhou);
    falhou = false;
    try { (void)bfs(Grafo{{4}}, 0); } catch (const std::out_of_range&) { falhou = true; }
    REF_CHECK(falhou);

    GrafoPesado pesado{{{1, 9}, {2, 1}}, {{3, 2}}, {{1, 1}, {3, 8}}, {}, {}};
    const auto d = dijkstra(pesado, 0);
    REF_CHECK(d[0] == 0 && d[1] == 2 && d[2] == 1 && d[3] == 4);
    REF_CHECK(!d[4]);
    REF_CHECK(dijkstra(GrafoPesado{{{0, 0}}}, 0)[0] == 0);
    falhou = false;
    try { (void)dijkstra(GrafoPesado{{{0, -1}}}, 0); }
    catch (const std::invalid_argument&) { falhou = true; }
    REF_CHECK(falhou);
    const long long maximo = std::numeric_limits<long long>::max();
    REF_CHECK(dijkstra(GrafoPesado{{{1, maximo}}, {}}, 0)[1] == maximo);
    falhou = false;
    try { (void)dijkstra(GrafoPesado{{{1, maximo}}, {{2, 1}}, {}}, 0); }
    catch (const std::overflow_error&) { falhou = true; }
    REF_CHECK(falhou);
    return ref_concluir();
}
