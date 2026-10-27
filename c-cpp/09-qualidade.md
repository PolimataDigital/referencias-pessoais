# 09 · Testes, depuração e desempenho

[Índice](../referencia-c-cpp.md) · Intermediário a avançado

## Ciclo curto de diagnóstico

1. Reproduza com entrada pequena e comando exato.
2. Classifique: erro de compilação, link, carregamento, execução ou lógica.
3. Confira avisos, estado das variáveis e contrato da função.
4. Use sanitizer/debugger conforme a suspeita.
5. Corrija a causa e execute o caso que falhou, mais as fronteiras relevantes.

| Sintoma | Causa frequente | Conferir |
|---|---|---|
| `undefined reference` / unresolved external | Fonte/biblioteca não entrou no link ou assinatura diferente | Targets, objetos, linkage C/C++ |
| Multiple definition | Definição não inline em vários headers/unidades | Header declara, fonte define |
| Funciona em Debug, quebra em Release | UB, variável não inicializada, dependência de ordem | Sanitizers, warnings, vida útil |
| Falha só com entrada grande | Overflow, profundidade recursiva, complexidade | Limites e custo assintótico |
| Crash depois de `push_back` | Ponteiro/iterador invalidado por realocação | Reobter posição ou redesenhar posse |
| Travamento com threads | Deadlock, corrida ou espera sem condição de saída | Ordem de locks, predicado, encerramento |

## Avisos e análise estática

GCC/Clang: `-Wall -Wextra -Wpedantic -Wconversion -Wshadow`. `-Wall` não ativa todos os avisos. MSVC: `/W4` e `/permissive-` para C++. `-Werror`/`/WX` é útil em ambiente controlado; versões novas podem introduzir diagnósticos, então documente o toolchain.

Ferramentas: `clang-tidy` para diagnósticos orientados a regras; `clang-format` para estilo; analisadores de GCC/Clang/MSVC para fluxos suspeitos. Configuração deve corresponder às flags reais do build. “Zero avisos” é evidência útil, não prova de correção.

## Sanitizers

```bash
# Linux/GCC ou Clang com runtimes disponíveis
g++ -std=c++20 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined c-cpp/exemplos/cpp/sequencias.cpp -o exemplo_san
./exemplo_san

# Corridas: build separado, sem ASan
g++ -std=c++20 -g -O1 -fsanitize=thread -pthread c-cpp/exemplos/cpp/concorrencia.cpp -o exemplo_tsan
./exemplo_tsan
```

| Ferramenta | Ajuda a encontrar | Não garante |
|---|---|---|
| AddressSanitizer | Muitos acessos fora de limites, use-after-free | Detectar todo bug/lifetime lógico |
| UndefinedBehaviorSanitizer | Diversas formas de UB, inclusive certos overflows signed | Cobrir todo UB da linguagem |
| ThreadSanitizer | Data races em execuções observadas | Ausência de deadlock/erro de protocolo |
| LeakSanitizer | Alocações perdidas, conforme plataforma | Correção de toda propriedade lógica |
| MemorySanitizer | Leituras de memória não inicializada, em ambiente suportado | Funcionar sem instrumentar dependências relevantes |

Não combine TSan e ASan na mesma execução. Suporte depende de compilador, sistema e runtime. Sanitizer precisa executar o caminho defeituoso; complemente com cobertura, análise e revisão. Binário instrumentado não é a medição de desempenho de produção.

## Debugger: correspondências úteis

| Ação | GDB | LLDB |
|---|---|---|
| Abrir | `gdb ./app` | `lldb ./app` |
| Breakpoint | `break funcao` | `breakpoint set --name funcao` |
| Executar | `run` | `run` |
| Próxima linha | `next` | `next` |
| Entrar em chamada | `step` | `step` |
| Continuar | `continue` | `continue` |
| Variável | `print variavel` | `frame variable variavel` |
| Pilha | `backtrace` | `bt` |

Compile com símbolos (`-g` ou `/Zi`). Otimização pode reorganizar/eliminar variáveis; use Debug para inspeção inicial e um build otimizado com símbolos quando o bug depender dessa configuração. No Windows, Visual Studio/WinDbg também oferecem breakpoints, pilha e inspeção de memória.

## Testes que verificam contratos

- Parsing: vazio, espaços, sinal, sobra de texto, limites, overflow, ponteiro de saída nulo quando a API permite rejeitar.
- Container: vazio, crescer várias vezes, preservar elementos, fronteiras, remoção total; falha de alocação precisa de injeção para teste determinístico.
- Ordenação: conferir ordem e preservação dos elementos; duplicados e extremos.
- Busca: comparar com uma busca linear independente em vários vetores pequenos.
- Grafos: conferir inalcançáveis, múltiplos caminhos, restrições de peso, reconstrução quando oferecida.
- Integração: compilar C e C++ separadamente e linkar; não “validar C” compilando tudo como C++.

`assert` pode desaparecer com `NDEBUG`; não coloque validação de entrada externa ou efeitos necessários dentro dele. Os exemplos deste material usam verificações explícitas que continuam ativas em Release. Testes de propriedade (por exemplo, ordenar não perde elementos) e fuzzing de parsers acham classes de falhas que exemplos isolados podem não cobrir.

## Medir antes de otimizar

| Passo | Razão |
|---|---|
| Escolher algoritmo/estrutura | Trocar O(n²) por O(n log n) costuma superar ajuste local |
| Medir carga representativa | Entrada minúscula ou uniforme pode esconder o problema real |
| Separar I/O, alocação e cálculo | Saber onde o tempo e a memória são gastos |
| Usar Release e várias amostras | Debug e uma execução não representam desempenho |
| Preservar resultado observável | Compilador pode eliminar cálculo sem efeito |
| Usar relógio apropriado | `steady_clock` para duração monotônica; `clock()` é CPU em C |
| Analisar perfil | perf/Instruments/Visual Studio Profiler conforme sistema |

Localidade importa: array contíguo costuma aproveitar cache melhor que muitos nós espalhados. Reservar capacidade uma vez pode reduzir realocações; chamar `reserve(size()+1)` a cada inserção pode destruir o crescimento eficiente. Evite cópias caras por acidente em `for (auto x : objetos)` quando só precisa observar (`const auto&`).

Não use `-ffast-math`, intrinsics, alinhamentos especiais, arenas ou lock-free como decoração: cada um muda hipóteses, portabilidade e manutenção. `-ffast-math` permite transformações que alteram garantias numéricas, incluindo tratamento de NaN e ordem de cálculo. Documente precisão aceitável.

