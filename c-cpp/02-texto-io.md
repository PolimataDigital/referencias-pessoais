# 02 · Texto, entrada, saída e arquivos

[Índice](../referencia-c-cpp.md) · Básico a intermediário

## Strings: comparar operação e contrato

| Operação | C17 | C++20 |
|---|---|---|
| Possuir texto | `char nome[32] = "Ana";` | `std::string nome = "Ana";` |
| Observar literal | `const char *s = "abc";` | `std::string_view s = "abc";` |
| Comprimento | `strlen(s)` O(L), exige terminador | `s.size()` O(1) |
| Comparar conteúdo | `strcmp(a, b) == 0` | `a == b` para `std::string`/`string_view` |
| Buscar caractere | `strchr(s, 'x')`, retorna ponteiro ou `NULL` | `s.find('x')`, retorna índice ou `npos` |
| Buscar trecho | `strstr(s, "abc")` | `s.find("abc")` |
| Substring | Copiar com capacidade e terminador, ou vista ponteiro+tamanho | `s.substr(pos, qtd)`; em `string` copia, em `string_view` empresta |
| Concatenar | Capacidade deve comportar ambos + `\0` | `s += trecho`; pode realocar |
| Converter número para texto | `snprintf` | `std::to_string`; `std::to_chars` para buffer |
| Separar tokens | Percorrer ou `strtok` (altera buffer/usa estado interno) | `getline(stream, token, delimitador)` ou ranges (C++20) |
| Remover caracteres | Compactar buffer e terminar | `std::erase_if(s, predicado)` (C++20) |
| Passar para API C | Ponteiro válido para texto terminado | `s.c_str()`; não guardar além da vida/validade da string |

Headers C: `<string.h>`, `<stdio.h>`, `<stdlib.h>`, `<ctype.h>`. Headers C++ conforme uso: `<string>`, `<string_view>`, `<charconv>`, `<sstream>`, `<algorithm>`, `<cctype>`.

`char *` não armazena capacidade nem comprimento. `sizeof ponteiro` não mede o texto. `sizeof "abc" == 4` inclui `\0`, enquanto `strlen("abc") == 3`. Literais de string não devem ser modificados em nenhuma das linguagens. `std::string` pode conter `\0` interno; APIs C normalmente interpretarão só o prefixo.

`std::string::size()` conta unidades `char` (bytes usuais), não caracteres visuais Unicode. Acentos e emojis podem ocupar várias unidades. `tolower` não faz transformação Unicode de uma string UTF-8 byte a byte.

## Entrada e saída no terminal

| Intenção | C17 | C++20 |
|---|---|---|
| Ler inteiro controlado | `scanf("%d", &n) == 1` | `if (std::cin >> n)` |
| Ler linha | `fgets(buffer, sizeof buffer, stdin)` | `std::getline(std::cin, linha)` |
| Escrever inteiro | `printf("%d\n", n)` | `std::cout << n << '\n';` |
| Duas casas decimais | `printf("%.2f\n", x)` | `std::fixed << std::setprecision(2) << x` (`<iomanip>`) |
| Tamanho | `printf("%zu\n", n)` para `size_t` | `std::cout << n` |
| Diagnóstico | `fprintf(stderr, "falha\n")` | `std::cerr << "falha\n";` |
| Leitura byte a byte | `int c; while ((c = getchar()) != EOF)` | `char c; while (std::cin.get(c))` |

Em `scanf`, `%f` recebe `float *` e `%lf` recebe `double *`; em `printf`, um `float` é promovido a `double` e `%f` imprime ambos. Para inteiros de largura fixa em C, use macros como `PRId64` de `<inttypes.h>`.

**Parsing robusto:** `scanf("%d", ...)` não é uma boa fronteira para números arbitrariamente grandes de entrada não confiável: conversão fora da faixa é problemática. Ler texto e converter com checagem explícita torna o contrato controlável. `fgets` lê no máximo capacidade − 1 bytes; ausência de `\n` pode indicar linha truncada. Rejeite/descarte o restante ou acumule a linha.

Ao misturar `>>` com `getline`, a quebra de linha pode permanecer no stream:

```cpp
// <iostream>, <limits>, <string>; trecho após leitura numérica bem-sucedida
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::string linha;
std::getline(std::cin, linha);
```

Se houve falha de extração, use `clear()` antes de descartar a linha. `std::getline(std::cin >> std::ws, linha)` também consome todos os espaços iniciais e linhas em branco; use só quando isso corresponder à intenção. `clear()` sozinho não remove o texto inválido.

Em programação competitiva, antes de qualquer I/O:

```cpp
std::ios::sync_with_stdio(false);
std::cin.tie(nullptr);
```

Depois disso, evite misturar I/O C e C++ nos mesmos streams. Use `\n` para quebra de linha; `std::endl` também força flush. Em interação com juiz/terminal, faça flush quando o protocolo precisar.

## Texto para número sem aceitar lixo silenciosamente

