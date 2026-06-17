# Caderno de Referências — Desenvolvimento Web (HTML, CSS, JS, Python)

---

## 1. HTML

### Estrutura básica

```html
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Título da página</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <script src="script.js"></script>
</body>
</html>
```

- `<head>`: metadados, título, links para CSS, fontes etc. Nada visível na página.
- `<body>`: conteúdo visível.
- O `<script>` geralmente vai no final do `<body>` (ou usa `defer`) pra não bloquear o carregamento da página.

### Tags semânticas (estrutura de página)

```html
<header>...</header>     <!-- topo, geralmente logo e navegação -->
<nav>...</nav>           <!-- menu de navegação -->
<main>...</main>         <!-- conteúdo principal, único por página -->
<section>...</section>   <!-- bloco temático de conteúdo -->
<article>...</article>   <!-- conteúdo independente (post, notícia) -->
<aside>...</aside>       <!-- conteúdo lateral, complementar -->
<footer>...</footer>     <!-- rodapé -->
```

Usar essas em vez de `<div>` genérico ajuda acessibilidade (leitores de tela) e SEO.

### Texto e estrutura de conteúdo

```html
<h1>Título principal</h1>   <!-- só um por página, idealmente -->
<h2>Subtítulo</h2>
<p>Parágrafo de texto.</p>
<a href="https://exemplo.com">Link</a>
<a href="https://exemplo.com" target="_blank">Abre em nova aba</a>
<img src="foto.jpg" alt="Descrição da imagem">
<ul>                          <!-- lista não ordenada -->
    <li>Item</li>
</ul>
<ol>                          <!-- lista ordenada -->
    <li>Primeiro</li>
</ol>
<br>                          <!-- quebra de linha -->
<hr>                          <!-- linha horizontal -->
<strong>negrito (semântico)</strong>
<em>itálico (semântico)</em>
```

`alt` na imagem não é opcional — é usado por leitores de tela e aparece se a imagem não carregar.

### Divs, spans e atributos genéricos

```html
<div class="card">           <!-- bloco genérico -->
    <span class="destaque">texto inline</span>
</div>
```

```html
<div id="unico" class="card destaque" data-user-id="42"></div>
```

- `id`: único na página, usado para CSS específico ou para pegar via JS (`getElementById`).
- `class`: reutilizável, várias tags podem compartilhar.
- `data-*`: atributos customizados, acessíveis via JS (`element.dataset.userId`).

### Formulários

```html
<form action="/enviar" method="POST">
    <label for="nome">Nome:</label>
    <input type="text" id="nome" name="nome" required placeholder="Digite seu nome">

    <input type="email" name="email" required>
    <input type="password" name="senha">
    <input type="number" name="idade" min="0" max="120">
    <input type="checkbox" name="aceite" checked>
    <input type="radio" name="opcao" value="a"> A
    <input type="radio" name="opcao" value="b"> B

    <select name="cidade">
        <option value="sp">São Paulo</option>
        <option value="rj">Rio de Janeiro</option>
    </select>

    <textarea name="mensagem" rows="4"></textarea>

    <button type="submit">Enviar</button>
</form>
```

- `method="GET"` manda dados na URL (visível, usado para buscas/filtros).
- `method="POST"` manda dados no corpo da requisição (usado para criar/alterar dados, login etc).
- `name` é o que define a chave do dado enviado ao servidor — sem `name`, o campo não é enviado.

---

## 2. CSS

### Como aplicar CSS

```html
<!-- Externo (recomendado) -->
<link rel="stylesheet" href="style.css">

<!-- Interno -->
<style> body { margin: 0; } </style>

<!-- Inline (evitar, difícil de manter) -->
<p style="color: red;">Texto</p>
```

### Seletores

```css
* { }                     /* todos os elementos */
p { }                     /* todas as tags <p> */
.classe { }                /* todos com class="classe" */
#id { }                    /* o elemento com esse id */
div p { }                  /* todo <p> dentro de <div> (descendente, qualquer nível) */
div > p { }                /* <p> filho DIRETO de <div> */
a:hover { }                 /* enquanto o mouse passa por cima */
input:focus { }              /* enquanto o input está selecionado */
li:first-child { }           /* primeiro <li> de uma lista */
p::before { content: "→ "; } /* insere conteúdo antes do elemento */
```

