# 07 · Algoritmos e técnicas recorrentes

[Índice](../referencia-c-cpp.md) · Básico a avançado

## Catálogo de escolha

O espaço indicado é auxiliar, além da entrada, salvo grafos cuja representação é explicitada. Custos assumem operações numéricas/chaves de custo constante.

| Algoritmo/técnica | Quando usar / pré-condição | Tempo | Espaço |
|---|---|---|---|
| Busca linear | Dados sem ordenação, uma consulta | O(n) | O(1) |
| Busca binária | Sequência ordenada/particionada pelo mesmo critério | O(log n) com acesso aleatório | O(1) |
| Insertion sort | Poucos elementos/quase ordenado | O(n²) pior, O(n) melhor | O(1) |
| Merge sort | Estabilidade e garantia de pior caso | O(n log n) | O(n) usual |
| Quicksort | Partição; escolha de pivô importa | O(n log n) esperado; O(n²) pior | Pilha O(log n) esperado, O(n) pior usual |
| Heapsort | Ordenar no próprio array com limite de pior caso | O(n log n) | O(1) |
| Counting sort | Chaves inteiras em faixa pequena de K valores | O(n+K) | O(n+K) na versão estável |
| Prefix sums | Muitas somas de intervalos, dados estáticos | O(n) montar; O(1) consulta | O(n) |
| Dois ponteiros | Ordem/monotonicidade permite descartar candidatos | O(n) após ordenar quando necessário | O(1) usual |
| Janela deslizante | Intervalo contíguo com atualização incremental | O(n) com movimentos monotônicos | Depende do estado |
| BFS | Menor número de arestas em grafo sem peso/unitário | O(V+E) com adjacência | O(V) além do grafo |
| DFS | Componentes, ciclos, ordem e exploração | O(V+E) com adjacência | O(V) |
| Ordenação topológica | Dependências em DAG | O(V+E) | O(V) |
| Dijkstra | Caminhos mínimos com pesos não negativos | O((V+E) log V) em grafos simples com heap usual | O(V+E) com entradas duplicadas |
| Bellman–Ford | Pesos negativos e detectar ciclo negativo alcançável | O(VE) | O(V) |
| Floyd–Warshall | Distâncias entre todos os pares em grafo pequeno/denso | O(V³) | O(V²) |
| Kruskal + DSU | Árvore/floresta geradora mínima, grafo não dirigido | O(E log E) | O(V+E) incluindo ordenação/arestas |
| Euclides (MDC) | Divisibilidade/frações com inteiros adequados | O(log min(a,b)) para positivos | O(1) |
| Crivo de Eratóstenes | Muitos primos até N | O(N log log N) | O(N) |
| Exponenciação binária | Potência inteira, expoente não negativo | O(log e) multiplicações | O(1) |
| KMP | Busca de padrão em texto | O(n+m) | O(m) |
| Programação dinâmica | Estados repetidos e transição bem definida | Estados × transições por estado | Estados armazenados |
| Backtracking | Enumerar soluções com poda | Frequentemente exponencial | Profundidade + estado |

Bubble sort é útil como exemplo didático de trocas, mas raramente é a escolha prática. Em C++, use `std::sort` para ordenação geral; em C, `qsort` fornece interface padrão, sem promessa ISO de um algoritmo específico, estabilidade ou O(n log n).

## Biblioteca padrão: C versus C++

| Intenção | C17 | C++20 |
|---|---|---|
| Ordenar | `qsort(a, n, sizeof *a, comparar)` | `std::sort(v.begin(), v.end())` |
| Ordenar por campo | Comparador recebe `const void *` | Lambda ou projeção de `std::ranges::sort` |
| Estabilidade | Rotina/biblioteca com contrato explícito | `std::stable_sort` |
| Buscar em ordenado | `bsearch(&chave, a, n, sizeof *a, comparar)` | `std::binary_search` |
| Primeiro ≥ x | Implementar lower bound | `std::lower_bound` |
| Primeiro > x | Implementar upper bound | `std::upper_bound` |
| Mínimo/máximo | Laço | `std::min_element` / `std::max_element` |
| Inverter | Trocar extremos | `std::reverse` |
| Somar | Laço + tipo acumulador adequado | `std::accumulate(v.begin(), v.end(), 0LL)` |
| Particionar | Laço/rotina | `std::partition`, `std::stable_partition` |
| k-ésimo | Quickselect/rotina | `std::nth_element` (média linear na forma sequencial comum) |
| Remover duplicados consecutivos | Compactar | `std::unique`; depois `erase` |
| Permutação seguinte | Implementação | `std::next_permutation` |

