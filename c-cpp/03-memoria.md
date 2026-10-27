# 03 · Memória, ponteiros e tempo de vida

[Índice](../referencia-c-cpp.md) · Intermediário a avançado

## Conceitos que não são sinônimos

| Conceito | O que informa |
|---|---|
| Escopo | Onde um nome pode ser usado |
| Tempo de vida | Quando o objeto existe e pode ser acessado |
| Duração de armazenamento | Por quanto tempo a região de armazenamento está disponível |
| Ownership | Quem é responsável por liberar o recurso |
| Alias | Outro caminho de acesso ao mesmo objeto |
| Capacidade | Espaço reservado; não necessariamente quantidade de objetos válidos |

“Stack” e “heap” são modelos úteis de implementação; as linguagens especificam duração automática, estática, de thread e armazenamento dinâmico com regras próprias. Um ponteiro local pode apontar para memória dinâmica. Um objeto local C++ pode possuir essa memória via `vector`.

## Ponteiros e referências

| Declaração | Significado |
|---|---|
| `int *p` | Ponteiro mutável para `int` mutável |
| `const int *p` | Pode mudar `p`; não pode escrever no `int` por ele |
| `int *const p = &x` | Ponteiro fixo; pode escrever no `int` |
| `const int *const p = &x` | Ambos protegidos por esse acesso |
| `int **pp` | Ponteiro para ponteiro; não equivale a matriz contígua |
| `int (*p)[4]` | Ponteiro para array de quatro inteiros |
| `int *p[4]` | Array de quatro ponteiros |
| `int& r = x` (C++) | Referência vinculada a `x` |
| `const T& r` (C++) | Empréstimo para leitura; vida útil precisa ser válida |

`int *a, b;` declara somente `a` como ponteiro. `&x` obtém endereço, `*p` acessa o objeto, `p->campo` equivale a `(*p).campo`.

Aritmética de ponteiros é limitada ao mesmo array e à posição imediatamente após seu fim. Essa posição final pode ser formada/comparada dentro das regras, mas não desreferenciada. Não subtraia ponteiros de alocações diferentes.

## Arrays, decaimento e vistas

```c
/* C17: <stddef.h>; exige a apontando para pelo menos n inteiros. */
void preencher(int *a, size_t n, int valor) {
    for (size_t i = 0; i < n; ++i) a[i] = valor;
}
```

Na assinatura `void f(int a[10])`, o parâmetro é ajustado para ponteiro: `10` não cria verificação automática. `sizeof a / sizeof a[0]` funciona para array real no escopo, não para esse parâmetro. `int a[static 10]` em parâmetro C99 expressa uma exigência mínima, não um teste de runtime.

```cpp
// C++20: <span>
void preencher(std::span<int> a, int valor) {
    for (int& x : a) x = valor;
}
```

`span` empresta elementos contíguos e carrega tamanho, mas não os mantém vivos. Em C++20, `span::operator[]` não fornece checagem obrigatória de limites. `std::vector::at` verifica; `operator[]` exige índice válido.

Uma matriz `int a[3][4]` é contígua e decai para `int (*)[4]`, não `int **`. Para tamanho dinâmico portátil, considere vetor plano e índice `linha * colunas + coluna`; valide produto e limites antes da alocação/acesso.

## Alocação em C

```c
/* <stdlib.h>, <stdint.h>; trecho com n > 0 */
int *dados = NULL;
if (n > SIZE_MAX / sizeof *dados) { /* tamanho impossível */ }
else {
    dados = malloc(n * sizeof *dados);
    if (dados == NULL) { /* falha de alocação */ }
    else {
        /* inicializar antes de ler */
        free(dados);
        dados = NULL;
    }
}
```

Não faça cast de `malloc` em C; inclua `<stdlib.h>`. `malloc` não inicializa valores. `calloc` zera bytes, o que não garante toda representação de ponteiro nulo ou ponto flutuante em todo alvo. Para inteiros usuais, os bits zerados representam zero.

