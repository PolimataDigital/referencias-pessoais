# Referência — C e C++

> Consulta para quem já programa: localizar a operação, comparar as linguagens, conferir o contrato e adaptar. Não exige leitura em sequência. Os exemplos e a organização são originais; os livros enviados ajudam a selecionar assuntos, e a documentação primária serve para conferir regras e ferramentas.

**Base dos exemplos:** C17 e C++20. Recursos de C23 e C++23 aparecem identificados. C e C++ são linguagens distintas: um arquivo `.c` deve ser compilado como C; um `.cpp`, como C++. A semelhança de sintaxe não torna um programa automaticamente válido nas duas.

## Índice por necessidade

| Quero consultar… | Onde |
|---|---|
| Tipos, declaração, operadores, laços, casts, números | [01 · Sintaxe e tipos](c-cpp/01-sintaxe-tipos.md) |
| Strings, parsing, entrada, saída, arquivos, biblioteca padrão | [02 · Texto e I/O](c-cpp/02-texto-io.md) |
| Ponteiros, arrays, referências, alocação, ownership, RAII | [03 · Memória](c-cpp/03-memoria.md) |
| Funções, headers, structs, callbacks, macros, APIs em C | [04 · C idiomático](c-cpp/04-c-idiomatico.md) |
| Classes, templates, lambdas, move, ranges, concepts, erros | [05 · C++ moderno](c-cpp/05-cpp-moderno.md) |
| Qual container escolher e quanto custa cada operação | [06 · Estruturas de dados](c-cpp/06-estruturas.md) |
| Busca, ordenação, grafos, DP e técnicas recorrentes | [07 · Algoritmos](c-cpp/07-algoritmos.md) |
| GCC, Clang, MSVC, CMake, Windows/Linux/macOS, ARM | [08 · Plataformas e build](c-cpp/08-plataformas-build.md) |
| Testes, sanitizers, debugger, profiling e otimização | [09 · Qualidade e desempenho](c-cpp/09-qualidade.md) |
| Threads, atomics, API C/C++, ABI, redes e FFI | [10 · Concorrência e integração](c-cpp/10-concorrencia-ffi.md) |
| Erros frequentes e revisão rápida de um projeto | [11 · Armadilhas e checklists](c-cpp/11-armadilhas.md) |
| Padrões, fontes, livros consultados e limites da validação | [12 · Fontes e versões](c-cpp/12-fontes.md) |
| Programas completos, compiláveis, com casos de verificação | [Exemplos](c-cpp/exemplos/README.md) |

## Comparação de bolso

Os trechos desta tabela são fragmentos; cada capítulo informa os headers e o contexto. C17/C++20, salvo indicação.

| Intenção | C | C++ idiomático |
|---|---|---|
| Inteiro inicializado | `int n = 0;` | `int n{};` |
| Booleano | `<stdbool.h>` e `bool ok = true;` | `bool ok = true;` nativo |
| Constante | `const int limite = 10;` | `constexpr int limite = 10;` se constante em compilação |
| Inferir tipo | Escrever o tipo; `auto` muda de significado em C23 | `auto n = 10;` (tipo estático `int`) |
| Ponteiro nulo | `NULL` | `nullptr` |
| Texto com propriedade | Array de `char` ou buffer alocado | `std::string` |
| Texto emprestado | `const char *` e tamanho/contrato | `std::string_view` (C++17) |
| Array de tamanho fixo | `int a[3] = {1, 2, 3};` | `std::array<int, 3> a{1, 2, 3};` |
| Sequência dinâmica | Ponteiro + tamanho + capacidade + funções | `std::vector<int> v{1, 2, 3};` |
| Vista de sequência | Ponteiro e tamanho | `std::span<const int>` (C++20) |
| Alterar argumento | `void f(int *p);` e `f(&x);` | `void f(int& x);` e `f(x);` |
| Registro | `struct Pessoa { int idade; };` | `struct Pessoa { int idade{}; };` |
| Usar nome do registro | `struct Pessoa p = {20};` ou `typedef` | `Pessoa p{20};` |
| Enumeração | `enum Estado { PARADO, ATIVO };` | `enum class Estado { parado, ativo };` |
| Encapsular | Tipo opaco e funções | Classe com membros privados |
| Variantes | `enum` discriminador + `union` | `std::variant` (C++17) |
| Ausência de resultado | Status + parâmetro de saída | `std::optional<T>` (C++17) |
| Erro com detalhe | `enum`/status + saída; `errno` quando a API o define | Exceção ou `std::expected<T,E>` (C++23) |
| Memória dinâmica | `malloc`/`realloc`/`free` | Containers; `std::make_unique<T>` quando necessário |
| Recurso exclusivo | Contrato manual de aquisição/liberação | RAII e `std::unique_ptr` |
| Ordenar | `qsort` + comparador de ponteiros | `std::sort`/`std::ranges::sort` |
| Busca binária | `bsearch` ou implementação própria | `std::lower_bound`/`std::binary_search` |
| Tabela hash | Implementar ou escolher biblioteca | `std::unordered_map` |
| Genérico | `void *`, macros, `_Generic` (C11) | Templates, concepts (C++20) |
| Callback | Ponteiro de função + contexto | Lambda, template de callable ou `std::function` |
| Arquivo | `FILE *`, `fopen`, `fclose` | `std::ifstream`/`std::ofstream` |
| Caminho de arquivo | Biblioteca do sistema/terceiros | `std::filesystem::path` (C++17) |
| Concorrência | `<threads.h>`/`<stdatomic.h>` quando disponíveis | `<thread>`, `<mutex>`, `<atomic>`; `std::jthread` (C++20) |

## Níveis para localizar, não para impor uma trilha

| Nível | Referências típicas | Pergunta de revisão |
|---|---|---|
| Básico | Sintaxe, tipos, fluxo, strings, arrays, funções | O que esta expressão faz e qual é seu tipo? |
| Intermediário | Ponteiros, arquivos, módulos, containers, complexidade | Quem possui o dado e quais são os limites? |
| Avançado | RAII, templates, grafos, DP, concorrência, ABI | Quais invariantes tornam esta solução correta? |
| Especializado | Atomics, allocators, layout/cache, FFI, cross-compilation | Que medição ou restrição justifica a complexidade? |

## Convenções

- Intervalos de índice preferem **`[inicio, fim)`**: início incluído e fim excluído.
- `n`: quantidade de elementos; `V`/`E`: vértices/arestas; `L`: comprimento de texto.
- “Amortizado” considera uma sequência de operações; “esperado” depende de hipóteses sobre dados/hashing. Não são sinônimos de pior caso.
- Um exemplo de contrato não valida necessariamente toda entrada. Pré-condições e limites aparecem junto dele.
- Blocos nos capítulos são **fragmentos de consulta**. Os arquivos em `exemplos/` são os programas completos usados na validação.
- A existência de um recurso no padrão não garante suporte na combinação compilador + biblioteca padrão + sistema. [Conferir versões](c-cpp/12-fontes.md).

## Decisões rápidas

1. Em C++, comece com valores, `std::string` e `std::vector`; introduza ponteiros proprietários quando houver uma necessidade concreta.
2. Em C, escreva no contrato quem aloca, quem libera, qual o tamanho e como o erro é comunicado.
3. Para dados externos, valide conversão, intervalo e comprimento antes de acessar memória.
4. Use a biblioteca padrão antes de reimplementar um algoritmo; mantenha implementações próprias para estudo ou requisitos específicos.
5. Escolha a estrutura de dados antes de micro-otimizar o laço.
6. Compile com padrão explícito, avisos e testes; use sanitizers em desenvolvimento.