Headers: C `<stdlib.h>`; C++ `<algorithm>`, `<numeric>` e containers usados. `std::sort` exige iteradores de acesso aleatório; `list` oferece `lista.sort()`. Busca binária em iteradores não aleatórios pode ter O(log n) comparações e O(n) avanços.

## Ordenar e comparar sem overflow

```c
/* C17: <stdlib.h> */
static int comparar(const void *pa, const void *pb) {
    int a = *(const int *)pa;
    int b = *(const int *)pb;
    return (a > b) - (a < b);
}
/* qsort(valores, n, sizeof valores[0], comparar); */
```

```cpp
// C++20: <algorithm>, <vector>
struct Item { int prioridade; int id; };
std::vector<Item> itens{{2, 9}, {1, 8}, {2, 3}};
std::ranges::sort(itens, [](const Item& a, const Item& b) {
    if (a.prioridade != b.prioridade) return a.prioridade < b.prioridade;
    return a.id < b.id;
});
```

O comparador C++ deve definir **ordenação fraca estrita**: em particular, `comp(x,x)` é falso. `a <= b` não serve. Evite comparadores com tolerância ingênua e estabeleça política para NaN. `qsort` retorna sinal negativo/zero/positivo no comparador, enquanto `std::sort` espera `bool` “vem antes”.

## Busca binária por primeira posição ≥ x

```c
/* C17: <stddef.h>; a ordenado, a válido se n > 0. Retorna n se não há. */
size_t lower_bound_int(const int *a, size_t n, int x) {
    size_t inicio = 0, fim = n;
    while (inicio < fim) {
        size_t meio = inicio + (fim - inicio) / 2;
        if (a[meio] < x) inicio = meio + 1;
        else fim = meio;
    }
    return inicio;
}
```

Invariante: valores antes de `inicio` são menores que x; posições a partir de `fim` não são menores. O intervalo de candidatos diminui até ficar vazio. Encontrar existência exige `pos < n && a[pos] == x`. Duplicados retornam a primeira posição possível.

```cpp
auto it = std::lower_bound(v.begin(), v.end(), x);
bool existe = it != v.end() && *it == x;
```

Não teste `*it` antes de verificar `end()`. Não use `n-1` para inicializar limite unsigned de vetor possivelmente vazio. [Comparação C/C++ e casos de borda](exemplos/README.md).

## Prefixos, dois ponteiros e janela

Prefixos: `p[0]=0`, `p[i+1]=p[i]+a[i]`; soma `[l,r)` = `p[r]-p[l]`. Exige `0 <= l <= r <= n` e somas/diferenças representáveis. Atualizações frequentes sugerem Fenwick/segment tree.

Dois ponteiros para soma de dois valores em vetor ordenado: se a soma é pequena, avançar a esquerda; se grande, recuar a direita. A ordenação justifica descartar candidatos. A soma também pode transbordar; escolher tipo maior sem verificar a faixa não prova segurança.

Janela de soma ≤ K em dados **não negativos** permite expandir à direita e encolher à esquerda. Com negativos, a monotonicidade quebra; prefixos + hash/árvore ou outra técnica pode ser necessária. Uma deque monotônica encontra máximo de cada janela fixa em O(n): cada índice entra e sai no máximo uma vez.

## BFS, DFS e caminhos