**Especificidade** (o que "ganha" quando há conflito): inline > `id` > `class`/atributo > tag. Em caso de empate, a regra escrita depois no arquivo vence.

### Box model

Todo elemento HTML é uma caixa composta por:

```
margin (espaço fora da caixa, transparente)
  border (borda)
    padding (espaço dentro da caixa, antes do conteúdo)
      content (o conteúdo real)
```

```css
.caixa {
    width: 200px;
    height: 100px;
    padding: 16px;
    border: 1px solid black;
    margin: 8px;
    box-sizing: border-box;  /* width/height passam a incluir padding e border */
}
```

`box-sizing: border-box` é quase sempre o que você quer — sem ele, padding e border somam ao width definido, o que é contraintuitivo.

### Display e posicionamento

```css
div { display: block; }        /* ocupa a linha toda (padrão de div, p, h1...) */
span { display: inline; }      /* só o espaço do conteúdo (padrão de span, a, strong...) */
div { display: inline-block; } /* comportamento de inline, mas aceita width/height */
div { display: none; }         /* remove do layout, como se não existisse */
div { display: flex; }         /* ativa Flexbox nos filhos */
div { display: grid; }         /* ativa Grid nos filhos */
```

```css
.elemento {
    position: relative;   /* serve de referência para filhos com position: absolute */
}
.elemento-filho {
    position: absolute;   /* posicionado em relação ao ancestral com position relative */
    top: 0;
    right: 0;
}
.fixo {
    position: fixed;      /* fica fixo na tela, ignora o scroll */
}
```

### Flexbox (layout em uma dimensão — linha ou coluna)

```css
.container {
    display: flex;
    flex-direction: row;        /* row (padrão) ou column */
    justify-content: center;    /* alinhamento no eixo principal: flex-start, center, space-between, space-around */
    align-items: center;        /* alinhamento no eixo cruzado: flex-start, center, stretch */
    gap: 16px;                  /* espaço entre os itens */
    flex-wrap: wrap;            /* permite quebrar linha quando não cabe */
}
.item {
    flex: 1;                    /* cresce para ocupar espaço disponível, dividido entre os itens com flex:1 */
}
```

Flexbox é o que você usa pra: centralizar coisas, distribuir itens em uma barra de navegação, alinhar elementos em linha ou coluna.

### Grid (layout em duas dimensões)

```css
.container {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;  /* 3 colunas iguais */
    grid-template-columns: 200px 1fr;    /* coluna fixa + coluna que ocupa o resto */
    gap: 16px;
}
.item-grande {
    grid-column: span 2;   /* ocupa 2 colunas */
}
```

Grid é melhor quando você precisa controlar linhas E colunas ao mesmo tempo (layout de página inteira, por exemplo).

### Unidades comuns

```css
px      /* pixels, valor fixo */
%       /* relativo ao elemento pai */
em      /* relativo ao font-size do elemento atual */
rem     /* relativo ao font-size da raiz (html), geralmente 16px — preferível ao em para evitar efeito cascata */
vw, vh  /* % da largura/altura da viewport (tela visível) */
```

### Responsividade (media queries)

```css
/* Estilo padrão pensado para mobile */
.container { flex-direction: column; }

/* A partir de 768px de largura, muda o layout */
@media (min-width: 768px) {
    .container { flex-direction: row; }
}
```

Convenção comum: escrever o CSS "mobile-first" (estilo padrão pensado pra tela pequena, e media queries para alargar conforme a tela cresce).

### Cores e fontes

```css
.elemento {
    color: #333333;            /* cor do texto */
    background-color: #f0f0f0; /* cor de fundo */
    color: rgb(51, 51, 51);
    color: rgba(51, 51, 51, 0.5); /* com transparência */
    font-family: 'Arial', sans-serif;
    font-size: 16px;
    font-weight: bold;          /* ou: 400, 700 etc */
    text-align: center;
}
```

