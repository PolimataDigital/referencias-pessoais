# 11 · Armadilhas e revisão rápida

[Índice](../referencia-c-cpp.md) · Todos os níveis

## Código que merece uma segunda olhada

| Padrão suspeito | Problema | Alternativa/checagem |
|---|---|---|
| `int main` sem retorno de tipo correto / `void main` | Interface hospedada fora da forma padrão | `int main(void)` em C; `int main()` em C++ |
| Ler variável automática não inicializada | Valor indeterminado e possíveis comportamentos inválidos | Inicializar; não depender do conteúdo anterior da memória |
| `i <= n` para vetor de n elementos | Acesso a `v[n]` | `i < n` |
| `size_t i = n - 1` com n zero | Wrap unsigned | Intervalo meio aberto ou laço reverso seguro |
| `a < b < c` | Comparação encadeada não matemática | `a < b && b < c` |
| `a & mascara == 0` | Precedência inesperada | `(a & mascara) == 0` |
| `strcmp(a,b) == 1` | Retorno positivo não precisa ser exatamente 1 | `strcmp(a,b) > 0` |
| `s1 == s2` com `char *` | Compara endereços | `strcmp` com strings válidas |
| `sizeof(p)/sizeof(p[0])` para ponteiro | Não informa quantidade alocada | Carregar tamanho no contrato |
| `malloc(n * sizeof(T))` sem verificar | Overflow de tamanho pode alocar menos | Conferir divisão por `SIZE_MAX` antes |
| Cast de `malloc` em C sem header | Pode ocultar problema em ambientes permissivos | `<stdlib.h>` e sem cast |
| `p = realloc(p, bytes)` | Perde ponteiro no erro | Temporário e tamanho positivo |
| `free(p); usar(p);` | Use-after-free | Dono claro; invalidar empréstimos |
| `new[]` com `delete`, `malloc` com `delete` | Pares incompatíveis | Containers/RAII; API correta de liberação |
| `return &local` | Objeto já terminou sua vida útil | Retornar valor ou posse definida |
| `string_view` de temporário | Empréstimo pendente | Guardar `string` ou garantir origem viva |
| `vector.reserve(n); v[i] = ...` | Capacidade não é tamanho | `resize` ou `push_back` |
| `map[chave]` durante consulta | Insere elemento | `find`, `contains`, `at` |
| Comparador retorna `a-b` | Overflow signed | `(a>b)-(a<b)` em C |
| Comparador C++ usa `<=` | Viola ordenação fraca estrita | Critério estrito e consistente |
| `std::accumulate(..., 0)` para soma grande | Acumulador é `int` | Tipo inicial adequado (`0LL`), limites verificados |
| `memset` para “construir” objeto C++ | Ignora construtores/invariantes | Inicialização/atribuição por tipo |
| `memset(a, 1, sizeof a)` para inteiros 1 | Preenche bytes com 1 | Laço/`std::fill` |
| `gets` / `scanf("%s", buffer)` sem largura | Overflow de buffer | Leitura limitada e política de truncamento |
| `fflush(stdin)` | Não é solução ISO portátil para entrada | Consumir entrada conforme contrato |
| `while (!feof(f))` | Testa estado antes de tentar leitura | Laço controlado pela leitura |
| `printf(texto_externo)` | Formato controlado externamente | `printf("%s", texto_externo)` |
| `tolower(c)` com char negativo | Argumento fora do domínio | Converter a `unsigned char` |
| `i = i++ + 1` / efeitos misturados | Regras de sequenciamento variam | Separar operações, não memorizar truques |
| `volatile` para thread | Não sincroniza memória compartilhada | Mutex/atomic e protocolo |
| `using namespace std;` em header | Polui todos os usuários do header | Qualificar `std::` |
| `#include <bits/stdc++.h>` | Header específico de implementação | Incluir os headers usados |

## Categorias de comportamento

| Categoria | Significado | Exemplo/atenção |
|---|---|---|
| Definido | A regra especifica o resultado/contrato | Wrap unsigned dentro das regras |
| Definido pela implementação | Implementação escolhe e documenta | Signedness de `char` |
| Não especificado | Há alternativas válidas, sem escolha única exigida | Ordem de avaliação de certos argumentos; regras diferem por padrão |
| Indefinido (UB) | Padrão não impõe requisitos à execução | Overflow signed, muitos acessos inválidos |

UB não significa “sempre dá erro” nem “produz aleatório”: pode aparentar funcionar, desaparecer com otimização ou afetar código vizinho. Um teste que passou não torna a operação definida. Também há programas malformados que requerem diagnóstico em compilação; não são a mesma categoria de problema de runtime.

## Checklist C

- [ ] Funções têm protótipos e headers corretos; sem parâmetros usa `(void)` na base C17.
- [ ] Buffers carregam tamanho/capacidade; terminador está contemplado quando necessário.
- [ ] Toda alocação verifica overflow e falha; toda posse tem liberação única.
- [ ] Contratos de ponteiros nulos e sobreposição estão documentados.
- [ ] Funções deixam saídas/estado definidos no erro.
- [ ] Macros não avaliam efeitos colaterais inesperadamente.
- [ ] I/O confere retornos; arquivos de escrita conferem erro final.

## Checklist C++

- [ ] Valores e containers usados quando suficientes; RAII para recursos.
- [ ] `span`, `string_view`, iteradores e referências não sobrevivem aos donos.
- [ ] Rule of Zero como padrão; cópia/move explícitos quando a semântica exige.
- [ ] Classes preservam invariantes; bases polimórficas têm política de destruição correta.
- [ ] `noexcept` corresponde a uma garantia real; erros não são engolidos.
- [ ] Comparadores são consistentes; algoritmos recebem intervalos válidos.
- [ ] Recursos modernos têm padrão mínimo e suporte de biblioteca verificado.

## Checklist de projeto

- [ ] Build reproduzível, padrão explícito, versão de toolchain registrada.
- [ ] Avisos, testes de fronteira e sanitizers relevantes executados.
- [ ] Testes não dependem de `assert` desativável para realmente verificar o resultado.
- [ ] Windows/Linux/macOS e arquiteturas exigidas testados de fato.
- [ ] Sem caminhos pessoais, senhas, binários temporários ou dependências locais ocultas.
- [ ] Formato de arquivo/rede e API têm tamanhos, versão e ownership definidos.
- [ ] Otimizações justificadas por perfil; limitações e pré-condições permanecem visíveis.

