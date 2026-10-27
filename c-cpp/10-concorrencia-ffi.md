# 10 · Concorrência, integração e fronteiras

[Índice](../referencia-c-cpp.md) · Avançado a especializado

## Concorrência: qual ferramenta resolve qual problema?

| Necessidade | C | C++ |
|---|---|---|
| Thread | `<threads.h>` (C11, conferir disponibilidade) ou API do sistema | `std::thread` (C++11), `std::jthread` (C++20) |
| Exclusão mútua | `mtx_t` ou primitiva da plataforma | `std::mutex`, `std::lock_guard`, `std::unique_lock` |
| Esperar por condição | `cnd_t` ou plataforma | `std::condition_variable` |
| Operação atômica | `_Atomic`, `<stdatomic.h>` (C11) | `std::atomic<T>`, `<atomic>` |
| Resultado futuro | API/biblioteca | `std::future`, `std::promise`, `std::async` |
| Cancelamento cooperativo | Estado/protocolo próprio | `std::stop_token` e `std::jthread` (C++20) |

Data race ocorre quando acessos conflitantes à mesma região não são devidamente sincronizados e pelo menos um escreve; nas condições definidas pelos padrões, é UB. `volatile` não resolve. Proteja o **invariante inteiro**, não apenas uma variável isolada.

```cpp
// C++11+: <mutex>
std::mutex mutex_estoque;
int estoque = 10;

bool retirar(int quantidade) {
    std::lock_guard<std::mutex> trava(mutex_estoque);
    if (quantidade < 0 || quantidade > estoque) return false;
    estoque -= quantidade;
    return true;
}
```

Toda leitura/escrita concorrente de `estoque` precisa seguir o mesmo protocolo. Fazer separadamente `load`, teste e `store` em uma variável atômica não torna a operação composta indivisível.

## Vida útil, espera e encerramento

- `std::thread` ainda joinable ao ser destruída causa `std::terminate`. Planeje `join`; `detach` exige outro contrato de vida útil e costuma dificultar o encerramento.
- `std::jthread` solicita parada e faz join ao destruir, mas o trabalhador precisa observar o pedido para parar cedo. O destrutor ainda pode bloquear.
- `condition_variable::wait(lock, predicado)` testa o predicado, libera o mutex ao esperar e o readquire antes de continuar. O predicado lida com despertares espúrios.
- Proteja estado da condição e mudança desse estado com o protocolo correto; notificação não substitui o estado.
- Evite chamar callback desconhecido ou fazer I/O lento sob lock sem necessidade. Para vários mutexes, estabeleça ordem ou use `std::scoped_lock` (C++17) quando adequado.
- `std::async` sem política explícita pode adiar execução. Future não significa sempre thread nova.

## Atomics e ordem de memória

```cpp
// C++11+: <atomic>
std::atomic<unsigned long long> contador{0};
void registrar_evento() {
    contador.fetch_add(1, std::memory_order_relaxed);
}
```

`relaxed` é suficiente para a contagem isolada quando ela não publica outros dados. Não use esse exemplo como bandeira de “dados prontos”. `seq_cst` é um ponto de partida mais simples para raciocinar, mas não torna todo algoritmo correto.

| Ordem | Ideia de uso | Limite |
|---|---|---|
| Relaxed | Atomicidade isolada | Não estabelece ordenação de outros dados |
| Release / acquire | Publicação e observação num protocolo correspondente | Precisa de relação de sincronização correta |
| Seq_cst | Ordem global para operações seq_cst | Não corrige invariantes divididos em operações separadas |

Atomics podem internamente usar locks (`is_lock_free` ajuda a consultar). Lock-free não significa wait-free nem necessariamente mais rápido. ABA, liberação de memória concorrente e ordem de memória tornam estruturas lock-free um tema especializado. Prefira mutex até haver necessidade e prova adequadas.

## API C consumida por C++

```c
/* ref_api.h */
#ifndef REF_API_H
#define REF_API_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Retorna 1 se soma couber em int e saída for válida; 0 no erro. */
int ref_somar(int a, int b, int *saida);

#ifdef __cplusplus
}
#endif
#endif
```

`extern "C"` especifica linkage de linguagem, inclusive convenções relevantes do toolchain; não converte qualquer interface C++ em ABI portátil. Não exponha `std::string`, templates, exceções ou layout de classes como se fossem tipos C.

[Exemplo completo C + C++](exemplos/ffi/api.h): a implementação `.c` é compilada como C e chamada por `.cpp`. A variante de API opaca deve oferecer funções de criação/destruição no mesmo lado.

Quando C++ implementa uma API C, capture exceções na fronteira e retorne status conforme contrato. Não deixe uma exceção atravessar código C que não foi projetado para esse mecanismo. Defina thread-safety, ownership e validade de buffers também na documentação da ABI.

## Pontes com Node.js e Python

| Integração | Contrato relevante |
|---|---|
| Processo externo (stdin/stdout/arquivo) | Formato, encoding, timeout, código de saída e tamanho de mensagens |
| Node.js via Node-API | Handles/vida útil do runtime; operações pesadas não devem bloquear o event loop |
| Python via API de extensão/FFI | Referências, vida útil dos buffers e regras da versão/runtime quanto a threads |
| C ABI para várias linguagens | Tipos simples, tamanhos, status, função de liberação explícita |

Use processo separado quando isolamento e simplicidade bastarem. Native addon/extension só compensa quando há requisito concreto de integração/desempenho. Consulte a documentação da versão do runtime antes de implementar; regras de ABI e concorrência evoluem.

## Sistemas, rede e segurança prática

C/C++ padrão nas bases deste material não oferecem uma API universal de sockets, processos, serial ou GUI. POSIX e Win32 são APIs distintas; mantenha adaptadores pequenos atrás de uma interface comum. POSIX `read/write` e sockets podem realizar transferências parciais; trate retorno e interrupção conforme contrato. Winsock possui inicialização e handles próprios.

Evite interpolar entrada do usuário em `system()`/shell. Use APIs de processo com argumentos separados. Para SQL, use parâmetros do driver; para criptografia, biblioteca estabelecida e APIs documentadas. Parsers devem validar comprimento **antes** de alocar, somar offsets ou copiar. Reserve limites de recurso para evitar crescimento ilimitado.

## Quando separar módulos por plataforma

Mantenha o algoritmo e os modelos de dados independentes do sistema. Implemente acesso a arquivos especiais, sockets, relógios ou processos em arquivos específicos (`plataforma_windows.c`, `plataforma_posix.c`), selecionados pelo build. Use `#if` em fronteiras pequenas; não espalhe decisões de SO dentro de cada algoritmo.

