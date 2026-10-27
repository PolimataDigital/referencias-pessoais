# 08 · Plataformas, compilação e distribuição

[Índice](../referencia-c-cpp.md) · Intermediário a avançado

## A cadeia de ferramentas

| Etapa | Entrada → saída | Erro típico |
|---|---|---|
| Pré-processamento | Includes/macros → unidade de tradução | Header não encontrado |
| Compilação | Código → assembly/objeto | Tipo incompatível, sintaxe, recurso ausente |
| Linkedição | Objetos + bibliotecas → executável/biblioteca | Símbolo indefinido ou definição múltipla |
| Carregamento | Executável + bibliotecas dinâmicas → processo | DLL/`.so` ausente ou incompatível |

O editor (VS Code/Neovim) não é o compilador. CMake configura o build; Ninja/Make/MSBuild executam etapas. `clangd` analisa o projeto para o editor e se beneficia de `compile_commands.json` com flags reais.

## Comandos mínimos

Exemplos abaixo assumem terminal na raiz deste repositório. Arquivos de saída sem extensão `.exe` são usados nos exemplos Unix. Compiladores precisam estar instalados e no PATH.

```bash
# Linux: GCC; C17 e C++20 explícitos
gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -g -O0 c-cpp/exemplos/c/basico.c -o c_basico
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -g -O0 c-cpp/exemplos/cpp/sequencias.cpp -o cpp_sequencias
./c_basico
./cpp_sequencias

# Clang (Linux/macOS): mesmas fontes
clang -std=c17 -Wall -Wextra -Wpedantic -g -O0 c-cpp/exemplos/c/basico.c -o c_basico
clang++ -std=c++20 -Wall -Wextra -Wpedantic -g -O0 c-cpp/exemplos/cpp/sequencias.cpp -o cpp_sequencias
```

No **Developer PowerShell/Prompt** do Visual Studio com ferramentas C++ instaladas:

```powershell
cl /nologo /TC /std:c17 /W4 /utf-8 /Zi c-cpp/exemplos/c/basico.c /Fe:c_basico.exe
cl /nologo /TP /std:c++20 /permissive- /Zc:__cplusplus /W4 /EHsc /utf-8 /Zi c-cpp/exemplos/cpp/sequencias.cpp /Fe:cpp_sequencias.exe
.\c_basico.exe
.\cpp_sequencias.exe
```

`/TC` força C; `/TP` força C++. `/EHsc` configura tratamento de exceções C++; `/Zc:__cplusplus` atualiza a macro no MSVC. Os comandos são receitas para o ambiente indicado; a [validação executada](exemplos/VALIDACAO.md) informa os ambientes realmente disponíveis.

## Ambiente por sistema

| Sistema | Opções | Atenção |
|---|---|---|
| Windows nativo | MSVC; LLVM/Clang com toolchain configurado; MinGW-w64 via MSYS2 | Não misturar bibliotecas/CRTs/ABIs incompatíveis |
| WSL | Toolchain Linux dentro do WSL | Produz binários Linux; não equivale a testar aplicativo Windows |
| Linux | GCC ou Clang + libc/biblioteca C++ | Distribuição/versão da libc influencia distribuição binária |
| macOS | Apple Clang + Command Line Tools/Xcode | Apple Clang não tem a mesma numeração do Clang upstream |
| ARM/embarcado | Toolchain para o alvo + SDK/sysroot | Sistema, ABI, FPU, biblioteca e recursos disponíveis variam |

GCC e Clang podem usar bibliotecas C++ diferentes. `-std=c++23` habilita o modo da linguagem, mas não instala `std::print` numa biblioteca antiga. Não fixe “suporta tudo” apenas pelo nome da versão.

## CMake no próprio material

O [CMakeLists.txt](exemplos/CMakeLists.txt) compila C como C17 e C++ como C++20 e registra os programas no CTest.

```bash
cmake -S c-cpp/exemplos -B build/c-cpp -DCMAKE_BUILD_TYPE=Debug
cmake --build build/c-cpp --config Debug
ctest --test-dir build/c-cpp -C Debug --output-on-failure
```

`CMAKE_BUILD_TYPE` vale para geradores de configuração única (Makefiles/Ninja). Visual Studio e outros geradores de múltiplas configurações selecionam com `--config`; CTest recebe `-C`.

