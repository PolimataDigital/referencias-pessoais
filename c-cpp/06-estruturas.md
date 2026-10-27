# 06 · Estruturas de dados e containers

[Índice](../referencia-c-cpp.md) · Intermediário a avançado

## Escolha pela operação dominante

Custos abaixo contam operações sobre elementos/chaves de custo constante. Comparar/copiar strings de comprimento L pode acrescentar fator O(L). “Busca” significa por valor/chave, não acesso por índice.

| Estrutura | Acesso / busca | Inserir / remover | C quando necessário | C++ padrão |
|---|---|---|---|---|
| Array fixo | Índice O(1); busca O(n) | Deslocamento O(n), capacidade fixa | `T a[N]` | `std::array<T,N>` |
| Vetor dinâmico | Índice O(1); busca O(n) | Fim O(1) amortizado; meio O(n) | Buffer + tamanho + capacidade | `std::vector<T>` |
| Deque | Índice O(1); busca O(n) | Pontas O(1); meio O(n) | Buffer circular/blocos | `std::deque<T>` |
| Lista duplamente ligada | Índice/busca O(n) | O(1) **com iterador/nó conhecido** | Nós e ponteiros | `std::list<T>` |
| Pilha | Topo O(1) | Push/pop O(1) com deque; push amortizado com vetor | Array/vetor ou nós | `std::stack<T>` |
| Fila | Frente O(1) | Enqueue/dequeue O(1) com deque/circular | Circular ou nós | `std::queue<T>` |
| Heap binário | Extremo O(1); busca O(n) | O(log n) | Array + sift-up/down | `std::priority_queue<T>` |
| Árvore balanceada | Chave O(log n) | O(log n) | AVL/RB ou biblioteca | `std::map`, `std::set` |
| Hash | Esperado O(1); pior O(n) | Esperado O(1); pior O(n) | Buckets + política de colisão | `std::unordered_map/set` |
| Trie | O(L) por chave com transições adequadas | O(L) | Nós/transições | Implementação/biblioteca |
| DSU (union-find) | O(α(n)) amortizado | União O(α(n)) amortizado | Arrays de pais/ranks | Implementação |
| Fenwick | Prefixo O(log n) | Atualização pontual O(log n) | Array | Implementação |
| Segment tree | Consulta O(log n) usual | Atualização O(log n) usual | Array/nós | Implementação |

C não tem containers genéricos equivalentes a STL na biblioteca padrão. Sua implementação precisa cuidar de alocação, overflow de capacidade, invariantes, destruição e falhas. Não presuma que `qsort` seja um container ou tenha complexidade imposta pelo padrão C.

Um `vector` costuma ser bom ponto de partida por simplicidade e localidade. Lista ligada não é automaticamente mais rápida para inserir: encontrar a posição e alocar nós pode dominar. Hash não preserva ordem de chaves. Escolha árvore quando precisar de intervalos ordenados/predecessores.

## Operações C++ de consulta rápida

| Intenção | Código | Header / detalhe |
|---|---|---|
| Vetor vazio | `std::vector<int> v;` | `<vector>` |
| n zeros | `std::vector<int> v(n, 0);` | n elementos reais |
| Reservar espaço | `v.reserve(n);` | Não altera `size()` |
| Alterar quantidade | `v.resize(n);` | Cria/destrói elementos |
| Adicionar | `v.push_back(x);` / `v.emplace_back(args...);` | `emplace` não é sempre mais rápido |
| Remover último | `v.pop_back();` | Exige não vazio; não retorna o valor |
| Acesso verificado | `v.at(i)` | Lança se índice inválido |
| Primeiro/último | `v.front()` / `v.back()` | Exigem não vazio |
| Remover valor | `std::erase(v, x);` | C++20, `<vector>` |
| Remover por condição | `std::erase_if(v, pred);` | C++20 |
| Mapa ordenado | `std::map<std::string, int> m;` | `<map>`, `<string>` |
| Mapa hash | `std::unordered_map<std::string, int> m;` | `<unordered_map>` |
| Procurar sem inserir | `auto it = m.find(chave);` | Comparar com `m.end()` |
| Testar chave | `m.contains(chave)` | C++20 |
| Acesso obrigatório | `m.at(chave)` | Lança se ausente |
| Inserir/atribuir | `m.insert_or_assign(chave, valor);` | C++17 |
| Inserir só se ausente | `m.try_emplace(chave, args...);` | C++17; argumentos da chamada ainda são avaliados |
| Iterar pares | `for (const auto& [k, v] : m)` | Structured bindings C++17 |
| Conjunto | `std::set<int> s; s.insert(7);` | `<set>` |
| Min-heap | `std::priority_queue<int, std::vector<int>, std::greater<int>> q;` | `<queue>`, `<vector>`, `<functional>` |
| Pilha/fila | `s.top()` / `q.front()` | Exigem não vazio; `pop()` não retorna valor |