### Transições simples

```css
.botao {
    background-color: blue;
    transition: background-color 0.3s ease;
}
.botao:hover {
    background-color: darkblue;
}
```

---

## 3. JavaScript para Web (além da sintaxe básica)

### Selecionando elementos do DOM

```javascript
document.getElementById('meu-id');
document.querySelector('.minha-classe');       // primeiro elemento que casa
document.querySelectorAll('.minha-classe');    // todos (retorna NodeList)
document.querySelector('div > p');             // aceita seletores CSS completos
```

### Manipulando elementos

```javascript
const el = document.querySelector('#titulo');

el.textContent = 'Novo texto';        // muda o texto (seguro, escapa HTML)
el.innerHTML = '<b>Negrito</b>';      // muda o HTML interno (cuidado com XSS se vier de input do usuário)
el.style.color = 'red';               // muda estilo inline
el.classList.add('ativo');            // adiciona uma classe
el.classList.remove('ativo');         // remove uma classe
el.classList.toggle('ativo');         // alterna (liga/desliga)
el.setAttribute('data-id', '42');     // define um atributo
el.getAttribute('data-id');           // lê um atributo
```

### Criando e inserindo elementos

```javascript
const novoEl = document.createElement('div');
novoEl.textContent = 'Criado via JS';
document.body.appendChild(novoEl);          // insere no final
document.querySelector('#lista').prepend(novoEl); // insere no início
novoEl.remove();                             // remove do DOM
```

### Eventos

```javascript
const botao = document.querySelector('#botao');

botao.addEventListener('click', function(event) {
    console.log('Clicado!');
    event.preventDefault();   // impede comportamento padrão (ex: form recarregar a página)
});

// Em formulários
document.querySelector('form').addEventListener('submit', (event) => {
    event.preventDefault();
    const dados = new FormData(event.target);
    console.log(dados.get('nome'));
});

// Outros eventos comuns
'input'    // a cada tecla digitada em um campo
'change'   // quando o valor muda e perde o foco
'keydown'  // tecla pressionada
'load'     // página/imagem terminou de carregar
```

### Fetch (requisições HTTP / consumir APIs)

```javascript
// GET simples
fetch('https://api.exemplo.com/usuarios')
    .then(response => response.json())
    .then(dados => console.log(dados))
    .catch(erro => console.error('Erro:', erro));

// Com async/await (mais legível, equivalente ao acima)
async function buscarUsuarios() {
    try {
        const response = await fetch('https://api.exemplo.com/usuarios');
        if (!response.ok) throw new Error(`Status ${response.status}`);
        const dados = await response.json();
        console.log(dados);
    } catch (erro) {
        console.error('Erro:', erro);
    }
}

// POST enviando JSON
async function criarUsuario(usuario) {
    const response = await fetch('https://api.exemplo.com/usuarios', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(usuario)
    });
    return await response.json();
}
```

### JSON

```javascript
const objeto = { nome: 'Victor', idade: 21 };

const texto = JSON.stringify(objeto);   // objeto → string JSON
const devolta = JSON.parse(texto);      // string JSON → objeto
```

### localStorage (dados persistentes no navegador)

```javascript
localStorage.setItem('tema', 'escuro');
localStorage.getItem('tema');     // 'escuro'
localStorage.removeItem('tema');
localStorage.clear();             // limpa tudo

// Só aceita strings — para objetos, combine com JSON
localStorage.setItem('usuario', JSON.stringify({ nome: 'Victor' }));
const usuario = JSON.parse(localStorage.getItem('usuario'));
```

### Módulos (organizando código em arquivos separados)

```html
<script type="module" src="main.js"></script>
```

```javascript
// arquivo: utils.js
export function soma(a, b) { return a + b; }
export const PI = 3.14;

// arquivo: main.js
import { soma, PI } from './utils.js';
```

---

## 4. Python para Web (visão geral)

Python não tem um "jeito único" de fazer web — você normalmente usa um **framework**. Os mais comuns:

