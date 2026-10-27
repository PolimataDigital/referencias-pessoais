# 12 · Padrões, fontes e livros

[Índice](../referencia-c-cpp.md)

## Política de versões deste material

Base escolhida: **C17 para C** e **C++20 para C++**. Isso permite separar o núcleo de consulta das novidades cujo suporte varia. Não significa que C17 seja a edição mais recente nem que todo recurso de C++20 funcione em qualquer toolchain.

| Padrão | Recursos relevantes para reconhecer |
|---|---|
| C99 | `//`, declarar no `for`, designadores, `stdint.h`, membro flexível, `restrict` |
| C11 | `_Static_assert`, `_Generic`, alinhamento, atomics e threads com condições de suporte |
| C17 | Revisão com correções; base dos exemplos C |
| C23 | `nullptr`, `bool` nativo, inferência `auto`, `<stdckdint.h>` e outras mudanças; não presumir identidade com C++ |
| C++11 | RAII já existia; chegam move, lambdas, `unique_ptr`, `shared_ptr`, threads, `nullptr`, `constexpr` |
| C++14 | `make_unique`, lambdas genéricas, capturas com inicializador |
| C++17 | `optional`, `variant`, `string_view`, `filesystem`, structured bindings, `if constexpr`, PMR |
| C++20 | Concepts, ranges, `span`, `jthread`, coroutines, modules, `format` |
| C++23 | `expected`, `print`, `ranges::to`, `generator`, entre outros; suporte por recurso |

C++26/rascunhos posteriores ficam fora da base. As páginas de *working draft* evoluem e podem mostrar recursos posteriores a C++20/23. Consulte a edição pertinente antes de interpretar uma regra nova como retroativa.

## Fontes primárias para conferir regras

Links consultados em **27/10/2026**. A data registra consulta, não promete suporte uniforme. Os exemplos são originais e não transcrevem programas dos livros.

| Fonte | Uso no material |
|---|---|
| [WG14 — comitê de C](https://www.open-std.org/jtc1/sc22/wg14/) | Evolução e documentos oficiais de C |
| [N1570 — rascunho público C11](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf) | Consulta das regras fundamentais de linguagem/biblioteca; não apresentado como texto final de C17 |
| [N3096 — rascunho público de C23](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf) | Comparação de novidades; não substitui a edição ISO publicada |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) | Orientações de projeto, RAII, interfaces e ownership; diretrizes, não norma |
| [Working draft C++ — vector](https://eel.is/c++draft/vector) | Complexidade, armazenamento e validade de acessos |
| [Working draft C++ — algorithms](https://eel.is/c++draft/algorithms) | Contratos de algoritmos, comparadores e categorias de iteradores |
| [Working draft C++ — expected](https://eel.is/c++draft/expected) | Modelo de valor ou erro; checar disponibilidade na versão usada |
| [GCC — standards](https://gcc.gnu.org/onlinedocs/gcc/Standards.html) | Modos de linguagem e padrões |
| [GCC — warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) | Flags de diagnóstico |
| [Clang — C++ status](https://clang.llvm.org/cxx_status.html) | Suporte por recurso e versão |
| [MSVC — opção /std](https://learn.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170) | Seleção de padrões, `_MSVC_LANG` e `__cplusplus` |
| [Microsoft — modelos de dados](https://learn.microsoft.com/en-us/windows/win32/winprog64/abstract-data-models) | ABI e modelo LLP64 do Windows |
| [MSVC — /utf-8](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170) | Charset de código-fonte e execução |
| [CMake — tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) | Targets, configurações e organização do build |
| [CMake — toolchains](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html) | Compilação para outro alvo e sysroot |
| [Clang — AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) | Instrumentação de memória e limitações |
| [Clang — ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) | Diagnóstico de corridas e ambientes suportados |
| [MIT 6.006 — notas de algoritmos](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/pages/lecture-notes/) | Material acadêmico complementar sobre estruturas, grafos e DP |

## Livros enviados: como foram aproveitados

Foi feita **consulta seletiva a sumários e trechos**, não leitura integral dos PDFs. O uso foi orientar cobertura e localizar pontos que precisam de atualização. Nenhum PDF nem extração de texto é incluído no repositório.

| Livro identificado no arquivo | Consulta e utilidade | Como complementar |
|---|---|---|
| André Backes, *Linguagem C: completa e descomplicada*, edição consultada de 2013 | Funções e capítulos 11 (alocação dinâmica) e 12 (arquivos), além do sumário | Conferir portabilidade, overflow, tratamento de erro e diferenças C17/C23 |
| H. M. Deitel e P. J. Deitel, *C++ como programar*, 5ª edição, tradução de 2006 | Sumário e trechos sobre funções/templates, biblioteca e exceções | Acrescentar recursos de C++11 em diante e práticas de ownership moderno |
| Cay Horstmann, *Conceitos de computação com o essencial de C++*, 3ª edição, arquivo eletrônico de 2008 | Organização dos capítulos de testes, ordenação/pesquisa e estruturas; trecho de ordenação | Atualizar containers, cópias, RAII e recursos modernos |

Os demais livros enviados não foram necessários para a primeira versão. Os livros antigos continuam úteis para algoritmos e fundamentos, mas um exemplo com ferramenta antiga ou `system("pause")` precisa ser adaptado antes de ser tratado como recomendação multiplataforma.

## Limites e manutenção

- Este é um guia amplo de consulta, não uma reprodução exaustiva das normas ISO ou de cada biblioteca.
- Fragmentos nos capítulos mostram a operação e seu contrato; os programas completos têm headers e execução em [exemplos](exemplos/README.md).
- A [validação](exemplos/VALIDACAO.md) distingue o que foi executado do que é receita para outro sistema. Não se infere aprovação em MSVC/Apple Clang a partir de GCC.
- Ao adicionar recurso: marcar padrão mínimo, headers, pré-condições, complexidade quando pertinente, exemplo e teste de fronteira.
- Ao corrigir regra de linguagem: consultar a edição correta e registrar a fonte primária; quando houver diferença de comportamento por padrão, não apagar essa distinção.