```cpp
// C++20: <queue>, <vector>, <stdexcept>
std::vector<int> bfs(const std::vector<std::vector<int>>& adj, int origem) {
    if (origem < 0 || static_cast<std::size_t>(origem) >= adj.size())
        throw std::out_of_range("origem");
    std::vector<int> dist(adj.size(), -1);
    std::queue<int> fila;
    dist[static_cast<std::size_t>(origem)] = 0;
    fila.push(origem);
    while (!fila.empty()) {
        int u = fila.front(); fila.pop();
        for (int v : adj[static_cast<std::size_t>(u)]) {
            // Pré-condição: todos os vizinhos são índices válidos;
            // o número de vértices e as distâncias cabem em int.
            if (dist[static_cast<std::size_t>(v)] != -1) continue;
            dist[static_cast<std::size_t>(v)] = dist[static_cast<std::size_t>(u)] + 1;
            fila.push(v);
        }
    }
    return dist;
}
```

Marque ao enfileirar, para não enfileirar repetidamente. Para reconstruir caminho, guarde predecessor. DFS recursiva pode estourar a pilha em grafos profundos; use pilha explícita. Para detectar ciclo dirigido, distinga não visitado, ativo e concluído. Em grafo não dirigido, leve em conta a aresta pai e múltiplas arestas.

Dijkstra: relaxe somente quando `nova_dist < dist[v]`; descarte do heap a entrada cuja distância já não corresponde à melhor conhecida. **Pesos negativos invalidam a justificativa.** Evite `INF + peso` e overflow. O [exemplo completo](exemplos/cpp/grafos.cpp) valida vértices, rejeita peso negativo e evita sentinela que colida com uma distância válida.

Ordenação topológica com Kahn: iniciar fila com grau de entrada zero, remover vértice e reduzir graus dos sucessores. Se processar menos que V vértices, há ciclo. Kruskal: ordenar arestas por peso, unir componentes diferentes com DSU; em grafo desconectado, produz floresta.

## Programação dinâmica e escolha gulosa

Antes do código, registre **estado, transição, casos-base, ordem de avaliação, resposta**.

Exemplo de moeda ilimitada, minimizar quantidade para total T: `dp[0]=0`; para cada total positivo, testar `dp[t-moeda]+1` com moeda positiva e estado alcançável. Custo O(T × quantidade_de_moedas); é pseudopolinomial no valor T, não no número de bits de T. Guloso de “pegar a maior moeda” falha, por exemplo, com moedas 1, 3, 4 e total 6.

Mochila 0/1 com uma linha: percorra capacidade **decrescente** por item para não reutilizar o mesmo item. Moeda/item ilimitado costuma usar ordem crescente; a semântica da transição determina a ordem.

LIS estritamente crescente pode usar um vetor de menores finais e `lower_bound`, O(n log n); trocar por `upper_bound` altera o tratamento de iguais e pode servir à subsequência não decrescente. Esse vetor sozinho não é necessariamente a subsequência solução; para reconstruir, guarde índices/predecessores.

Backtracking deve desfazer mutações ao retornar e podar somente quando há justificativa. Memoização funciona quando o estado representa toda informação relevante; omitir parte do estado pode reutilizar resposta incorreta.

## Algoritmos numéricos e de texto

- Euclides: repetir `(a,b)=(b,a%b)` até `b==0`; defina política para negativos e `(0,0)`.
- Potência binária: multiplicar acumulador nos bits 1 do expoente e quadrar a base. Módulo não evita overflow da multiplicação intermediária; use método/tipo adequado ao domínio.
- Crivo: começar a marcar em `p*p` porque múltiplos menores já foram tratados; testar `p <= N/p` evita overflow no teste da condição.
- KMP: tabela de prefixos representa o maior prefixo próprio que também é sufixo. Quando há incompatibilidade, recuar pela tabela reaproveita o que já foi comparado.
- Hash de strings: igualdade de hash não prova igualdade do texto; considere colisões. Dados hostis podem afetar desempenho de estruturas hash.

**Teste mínimo por algoritmo:** vazio, um elemento, repetidos, já ordenado/inverso, ausente, primeiro/último, limites numéricos; para grafos, desconectado, ciclo, laço, múltiplas arestas e peso zero/negativo conforme contrato.