| Framework | Perfil |
|---|---|
| **Flask** | Minimalista, você monta as peças. Bom para aprender os conceitos e projetos pequenos/médios. |
| **Django** | "Tudo incluso" — ORM, autenticação, admin, etc. Bom para projetos maiores, mais convenção e estrutura. |
| **FastAPI** | Focado em APIs, moderno, usa tipagem do Python para validar dados automaticamente. Muito usado quando o front-end é separado (React, Vue etc). |

### Ambiente virtual e dependências

Sempre isole as dependências do projeto:

```bash
python -m venv venv              # cria o ambiente virtual
source venv/bin/activate         # ativa (Linux/Mac)
venv\Scripts\activate            # ativa (Windows)

pip install flask                # instala dentro do ambiente ativo
pip freeze > requirements.txt    # salva as dependências do projeto
pip install -r requirements.txt  # instala a partir do arquivo (em outra máquina, por exemplo)
```

### Flask — exemplo mínimo

```python
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.html')   # busca em uma pasta templates/

@app.route('/sobre')
def sobre():
    return '<h1>Página sobre</h1>'          # também pode retornar HTML direto

@app.route('/usuario/<nome>')               # parâmetro na URL
def usuario(nome):
    return f'Olá, {nome}!'

@app.route('/api/dados', methods=['GET', 'POST'])
def api_dados():
    if request.method == 'POST':
        dados = request.get_json()          # lê JSON enviado pelo cliente
        return jsonify({'recebido': dados}), 201
    return jsonify({'mensagem': 'use POST para enviar dados'})

if __name__ == '__main__':
    app.run(debug=True)   # debug=True reinicia o servidor automaticamente ao salvar
```

Estrutura de pastas esperada pelo Flask:

```
projeto/
├── app.py
├── templates/        ← arquivos .html (Flask usa Jinja2 para templates dinâmicos)
│   └── index.html
└── static/           ← CSS, JS, imagens
    ├── style.css
    └── script.js
```

### Jinja2 (templates dinâmicos do Flask)

```html
<!-- templates/index.html -->
<h1>Olá, {{ nome }}!</h1>

{% if usuarios %}
    <ul>
    {% for usuario in usuarios %}
        <li>{{ usuario.nome }}</li>
    {% endfor %}
    </ul>
{% else %}
    <p>Nenhum usuário encontrado.</p>
{% endif %}
```

```python
@app.route('/')
def home():
    usuarios = [{'nome': 'Ana'}, {'nome': 'Bruno'}]
    return render_template('index.html', nome='Victor', usuarios=usuarios)
```

### Conceitos que valem entender independente do framework

- **Rota (route)**: associação entre uma URL e uma função que responde a ela.
- **Request/Response**: o ciclo de toda requisição web — o cliente manda uma *request* (com método, headers, corpo), o servidor devolve uma *response* (status code, headers, corpo).
- **Status codes** comuns: `200` OK, `201` Criado, `400` Requisição inválida, `401` Não autenticado, `404` Não encontrado, `500` Erro no servidor.
- **REST**: convenção onde cada URL representa um recurso, e o método HTTP (`GET`, `POST`, `PUT`, `DELETE`) representa a ação sobre ele (ex: `GET /usuarios` lista, `POST /usuarios` cria, `DELETE /usuarios/3` remove o usuário 3).
- **CORS**: mecanismo de segurança do navegador que bloqueia requisições entre domínios diferentes por padrão — relevante quando o front-end e o back-end rodam em portas/domínios separados.

---

## Notas finais

- HTML define **estrutura**, CSS define **aparência**, JS define **comportamento**. Separar essas responsabilidades (não misturar estilo inline ou lógica de JS dentro do HTML) facilita manutenção.
- Para depurar no navegador: F12 abre o DevTools — aba *Elements* mostra o DOM e CSS aplicado, aba *Console* mostra `console.log` e erros de JS, aba *Network* mostra as requisições (`fetch`) feitas.
- Esse documento é um ponto de partida — conforme for fazendo projetos, vale ir complementando com o que aprender na prática.