```bash
# Outra pasta para outro compilador; não reutilizar cache misturado.
cmake -S c-cpp/exemplos -B build/clang -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Exportação de `compile_commands.json` depende do gerador (como Makefiles/Ninja). Configure o editor para a pasta correta. CMake por target usa `target_include_directories`, `target_link_libraries` e `target_compile_features/options`; evite flags globais que afetem bibliotecas de terceiros sem necessidade.

## Portabilidade de fonte, de comportamento e de binário

| Problema | Conduta |
|---|---|
| `long` muda de largura | Use tipos/limites adequados ao contrato; não armazene ponteiro em `long` |
| Endianness | Serialize campo por campo com ordem de bytes definida |
| Padding/alinhamento | Não grave structs brutas como formato portátil |
| Signedness de `char` | Use `unsigned char` para valores de bytes e funções `ctype` |
| Caminhos | `std::filesystem::path`; não montar tudo com `"C:\\..."` |
| Caixa em nomes de arquivo | Inclua o nome exato; diferenças podem passar no Windows e falhar no Linux |
| Codificação | Defina UTF-8 para dados/texto do projeto; converta nas fronteiras necessárias |
| Quebra de linha | Texto pode traduzir CRLF/LF; binário preserva bytes |
| Locale | Números/datas podem mudar; protocolo precisa de formato explícito |
| Headers não padrão | Evite `<bits/stdc++.h>`, `<conio.h>`, `getch`, `system("pause")` como base portátil |
| VLA | Não existe em C++ padrão; em C17 suporte pode ser opcional |
| `M_PI`, `strdup`, `getline` C | Não são todos ISO C17; POSIX/extensão ou padrão posterior precisam de rótulo |

`/utf-8` configura codificação do código-fonte/literais no MSVC; não configura sozinho o terminal, a fonte ou toda API Windows. APIs Windows `W` usam UTF-16; `std::filesystem::path` usa representação nativa. `wchar_t` não tem largura universal (Windows e Unix usuais diferem). Teste nomes com espaços e acentos.

Em C++20, um literal `u8"texto"` usa `char8_t`, não `char`; não o passe diretamente a uma API que exige `const char*`. Defina a conversão/representação na fronteira. Em C++20, `<numbers>` oferece constantes como `std::numbers::pi`, evitando depender da macro não padronizada `M_PI`.

## Interoperabilidade e bibliotecas

- Compile `.c` com compilador C, `.cpp` com C++, e use o driver C++ para link final quando houver runtime C++.
- Biblioteca estática é incorporada ao link; dinâmica é carregada em execução. `.a/.lib` e `.so/.dylib/.dll` dependem do sistema/toolchain.
- ABI inclui layout, convenção de chamada, nomes de símbolos, exceções e runtime. Mesmo fonte portátil pode produzir binários incompatíveis.
- MinGW e MSVC não são intercambiáveis para qualquer biblioteca C++. Compartilhar objetos STL entre binários exige combinação compatível.
- CMake ajuda a expressar dependências, mas não é gerenciador de pacotes. vcpkg/Conan são opções quando necessários; fixe versões, configuração e origem num projeto real.
- Em linha de comando GCC/Unix, ordem de bibliotecas estáticas pode importar. Para certas rotinas matemáticas C, use `-lm` depois dos objetos conforme plataforma.

## Cross-compilation e distribuição

Compilar para outro alvo exige compilador/target, sysroot, bibliotecas e ferramentas compatíveis. Passar `-m32` ou trocar uma macro não basta quando faltam dependências do alvo. Use arquivo de toolchain CMake (`CMAKE_SYSTEM_NAME`, compiladores, sysroot quando necessário) e pasta de build separada.

Não execute binário ARM/x86/Windows/Linux arbitrariamente no host: teste no alvo ou em emulação apropriada. Testes de configuração que tentam rodar programas podem exigir configuração especial em cross-compilation.

Distribua com uma arquitetura e versão mínima de sistema documentadas. `-march=native` otimiza para a máquina de build e pode gerar instrução ilegal em outra máquina. Preserve símbolos de debug em artefato apropriado para diagnóstico, respeitando privacidade. Assinatura, instaladores e empacotamento são específicos do sistema.

Para validar portabilidade: builds separados em Windows/MSVC, Linux/GCC, Linux/Clang e macOS/Apple Clang, mais arquiteturas exigidas pelo produto. Passar numa máquina não comprova suporte nas demais.

