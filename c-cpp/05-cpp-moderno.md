# 05 · C++ moderno: valores, classes e genéricos

[Índice](../referencia-c-cpp.md) · Intermediário a especializado

## Passagem de parâmetros

| Situação | Assinatura típica | Observação |
|---|---|---|
| Escalar pequeno | `void f(int x)` | Cópia simples |
| Objeto caro, leitura | `void f(const T& x)` | Sem cópia; validar vida útil |
| Modificar objeto existente | `void f(T& x)` | Não representa ausência |
| Empréstimo opcional | `void f(T* x)` | Definir contrato de `nullptr` |
| Texto observado | `void f(std::string_view x)` | Não armazenar sem garantir vida útil |
| Sequência observada | `void f(std::span<const T> x)` | C++20; tamanho explícito |
| Transferir propriedade exclusiva | `void f(std::unique_ptr<T> x)` | Chamador pode usar `std::move` |
| Guardar uma string própria | `void f(std::string x)` e mover para membro | Faz cópia para lvalue, pode mover rvalue |

Não passe `shared_ptr` por valor somente para observar o objeto; isso altera contagem e contrato de posse. Parâmetros de saída às vezes são apropriados, mas retorno por valor/struct costuma comunicar melhor o resultado. Otimizações de elisão tornam retorno por valor eficiente em muitos casos.

## Classe com invariante e Rule of Zero

```cpp
// C++17+: <stdexcept>, <string>, <string_view>, <utility>
class Produto {
public:
    Produto(std::string nome, int estoque)
        : nome_(std::move(nome)), estoque_(estoque) {
        if (nome_.empty() || estoque_ < 0)
            throw std::invalid_argument("produto invalido");
    }

    [[nodiscard]] int estoque() const noexcept { return estoque_; }
    [[nodiscard]] std::string_view nome() const noexcept { return nome_; }

    bool retirar(int quantidade) noexcept {
        if (quantidade < 0 || quantidade > estoque_) return false;
        estoque_ -= quantidade;
        return true;
    }
private:
    std::string nome_;
    int estoque_{};
};
```

O nome observado deixa de valer quando o objeto/string deixa de ser válido. Uma classe com membros que já gerenciam recursos normalmente não precisa de destrutor, construtor de cópia/move ou atribuições manuais: **Rule of Zero**.

Se administrar um recurso bruto diretamente, considere todos os cinco membros especiais (destrutor, cópia, atribuição de cópia, move, atribuição de move). Não implemente os cinco mecanicamente: pode ser correto excluir cópia e permitir move, ou excluir ambos. `= delete` impede operações; `= default` solicita a implementação padrão quando válida.

Use `explicit` em construtores de um argumento quando não desejar conversão implícita. A ordem de inicialização de membros é a ordem de declaração, não a ordem escrita na lista do construtor. Não lance exceções de destrutores.

## Polimorfismo e composição

```cpp
struct Repositorio {
    virtual ~Repositorio() = default;
    virtual bool existe(int id) const = 0;
};

struct RepositorioMemoria final : Repositorio {
    bool existe(int id) const override { return id == 1; }
};
```

Use destrutor virtual quando objetos derivados serão destruídos por ponteiro para base. Passar uma base polimórfica por valor pode causar *slicing*. `override` permite que o compilador confira a intenção. Prefira composição para compartilhar partes e capacidades; herança pública deve representar substituição válida do contrato.

| Modelo | Quando ajuda | Custo/limite |
|---|---|---|
| Valor concreto | Tipo conhecido | Simples e fácil de otimizar |
| Virtual/interface | Tipo escolhido em execução, conjunto aberto | Indireção; regras de vida útil |
| Template/concept | Tipo escolhido em compilação | Instanciações e tempo de compilação |
| `std::variant` | Conjunto fechado de alternativas | Visitação e tamanho do maior membro |
| Type erasure (`std::function`, wrappers) | Interface uniforme escondendo tipo | Pode alocar; restrições próprias |

## Move, categorias de valor e elisão

`std::move(x)` é um cast que permite selecionar operações de movimento; não move sozinho. Para tipos da biblioteca padrão, o objeto movido permanece válido, normalmente com estado não especificado salvo garantia específica. Não presuma que um `vector` movido esteja vazio em todo contexto; `unique_ptr` transferido fica vazio.

```cpp
std::string origem = "abc";
std::string destino = std::move(origem);
origem = "novo"; // reutilização válida
```

Não escreva `return std::move(local);` automaticamente: pode impedir NRVO. `const T&&` geralmente não permite o movimento que você espera; construtores de move usuais recebem `T&&` sem const. Marcar move `noexcept` quando a garantia é verdadeira pode permitir que containers prefiram mover durante realocação.

*lvalue* identifica um objeto persistente na expressão; *prvalue* produz um valor; *xvalue* permite reaproveitar recursos. A expressão de um parâmetro nomeado `T&& x` é um lvalue. Para encaminhamento perfeito, use `std::forward<T>(x)` em contexto apropriado de template, não `std::move` indiscriminado.

