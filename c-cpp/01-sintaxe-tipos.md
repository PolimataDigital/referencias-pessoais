# 01 · Sintaxe, tipos e operadores

[Índice](../referencia-c-cpp.md) · Básico a intermediário · C17 / C++20

## Programa, nomes e inicialização

| Operação | C | C++ |
|---|---|---|
| Ponto de entrada hospedado | `int main(void) { return 0; }` | `int main() { return 0; }` |
| Argumentos de linha de comando | `int main(int argc, char **argv)` | Mesma assinatura |
| Comentário | `// linha` ou `/* bloco */` | Igual |
| Incluir biblioteca | `#include <stdio.h>` | `#include <iostream>` |
| Inicializar escalar | `int x = 7;` | `int x{7};` |
| Zerar agregado | `int a[4] = {0};` | `std::array<int, 4> a{};` |
| Alias | `typedef unsigned long Contador;` | `using Contador = unsigned long;` |
| Namespace | Prefixos: `app_abrir` | `namespace app { void abrir(); }` |
| Constância | `const int x = 3;` | `const int x = 3;` |
| Constante de compilação | `enum { LIMITE = 16 };` em contextos inteiros | `constexpr int limite = 16;` |

`const` impede modificação pelo caminho de acesso correspondente; não significa automaticamente avaliação em compilação. Em C17, `const int n = 4; int a[n];` dentro de função pode produzir um VLA, se suportado. Para tamanho fixo portátil em C, use expressão constante inteira (`enum`, literal ou macro). C++ padrão não tem VLA.

Em C++, `{}` impede várias conversões redutoras, mas pode selecionar `initializer_list`: `std::vector<int> a(3, 7)` contém três valores 7; `std::vector<int> b{3, 7}` contém dois elementos. `auto` infere um tipo estático, e não cria uma variável de tipo dinâmico.

## Tipos e limites

| Necessidade | C | C++ | Observação |
|---|---|---|---|
| Inteiro usual | `int` | `int` | Pelo menos 16 bits; não assumir 32 em todo alvo |
| Inteiro maior | `long long` | `long long` | Pelo menos 64 bits; não é precisão arbitrária |
| Tamanho/índice não negativo | `size_t`, `<stddef.h>` | `std::size_t`, `<cstddef>` | Resultado de `sizeof`; unsigned |
| Diferença de ponteiros | `ptrdiff_t` | `std::ptrdiff_t` | Signed; diferença precisa ser representável |
| Largura exata | `int32_t`, `<stdint.h>` | `std::int32_t`, `<cstdint>` | Tipos exatos são opcionais se o alvo não os oferece |
| Largura mínima | `int_least32_t` | `std::int_least32_t` | Pelo menos a largura pedida |
| Limite de `int` | `INT_MIN`, `INT_MAX`, `<limits.h>` | `std::numeric_limits<int>::min()/max()`, `<limits>` | Consulte, não adivinhe |
| Limite de ponto flutuante | `DBL_MAX`, `<float.h>` | `std::numeric_limits<double>` | `min()` é positivo normal; `lowest()` é o mais negativo |
| Booleano | `<stdbool.h>` | Nativo | C23 torna `bool`, `true`, `false` palavras-chave |
| Bytes | `unsigned char` | `std::byte`, `<cstddef>` (C++17) | `std::byte` não é um pequeno inteiro aritmético |

`sizeof(char) == 1`, mas um byte da linguagem não precisa ter 8 bits: consulte `CHAR_BIT`. `char` pode ser signed ou unsigned. `long` tem 32 bits em Windows 64-bit usual e 64 bits em Linux/macOS 64-bit usuais; isso é propriedade da ABI, não da palavra “64-bit”.

## Operadores e controle

| Operação | Sintaxe comum | Cuidado |
|---|---|---|
| Atribuição / igualdade | `x = y` / `x == y` | `if (x = y)` atribui |
| Aritmética | `+ - * / %` | `%` é resto inteiro, não porcentagem |
| Divisão inteira | `7 / 2` resulta em `3` | `7 / 2.0` resulta em `3.5` |
| Negativos | `-7 / 3 == -2`; `-7 % 3 == -1` | Truncamento em direção a zero (bases aqui usadas) |
| Lógicos | `&&`, `\|\|`, `!` | Os dois primeiros têm curto-circuito |
| Bits | `&`, `\|`, `^`, `~`, `<<`, `>>` | Use tipos unsigned para máscaras |
| Incremento | `++i`, `i++` | Evite combinar várias modificações numa expressão |
| Ternário | `cond ? a : b` | Só o ramo escolhido é avaliado |
| Laços | `for`, `while`, `do ... while` | `do` executa pelo menos uma vez |
| Seleção | `if`, `else`, `switch` | `switch` pode continuar no próximo `case` |
| Saída de fluxo | `break`, `continue`, `return` | `break` sai apenas do laço/switch mais interno |

