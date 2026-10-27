# Exemplos completos e verificáveis

[Índice da referência](../../referencia-c-cpp.md)

Cada programa executa verificações explícitas e termina com código diferente de zero se alguma falhar. Os exemplos são pequenos para consulta; não constituem uma biblioteca geral pronta para produção. Contratos sobre índices, soma e posse permanecem importantes.

| Programa | Conteúdo | Verificações relevantes |
|---|---|---|
| [C: básico](c/basico.c) | Parsing decimal, soma verificada, qsort, lower bound | Vazio, lixo final, extremos, overflow, duplicados; busca comparada com varredura linear |
| [C: vetor](c/vetor.c) | Crescimento geométrico, realloc, remoção, destruição | 1.000 inserções, preservação de dados, tamanho impossível, erro de índice |
| [C: lista e fila](c/lista_fila.c) | Lista ligada e fila circular fixa | Remoção no meio/cabeça, vazio, cheio e wrap |
| [C++: sequências](cpp/sequencias.cpp) | Parsing, sort, unique/erase, span, map | Ausência, limites, não inserir durante consulta, vetor vazio |
| [C++: grafos](cpp/grafos.cpp) | BFS e Dijkstra | Desconexão, ciclo, índices inválidos, peso negativo, distância máxima e overflow |
| [C++: estruturas](cpp/estruturas.cpp) | DSU e Fenwick | Uniões repetidas, todos os intervalos de um vetor pequeno, atualização e vazio |
| [C++: moderno](cpp/moderno.cpp) | Classe, unique_ptr, RAII, concepts, ranges | Invariante, transferência de posse, destrutor durante exceção |
| [C++: concorrência](cpp/concorrencia.cpp) | jthread, atomic e mutex | 4 threads, contagem e encerramento com join |
| [C/C++: integração](ffi/main.cpp) | Header compartilhado e implementação C | Compilação separada, linkage, retorno de erro |

## Executar com GCC/Clang e Python 3

Na raiz do repositório:

```bash
python3 c-cpp/exemplos/verificar.py
python3 c-cpp/exemplos/verificar.py --release
python3 c-cpp/exemplos/verificar.py --sanitize address,undefined
python3 c-cpp/exemplos/verificar.py --cc clang --cxx clang++
```

O script usa `-Werror` para que estes exemplos não acumulem avisos. `--release` usa `-O2 -DNDEBUG`; as verificações continuam ativas. `--sanitize thread` executa somente o programa de concorrência, em build separado. Disponibilidade dos runtimes depende do sistema.

## Executar com CMake e CTest

```bash
cmake -S c-cpp/exemplos -B build/c-cpp -DCMAKE_BUILD_TYPE=Debug
cmake --build build/c-cpp --config Debug
ctest --test-dir build/c-cpp -C Debug --output-on-failure
```

Para GCC/Clang com runtimes suportados, configure uma pasta separada com `-DREF_SANITIZERS=ON`. No Windows/MSVC use o ambiente de desenvolvimento e gerador apropriados. Veja as [receitas de plataformas](../08-plataformas-build.md).

## Escopo dos testes

Falha de alocação real não é injetada; o vetor testa rejeição de tamanho impossível e tratamento no código, mas isso não substitui teste com allocator controlado. O Fenwick exige que acumulados caibam em `long long`. Dijkstra rejeita um candidato explorado que transborde, mesmo que outro caminho representável possa existir: é uma política conservadora explícita do exemplo.

Os fragmentos nos capítulos não são todos programas independentes. C23 e C++23 são referências identificadas, fora do build base C17/C++20. Resultados executados: [VALIDACAO.md](VALIDACAO.md).