**C17**, decimal, aceita espaços iniciais e finais e sinal; exige toda a entrada válida:

```c
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

bool parse_int(const char *texto, int *saida) {
    if (texto == NULL || saida == NULL) return false;
    errno = 0;
    char *fim;
    long valor = strtol(texto, &fim, 10);
    if (fim == texto || errno == ERANGE ||
        valor < INT_MIN || valor > INT_MAX) return false;
    while (isspace((unsigned char)*fim)) ++fim;
    if (*fim != '\0') return false;
    *saida = (int)valor;
    return true;
}
```

**C++17+**, decimal estrito, sem espaços nem sinal `+` (o `-` é aceito para `int`):

```cpp
#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

std::optional<int> parse_int(std::string_view texto) {
    if (texto.empty()) return std::nullopt;
    int valor{};
    const char* fim = texto.data() + texto.size();
    auto [pos, erro] = std::from_chars(texto.data(), fim, valor);
    if (erro != std::errc{} || pos != fim) return std::nullopt;
    return valor;
}
```

As políticas diferem intencionalmente. Normalizar espaços é uma decisão da aplicação. `atoi` não distingue claramente erro e zero; `stoi` pode lançar e aceitar prefixo válido se a posição final for ignorada. [Versões completas testadas](exemplos/README.md).

## Buffers, caracteres e formatação

```c
/* <stdio.h>: buffer com capacidade conhecida */
char mensagem[32];
int r = snprintf(mensagem, sizeof mensagem, "id=%d", 42);
if (r < 0 || (size_t)r >= sizeof mensagem) {
    /* erro ou truncamento: não tratar a saída como mensagem completa */
}
```

- `snprintf` retorna o tamanho necessário, excluindo `\0`, quando não há erro.
- `strncpy` pode deixar o destino sem terminador e preencher muitos bytes: não é substituta universal de cópia segura.
- `memcpy` exige regiões sem sobreposição; `memmove` admite sobreposição.
- Para `isalpha`, `isdigit`, `tolower` e semelhantes, passe `EOF` ou um valor representável como `unsigned char`: `tolower((unsigned char)c)` em C; `std::tolower(static_cast<unsigned char>(c))` em C++.
- `printf(texto_usuario)` interpreta o texto como formato. Use `printf("%s", texto_usuario)`.

## Arquivos e persistência

| Operação | C17 | C++20 |
|---|---|---|
| Abrir leitura binária | `fopen(caminho, "rb")` | `std::ifstream in(caminho, std::ios::binary)` |
| Abrir escrita binária | `fopen(caminho, "wb")` | `std::ofstream out(caminho, std::ios::binary)` |
| Acrescentar | Modo `"ab"` | `std::ios::app` |
| Ler bloco | `fread(buf, 1, cap, arq)` retorna bytes lidos | `in.read(buf, qtd)` e `in.gcount()` |
| Verificar erro | `ferror(arq)`; `perror` quando pertinente | Estado do stream ou exceções configuradas |
| Fechar | `fclose(arq)`; conferir retorno em escrita | RAII; para conferir erro final, `close()` e testar estado |

Não use `while (!feof(arq))` nem `while (!in.eof())`: o fim só é sinalizado depois da tentativa de leitura. Faça a leitura controlar o laço.

`fwrite(&registro, sizeof registro, 1, arq)` não cria um formato binário portátil: padding, endianness, tamanho dos tipos, ponteiros e versões mudam. Defina campos, ordem de bytes, comprimentos, versão e limites; ou use um formato documentado. Em Windows, modo texto pode traduzir quebras de linha. Para bytes exatos, use binário.

Flush de biblioteca não garante persistência física após queda de energia. `fsync`/`FlushFileBuffers` são APIs de sistema com contratos próprios.

## Mapa de bibliotecas úteis

| Assunto | C | C++ |
|---|---|---|
| Matemática | `<math.h>`: `sqrt`, `hypot`, `isfinite` | `<cmath>`: `std::sqrt`, `std::hypot`, `std::isfinite` |
| Acumular/MDC | Funções próprias | `<numeric>`: `accumulate`, `partial_sum`, `gcd`, `lcm` |
| Aleatoriedade | `rand` tem limitações; não criptográfico | `<random>`: `mt19937`, `uniform_int_distribution`; não criptográfico |
| Tempo civil | `<time.h>` | `<chrono>` e `<ctime>` |
| Duração monotônica | API de plataforma; `clock()` mede tempo de CPU | `std::chrono::steady_clock` |
| Assert | `<assert.h>` | `<cassert>`; não serve para validar entrada externa |
| Formatação moderna | `snprintf` | `std::format` (C++20), `std::print` (C++23), suporte a conferir |

Use semente fixa para testes de aleatoriedade reproduzíveis. `std::random_device` pode depender da implementação; nenhum desses exemplos substitui uma API criptográfica. Evite `rand() % n` quando uniformidade importa. `pow` opera em ponto flutuante; exponenciação inteira exige rotina e verificação de overflow.