```c
/* C17: tamanho_t e dados já válidos; <stddef.h> */
for (size_t i = 0; i < n; ++i) { /* usar dados[i] */ }
/* Reverso unsigned sem i >= 0 e sem decrementar zero: */
for (size_t i = n; i > 0; ) {
    --i;
    /* usar dados[i] */
}
```

```cpp
// C++20: <vector>
std::vector<int> valores{2, 4, 6};
for (int& valor : valores) { valor *= 2; }
for (const int valor : valores) { /* cópia barata, só leitura */ }
```

Parênteses esclarecem precedência: `(flags & mascara) != 0`, `a < b && b < c`. `a < b < c` compara primeiro `a < b` e depois o resultado com `c`; não representa um intervalo matemático.

## Funções e passagem de argumentos

```c
/* C17: a função recebe uma cópia do ponteiro; *valor acessa o objeto. */
void dobrar(int *valor) { *valor *= 2; } /* exige não nulo e sem overflow */
int dobro(int valor) { return valor * 2; } /* exige resultado representável */
```

```cpp
// C++20
void dobrar(int& valor) { valor *= 2; }
int dobro(int valor) { return valor * 2; }
```

Em C17, declare ausência de parâmetros com `f(void)`: uma declaração `int f();` não fornece um protótipo com tipos de parâmetros. Em C++, `f()` significa nenhum parâmetro. C23 altera a regra de C. Referência C++ não pode ser reselecionada para outro objeto após inicialização; um ponteiro pode.

## Conversões

| Caso | C | C++ |
|---|---|---|
| Conversão numérica explícita | `(double)a / b` | `static_cast<double>(a) / b` |
| Inteiro de tamanho para `int` | Verificar `n <= INT_MAX` antes do cast | `std::in_range<int>(n)` (C++20), `<utility>`, antes do cast |
| Ponteiro genérico alocado | `int *p = malloc(n * sizeof *p);` | Não usar `malloc` para criar objetos C++ usuais |
| Conversão em hierarquia | API manual | `dynamic_cast` para polimorfismo; `static_cast` só com garantia |
| Remover qualificador | Cast pode esconder erro de projeto | `const_cast`; modificar objeto originalmente `const` é UB |
| Reinterpretar representação | `memcpy`/acesso via bytes | `std::bit_cast` (C++20) com tipos adequados; não valida qualquer representação |

Misturar signed/unsigned pode converter o negativo para um valor enorme. Não “corrija” o aviso com cast automático. Em C++20, `<utility>` oferece `std::cmp_less`, `std::cmp_equal` e `std::in_range<T>` para comparações/faixa de inteiros suportados.

## Aritmética segura

- Overflow **signed** é comportamento indefinido nas bases adotadas. Unsigned calcula módulo `2^N`; wrap definido ainda pode ser um bug de tamanho.
- Faça a conversão **antes** da operação: `long long p = 1LL * a * b;` só resolve se o produto couber em `long long`.
- Antes de alocar `n * sizeof(T)`, verifique `n <= SIZE_MAX / sizeof(T)`.
- `abs(INT_MIN)` pode não ser representável; `INT_MIN / -1` também é problema. `std::gcd` exige que os valores absolutos caibam no tipo comum.
- Para máscara, não desloque por quantidade negativa ou maior/igual à largura do operando promovido.
- Dinheiro: prefira unidade mínima inteira com validação de faixa ou biblioteca decimal; `double` não representa todo valor decimal exatamente.

Exemplo de adição verificada em C17 (`<limits.h>`, `<stdbool.h>`):

```c
bool somar_int(int a, int b, int *saida) {
    if (saida == NULL) return false; /* <stddef.h> */
    if ((b > 0 && a > INT_MAX - b) ||
        (b < 0 && a < INT_MIN - b)) return false;
    *saida = a + b;
    return true;
}
```

Para aproximações numéricas, escolha tolerância segundo o domínio: `|a-b| <= atol + rtol * max(|a|,|b|)`. Isso não é um comparador válido de ordenação em geral e exige tratamento apropriado de NaN/infinito. `std::numeric_limits<double>::epsilon()` descreve espaçamento perto de 1, não uma tolerância universal.

**Novidades identificadas:** C23 tem `<stdckdint.h>` (`ckd_add`, `ckd_sub`, `ckd_mul`) e inferência com `auto`. Confira suporte de headers e compilador; os exemplos base não dependem deles.