Para redimensionar, calcule tamanho válido e use temporário:

```c
/* dados veio de malloc/calloc/realloc; novo_n > 0 e produto verificado */
int *novo = realloc(dados, novo_n * sizeof *dados);
if (novo != NULL) {
    dados = novo;  /* aliases antigos não devem mais ser usados */
    /* atualizar capacidade; inicializar novos elementos antes de ler */
} else {
    /* dados original ainda pertence ao chamador */
}
```

`dados = realloc(dados, ...)` pode perder a única referência no erro. Não dependa de `realloc(p, 0)`; trate tamanho zero com `free` e estado vazio explícito. `free(NULL)` é permitido. Definir um ponteiro como `NULL` depois de liberar não corrige outros aliases pendentes. [Vetor C completo](exemplos/c/vetor.c).

## C++: propriedade expressa por tipos

| Tipo | Possui? | Uso |
|---|---|---|
| `T` | O próprio valor | Primeira opção |
| `std::vector<T>` / `std::string` | Sim | Sequências/texto dinâmicos |
| `std::unique_ptr<T>` | Exclusivo | Objeto com vida dinâmica, transferível |
| `std::shared_ptr<T>` | Compartilhado | Vida útil realmente compartilhada |
| `std::weak_ptr<T>` | Observa um controle compartilhado | Quebrar ciclos; `lock()` obtém posse temporária |
| `T*`, `T&` | Por convenção, não | Acesso emprestado; documentar exceções em APIs C |
| `std::span<T>`, `std::string_view` | Não | Vista com tamanho |

```cpp
// C++14+: <memory>, <utility>
auto dono = std::make_unique<int>(42);
int* observador = dono.get();
auto outro = std::move(dono); // dono fica vazio; outro possui o mesmo int
// observador ainda é válido enquanto outro mantiver esse objeto vivo.
```

RAII vincula a liberação a um objeto e ao seu destrutor. Vale para memória, arquivos, locks e handles. `shared_ptr` não sincroniza o conteúdo apontado e seu contador tem custo. Ciclos de propriedade compartilhada impedem liberação automática.

## Invalidação e empréstimos pendentes

- `vector` realocado invalida ponteiros, referências e iteradores para seus elementos.
- `reserve` pode realocar; não cria elementos. Após `reserve(100)`, `v[99]` ainda é inválido se `size() == 0`.
- `erase` em `vector` invalida a posição apagada e as seguintes. Inserção sem realocação também invalida posições a partir da inserção.
- `string_view` de uma string temporária pode ficar pendente ao fim da expressão. `std::string_view s = std::string("abc");` é um anti-exemplo.
- `return local.c_str()` e retornar referência a variável local são erros de tempo de vida.
- `memcpy` de um `std::string` ou outro objeto com gerenciamento de recurso não equivale à cópia C++.

Para chamada que apenas observa texto, `string_view` costuma funcionar; para guardar o texto além da chamada, geralmente faça uma cópia `std::string`.

## Técnicas especializadas

Alinhamento, representação e vida útil são três requisitos diferentes. Um buffer suficientemente grande não autoriza acessar qualquer tipo com `reinterpret_cast`. Use cópia por valor, `memcpy` para casos adequados de representação ou `std::bit_cast` com tipos trivially copyable de mesmo tamanho; valide as representações possíveis.

Pools/arenas reduzem alocações quando muitos objetos compartilham vida útil. Em C++, `<memory_resource>` (C++17) permite `std::pmr::vector` e recursos monotônicos. O recurso precisa viver mais que os containers que o usam. Não reutilize uma arena antes de destruir seus objetos vivos. Só adote após medir e definir o contrato de reset.

`restrict` em C promete condições de não aliasing durante acessos relevantes. Violá-las pode produzir UB. Não é um “otimizador gratuito”; C++ não tem `restrict` padrão.

