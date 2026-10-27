# Validação desta versão

Data: **27/10/2026**. Ambiente executado: Linux, GCC e G++ **13.3.0**. Base: C17 / C++20. As verificações têm falha explícita, independentemente de `NDEBUG`.

| Execução | Resultado |
|---|---|
| Debug, `-O0 -g`, avisos + `-Werror` | 9/9 executáveis aprovados |
| Release, `-O2 -DNDEBUG`, avisos + `-Werror` | 9/9 executáveis aprovados |
| AddressSanitizer + UndefinedBehaviorSanitizer | 9/9 aprovados com detecção de vazamentos desativada |
| Link misto C/C++ | `api.c` compilado com GCC como C17; consumidor/link com G++ como C++20; aprovado |
| Links relativos e tabelas Markdown dos novos documentos | Conferidos localmente |

## Contagens por execução

| Programa | Verificações |
|---|---:|
| C: básico | 47 |
| C: lista/fila | 22 |
| C: vetor | 2.011 |
| C++: concorrência | 2 |
| C++: estruturas | 31 |
| C++: grafos | 11 |
| C++: moderno | 8 |
| C++: sequências | 22 |
| Integração C/C++ | 5 |
| **Total por configuração** | **2.159** |

Grande parte da contagem verifica inserção/preservação de elementos do vetor. Quantidade de verificações não é uma medida de cobertura completa ou uma prova de correção.

## Comandos reproduzíveis

Na raiz do repositório:

```bash
python3 c-cpp/exemplos/verificar.py
python3 c-cpp/exemplos/verificar.py --release
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 python3 c-cpp/exemplos/verificar.py --sanitize address,undefined
```

A compilação instrumentada foi feita pelo script; após a limitação abaixo, os nove binários resultantes foram executados com essas variáveis. Em uma máquina sem a limitação, prefira a execução padrão para também aproveitar detecção de vazamentos quando disponível.

## Limitações observadas

- A primeira execução com LeakSanitizer ativo falhou porque o runtime não conseguiu ler `/proc/.../task` neste ambiente. Isso é uma limitação da execução instrumentada, não um resultado de “sem vazamentos”. O teste subsequente usou `ASAN_OPTIONS=detect_leaks=0`; ASan e UBSan permaneceram ativos.
- Windows/MSVC, macOS/Apple Clang, Clang/Linux e ARM não estavam disponíveis para execução. Seus comandos são receitas documentadas, não testes aprovados nesta sessão.
- CMake/CTest não estavam instalados neste ambiente. O arquivo de build foi revisado; a execução local usou compilação direta pelo script Python.
- ThreadSanitizer não foi executado. A contagem concorrente passou nas configurações acima, mas isso não comprova ausência geral de corridas.
- Não foi feita injeção de falha real de alocação; apenas a rejeição de tamanho impossível foi exercitada.
- Fragmentos de C23/C++23, modules e coroutines não integram o build de exemplos base.