`m[chave]` insere valor padrão se a chave estiver ausente. Para frequência isso é útil (`++freq[x]`); para consulta pode esconder bug e alocar inesperadamente. `map::lower_bound` faz busca logarítmica; `std::lower_bound(m.begin(), m.end(), ...)` não oferece a mesma eficiência de movimentação de iteradores.

## Invalidação: resumo operacional

| Container | Regra útil |
|---|---|
| `vector` | Realocação invalida tudo; erase invalida a partir da posição; mesmo push sem realocar invalida o antigo `end()` |
| `deque` | Push nas pontas preserva referências a elementos, mas pode invalidar iteradores; operações no meio são mais fortes |
| `list` | Inserção preserva referências/iteradores; erase invalida os apagados |
| `map`/`set` | Inserção preserva; erase invalida os apagados |
| `unordered_map/set` | Rehash invalida iteradores, mas preserva referências/ponteiros a elementos existentes; erase invalida o elemento |

Consulte a operação específica quando a validade de um iterador atravessa uma mutação. Um iterador `end()` nunca identifica elemento. `vector<bool>` é uma especialização compactada com referências proxy; não a trate como array contíguo de `bool`. `std::vector<unsigned char>` pode ser mais simples para flags quando compactação não é necessária.

## Representações em C

```c
/* <stddef.h> */
typedef struct {
    int *dados;
    size_t tamanho;
    size_t capacidade;
} Vetor;

typedef struct No {
    int valor;
    struct No *proximo;
} No;
```

Vetor: `tamanho <= capacidade`; se capacidade for zero, adote `dados == NULL`. Ao crescer geometricamente, confira tanto a duplicação da capacidade quanto `capacidade * sizeof *dados`. Falha deve preservar o estado antigo. [Implementação completa](exemplos/c/vetor.c).

Lista: inserir na cabeça é O(1); buscar um valor é O(n). Destrua iterativamente se a lista puder ser longa. Para remover um nó, conheça seu predecessor ou use ponteiro para ponteiro; não leia `no->proximo` após liberar `no`.

Fila circular: mantenha capacidade > 0, índice da frente e quantidade; o próximo índice é calculado com wrap. Diferencie vazio e cheio pela quantidade ou reserve uma posição. A soma dos índices também deve ser representável. Crescer exige recolocar os elementos em ordem lógica.

## Grafos

| Representação | Espaço | Consultar aresta | Percorrer vizinhos |
|---|---|---|---|
| Matriz de adjacência | O(V²) | O(1) | O(V) por vértice |
| Lista de adjacência | O(V+E) | O(grau), salvo índice auxiliar | O(grau) |
| Lista de arestas | O(E) | O(E) | O(E), salvo índice auxiliar |
| CSR (arrays compactos) | O(V+E) | Busca no intervalo | Boa localidade; atualização difícil |

Em C++, lista usual: `std::vector<std::vector<int>>`. Em C: vetor de vetores, listas encadeadas ou CSR. Em grafo não dirigido, cada aresta costuma aparecer duas vezes na adjacência. Guarde peso separadamente ou em registro `{destino, peso}`.

## Estruturas especializadas

- **DSU:** conectividade por uniões; compressão de caminhos + união por rank/tamanho. Não resolve remoção arbitrária de arestas nem fornece caminho entre vértices.
- **Fenwick:** prefixos e atualização pontual. Soma de `[l,r)` = prefixo(r) − prefixo(l); requer operação inversa para essa diferença. Usualmente índices internos começam em 1.
- **Segment tree:** agrega intervalos com operação associativa e elemento neutro. Lazy propagation exige definir composição das atualizações e sua ação no agregado.
- **Trie:** útil para prefixos; custo e memória dependem da representação de transições e do alfabeto (bytes não são caracteres Unicode).
- **Heap:** extrema prioridade; não mantém toda a sequência ordenada. `priority_queue` padrão não oferece atualização direta de prioridade; Dijkstra pode inserir novo par e descartar entradas antigas.

