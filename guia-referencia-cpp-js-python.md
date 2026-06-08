# Guia de Referência — C++ vs JavaScript vs Python

---

## Entrada de dados

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Ler número | `cin >> numero` | `Number(readline.question("msg"))` | `int(input("msg"))` |
| Ler string | `getline(cin, str)` | `readline.question("msg")` | `input("msg")` |
| Ler char | `cin >> caractere` | — | `input("msg")` |
| Limpar buffer | `cin.ignore(numeric_limits<streamsize>::max(), '\n')` | — | — |
| Limpar erro do cin | `cin.clear()` | — | — |
| Validar número | `if(!(cin >> numero))` | `isNaN(entrada)` | `try: int(entrada) except ValueError` |
| Saída no terminal | `cout << valor << "\n"` | `console.log(valor)` | `print(valor)` |
| Saída formatada | `cout << fixed << setprecision(2) << valor` | `valor.toFixed(2)` | `f"{valor:.2f}"` |

---

## Tipos de dados

| Tipo | C++ | JavaScript | Python |
|---|---|---|---|
| Inteiro | `int` / `long long` | `Number` | `int` |
| Decimal | `double` | `Number` | `float` |
| Texto | `string` | `string` | `str` |
| Caractere | `char` | — | — |
| Booleano | `bool` (true/false) | `boolean` (true/false) | `bool` (True/False) |
| Sem valor | — | `null` | `None` |
| Inteiro grande | `unsigned long long` | `BigInt` | `int` (ilimitado) |

---

## Declaração de variáveis

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Variável mutável | `int x = 0;` | `let x = 0;` | `x = 0` |
| Constante | `const int X = 0;` | `const X = 0;` | — |
| Sem tipo definido | `auto x = valor;` | `let x = valor;` | `x = valor` |

---

## Strings

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Minúsculo (char) | `tolower(c)` | — | — |
| Minúsculo (string) | `transform(s.begin(), s.end(), s.begin(), [](unsigned char c)` | `str.toLowerCase()` | `str.lower()` |
| Remover espaços | — | `str.trim()` | `str.strip()` |
| Tamanho | `str.size()` ou `str.length()` | `str.length` | `len(str)` |
| Verificar se é letra | `isalpha(c)` | — | `str.isalpha()` |
| Verificar se é número | `isdigit(c)` | `!isNaN(str)` | `str.isdigit()` |
| Concatenar com número | `"texto" + to_string(n)` | `` `texto${n}` `` | `f"texto{n}"` |
| Juntar lista em string | — | `arr.join(' ')` | `' '.join(lista)` |

---

## Vetores / Arrays / Listas

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Declarar vazio | `vector<int> v;` | `const arr = [];` | `lista = []` |
| Declarar com tamanho | `vector<int> v(n, 0);` | — | `lista = [0] * n` |
| Adicionar elemento | `v.push_back(x);` | `arr.push(x)` | `lista.append(x)` |
| Tamanho | `v.size()` | `arr.length` | `len(lista)` |
| Acessar por índice | `v[i]` | `arr[i]` | `lista[i]` |
| Verificar se contém | `find(v.begin(), v.end(), x) != v.end()` | `arr.includes(x)` | `x in lista` |
| Iterar com índice | `for(int i=0; i<v.size(); i++)` | `for(let i=0; i<arr.length; i++)` | `for i in range(len(lista))` |
| Iterar sem índice | `for(auto& item : v)` | `for(const item of arr)` | `for item in lista` |
| Ordenar crescente | `sort(v.begin(), v.end())` | `arr.sort((a,b) => a-b)` | `lista.sort()` |
| Ordenar decrescente | `sort(v.begin(), v.end(), greater<int>())` | `arr.sort((a,b) => b-a)` | `lista.sort(reverse=True)` |
| Ordenar strings desc | `sort(v.begin(), v.end(), greater<string>())` | `arr.sort((a,b) => b.localeCompare(a))` | `lista.sort(reverse=True)` |
| Fatiar (slice) | `vector<int>(v.begin()+1, v.end())` | `arr.slice(1)` | `lista[1:]` |
| Juntar dois vetores | `v.insert(v.end(), v2.begin(), v2.end())` | `arr.concat(arr2)` | `lista + lista2` |
| Vazio? | `v.empty()` | `arr.length === 0` | `lista == []` |

---

## Sets (conjuntos)

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Declarar | `set<int> s;` | `const s = new Set();` | `s = set()` |
| Adicionar | `s.insert(x);` | `s.add(x)` | `s.add(x)` |
| Verificar se contém | `s.count(x) > 0` | `s.has(x)` | `x in s` |
| Remover | `s.erase(x);` | `s.delete(x)` | `s.remove(x)` |
| Vazio? | `s.empty()` | `s.size === 0` | `len(s) == 0` |

---

## Mapas / Dicionários

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Declarar | `map<string, int> m;` | `const m = {};` | `m = {}` |
| Adicionar/atualizar | `m["chave"] = valor;` | `m["chave"] = valor` | `m["chave"] = valor` |
| Acessar | `m["chave"]` | `m["chave"]` | `m["chave"]` |
| Verificar se existe | `m.count("chave") > 0` | `"chave" in m` | `"chave" in m` |
| Iterar | `for(auto& par : m)` → `par.first`, `par.second` | `for(const [k,v] of Object.entries(m))` | `for k, v in m.items()` |