## Lambdas

```cpp
// <algorithm>, <vector>
std::vector<int> v{2, 9, 4};
int limite = 3;
auto maior = [limite](int x) { return x > limite; };
auto quantidade = std::count_if(v.begin(), v.end(), maior);
```

| Captura | Efeito |
|---|---|
| `[x]` | Copia `x` |
| `[&x]` | Empresta `x` |
| `[p = std::move(p)]` | Move para a closure (C++14) |
| `[this]` | Copia ponteiro; objeto precisa continuar vivo |
| `[*this]` | Copia objeto (C++17) |

Uma lambda retornada/agendada com captura por referência pode ficar pendente. `mutable` permite modificar cópias capturadas, não transforma a cópia em referência. Prefira captura explícita em callbacks duradouros.

## Templates, concepts e compilação

```cpp
// C++20: <concepts>
template<std::integral T>
constexpr bool par(T valor) noexcept {
    return valor % 2 == 0;
}
static_assert(par(12));
```

Concepts restringem operações/tipos aceitos e melhoram diagnósticos; não provam propriedades matemáticas ou ausência de overflow. Para template usado com tipos arbitrários, a definição normalmente precisa estar visível no ponto de instanciação (frequentemente no header). Instanciação explícita permite outros arranjos.

```cpp
// C++17+: <type_traits>
template<class T>
auto magnitude(T x) {
    if constexpr (std::is_unsigned_v<T>) return x;
    else return x < 0 ? -x : x; // exige -x representável; cuidado com INT_MIN
}
```

`if constexpr` descarta ramo por condição constante na instanciação; `if` normal não tem essa função. `constexpr` permite avaliação em compilação quando as condições são atendidas; `consteval` (C++20) exige avaliação imediata; `constinit` (C++20) exige inicialização estática, mas não torna o objeto imutável.

Traits (`std::is_trivially_copyable_v<T>`, `<type_traits>`) descrevem propriedades do tipo, não todas as condições de segurança do seu algoritmo. Evite metaprogramação quando uma função normal expressa o contrato.

## Erros e ausência

| Recurso | Significado | Limite |
|---|---|---|
| `optional<T>` (C++17) | Valor ou ausência | Não transporta motivo da ausência |
| `expected<T,E>` (C++23) | Valor ou erro tipado | Precisa de suporte de biblioteca |
| Exceções | Falha propagada por desenrolamento | Definir fronteiras e garantias; RAII |
| Status/enum | Protocolo explícito | Chamador pode ignorar sem disciplina |

```cpp
// C++23: <expected>; só compila com biblioteca compatível
enum class Erro { divisor_zero };
std::expected<double, Erro> dividir(double a, double b) {
    if (b == 0.0) return std::unexpected(Erro::divisor_zero);
    return a / b; // política de NaN/infinito pertence ao domínio
}
```

Garantia básica: objeto preserva invariantes e não vaza recursos. Garantia forte: a operação falha sem alterar o estado observável pretendido. Garantia de não lançar: a operação não propaga exceções. `noexcept` não captura nem converte falhas: exceção escapando chama `std::terminate`.

Capture por referência (`catch (const std::exception& e)`), não esconda toda falha com `catch (...) {}`. Não atravesse uma ABI C com exceções; traduza na fronteira.

## Ranges e algoritmos

```cpp
// C++20: <algorithm>, <ranges>, <vector>
std::vector<int> v{5, 2, 8, 1};
std::ranges::sort(v);
auto pares = v | std::views::filter([](int x) { return x % 2 == 0; });
for (int x : pares) { /* 2, 8 */ }
```

Views frequentemente são preguiçosas e emprestam a origem; não significam cópia materializada. Preserve vida útil e evite modificar a estrutura do container enquanto itera uma vista sem conhecer as regras. `std::ranges::to` só chega em C++23. Ranges não melhoram automaticamente a complexidade.

## Recursos avançados: quando consultar

| Técnica | Uso concreto | Restrição essencial |
|---|---|---|
| PImpl | Esconder detalhes de implementação/layout | Indireção; destrutor do dono do tipo incompleto definido no `.cpp` |
| Allocators/PMR (C++17) | Controle de alocações por fase | Vida útil do recurso e medições |
| Coroutines (C++20) | Funções suspensíveis/generators/tarefas | Não criam threads nem um event loop automaticamente |
| Modules (C++20) | Fronteiras de importação e compilação | Suporte do toolchain/build; não basta trocar todos os `#include` |
| `std::generator` (C++23) | Gerador baseado em coroutine | Biblioteca compatível; iterar respeitando vida útil |
| SIMD/intrinsics | Processamento vetorial medido | ISA, alinhamento, fallback e distribuição do binário |

Política prática: padrão novo habilitado não significa todos os recursos implementados. Consulte macros de teste de recurso (`<version>`, `__cpp_*`, `__cpp_lib_*`) e compile um pequeno teste da funcionalidade exigida.