---

## Funções

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Declarar | `int funcao(int a) { return a; }` | `function funcao(a) { return a; }` | `def funcao(a): return a` |
| Passar por referência | `void f(int& x)` | — (automático para objetos) | — (automático para listas/dicts) |
| Passar por valor | `void f(int x)` | `function f(x)` | `def f(x)` |
| Retornar múltiplos | `struct` ou `pair` | `return { a, b }` | `return a, b` |
| Receber múltiplos | `auto [a, b] = funcao()` | `const { a, b } = funcao()` | `a, b = funcao()` |

---

## Condicionais e loops

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| if/else if/else | `if(...) {} else if(...) {} else {}` | `if(...) {} else if(...) {} else {}` | `if ...: elif ...: else:` |
| E lógico | `&&` | `&&` | `and` |
| OU lógico | `\|\|` | `\|\|` | `or` |
| NÃO lógico | `!` | `!` | `not` |
| while | `while(cond) {}` | `while(cond) {}` | `while cond:` |
| loop infinito | `while(true) {}` | `while(true) {}` | `while True:` |
| break | `break;` | `break;` | `break` |
| continue | `continue;` | `continue;` | `continue` |

---

## Números aleatórios

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Gerar inteiro no intervalo | `mt19937 g(seed); uniform_int_distribution<int> d(min,max); d(g)` | `Math.floor(Math.random() * (max-min+1)) + min` | `random.randint(min, max)` |
| Semente baseada no tempo | `chrono::steady_clock::now().time_since_epoch().count()` | — | — |

---

## Medição de tempo

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| Início | `auto t = chrono::high_resolution_clock::now()` | `const t = performance.now()` | `import time; t = time.time()` |
| Duração em ms | `chrono::duration_cast<chrono::milliseconds>(fim-inicio).count()` | `(fim - inicio).toFixed(3)` | `(fim - inicio) * 1000` |

---

## Conversões de tipo

| Operação | C++ | JavaScript | Python |
|---|---|---|---|
| String para int | `stoi(str)` | `Number(str)` / `parseInt(str)` | `int(str)` |
| String para double | `stod(str)` | `Number(str)` / `parseFloat(str)` | `float(str)` |
| Int para string | `to_string(n)` | `String(n)` / `` `${n}` `` | `str(n)` |
| Forçar tipo (cast) | `(int)valor` | — | `int(valor)` |

---

## Estruturas de dados avançadas

| Estrutura | C++ | JavaScript | Python |
|---|---|---|---|
| Struct/objeto | `struct Nome { int x; double y; };` | `const obj = { x: 0, y: 0 }` | `dict` ou `class` |
| Acessar campo | `obj.campo` | `obj.campo` | `obj["campo"]` ou `obj.campo` |
| Par de valores | `pair<int,int> p = {1, 2}; p.first; p.second` | `[1, 2]` | `(1, 2)` |

---

## Algoritmos importantes

### Bubble Sort
```cpp
// C++
for(int i=0; i<n; i++)
    for(int j=0; j<n-1; j++)
        if(v[j] > v[j+1]) swap(v[j], v[j+1]);
```
```javascript
// JS
for(let i=0; i<n; i++)
    for(let j=0; j<n-1; j++)
        if(v[j] > v[j+1]) [v[j], v[j+1]] = [v[j+1], v[j]];
```
```python
# Python
for i in range(n):
    for j in range(n-1):
        if v[j] > v[j+1]: v[j], v[j+1] = v[j+1], v[j]
```

### Busca Binária
```cpp
// C++
int inicio=0, fim=n-1, meio;
while(inicio <= fim) {
    meio = inicio + (fim-inicio)/2;
    if(v[meio] == x) break;
    else if(x < v[meio]) fim = meio-1;
    else inicio = meio+1;
}
```
```javascript
// JS
let inicio=0, fim=n-1, meio;
while(inicio <= fim) {
    meio = Math.floor(inicio + (fim-inicio)/2);
    if(v[meio] === x) break;
    else if(x < v[meio]) fim = meio-1;
    else inicio = meio+1;
}
```
```python
# Python
inicio, fim = 0, n-1
while inicio <= fim:
    meio = inicio + (fim-inicio)//2
    if v[meio] == x: break
    elif x < v[meio]: fim = meio-1
    else: inicio = meio+1
```

---

## Armadilhas comuns

| Problema | C++ | JavaScript | Python |
|---|---|---|---|
| Overflow de índice | `inicio + (fim-inicio)/2` em vez de `(inicio+fim)/2` | `Math.floor(inicio + (fim-inicio)/2)` | `inicio + (fim-inicio)//2` |
| Índice decimal na busca binária | divisão inteira automática | `Math.floor(...)` obrigatório | `//` para divisão inteira |
| Comparar tipos diferentes | `(int)v.size()` para comparar com `int` | usar `===` em vez de `==` | — |
| Incrementar | `x++` ou `x += 1` | `x++` ou `x += 1` | `x += 1` (sem `++`) |
| String vazia | `str.empty()` | `str.trim() === ""` | `str.strip() == ""` |
