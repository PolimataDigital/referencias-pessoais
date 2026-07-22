# Referência — Node/Express Back-End

> Espelha o [referencia-python-backend.md](./referencia-python-backend.md) — mesma lógica, sintaxe diferente. Bom pra revisar os dois lado a lado.

---

## 1. Setup do projeto

```bash
mkdir backend-node
cd backend-node
npm init -y
npm install express
```

- `npm init -y` cria o `package.json` (equivalente ao `requirements.txt`, mas atualizado automaticamente a cada `npm install`)
- `package-lock.json` trava as versões exatas instaladas
- `node_modules/` guarda o código das bibliotecas — **nunca sobe pro GitHub** (adiciona no `.gitignore`, junto do `.env`)

---

## 2. Express — básico

```javascript
const express = require('express');
const app = express();

// middleware: sem isso, req.body vem vazio em POST/PUT/DELETE
app.use(express.urlencoded({ extended: true }));

app.get('/', (req, res) => {
    res.send('Ola mundo');
});

app.listen(8080, () => {
    console.log('Servidor rodando na porta 8080');
});
```

**Rota recebendo dados (POST):**
```javascript
app.post('/cadastro', (req, res) => {
    const nome = req.body.nome;
    const idade = req.body.idade;
    res.send(`Cadastro: ${nome}, ${idade} anos!`);
});
```

Cada método vira uma função diferente: `app.get`, `app.post`, `app.put`, `app.delete` — não existe um `methods=[...]` como no Flask.

**Rodar:** `node index.js` (reinicia manualmente a cada mudança, mesma lógica do `py app.py`)

**Testando com curl (PowerShell sempre com `curl.exe`):**
```bash
curl.exe -X POST http://localhost:8080/cadastro -d "nome=Victor&idade=25"
```

---

## 3. SQL com `node:sqlite` (nativo do Node, sem instalar nada)

> Alternativa ao `better-sqlite3`, que exige compilar C++ e pode falhar no Windows por falta do Windows SDK. `node:sqlite` já vem embutido a partir do Node 22+, sem esse problema — só mostra um aviso de "experimental", que não impede o uso.

**Conectar e criar tabela:**
```javascript
const { DatabaseSync } = require('node:sqlite');
const db = new DatabaseSync('banco.db');

db.exec(`
    CREATE TABLE IF NOT EXISTS usuarios (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        nome TEXT,
        idade INTEGER,
        email TEXT,
        senha TEXT
    )
`);
```

`IF NOT EXISTS` é essencial aqui porque esse código roda **toda vez que o servidor inicia** (diferente do script avulso que você rodava uma vez em Python).

**ALTER TABLE** (sem `IF NOT EXISTS` no SQLite — rodar num script separado, uma vez só, senão dá erro se o servidor reiniciar):
```javascript
// alterar_tabela.js — roda uma vez, separado do index.js
const { DatabaseSync } = require('node:sqlite');
const db = new DatabaseSync('banco.db');
db.exec(`ALTER TABLE usuarios ADD COLUMN senha TEXT`);
```

**INSERT:**
```javascript
const stmt = db.prepare('INSERT INTO usuarios (nome, idade, email) VALUES (?, ?, ?)');
stmt.run(nome, idade, email);
```

**SELECT (várias linhas / uma linha só):**
```javascript
let lista = db.prepare("SELECT * FROM usuarios").all();       // várias linhas → array de objetos
let linha = db.prepare("SELECT * FROM usuarios WHERE nome = ?").get(nome);  // uma linha só → objeto ou undefined
```

**UPDATE:**
```javascript
db.prepare("UPDATE usuarios SET nome = ? WHERE nome = ?").run(nomeNovo, nomeAntigo);
```

**DELETE:**
```javascript
db.prepare("DELETE FROM usuarios WHERE nome = ?").run(nome);
```

**Responder em JSON de verdade:**
```javascript
res.json(lista);  // mais explícito que res.send(lista), mesmo o Express convertendo sozinho
```

⚠️ **Ordem dos `?` importa.** No `UPDATE ... SET x = ? WHERE y = ?`, o primeiro `?` é o **valor novo**, o segundo é o **critério de busca** — fácil inverter sem perceber.

⚠️ **Objeto, não tupla.** `.get(nome)` retorna um **objeto** (`resultado.senha`), diferente do Python que retorna uma tupla por índice (`resultado[0]`) — mais legível aqui.

---

## 4. Variáveis de ambiente (`.env`)

```bash
npm install dotenv
```

```javascript
require('dotenv').config();
const CHAVE_SECRETA = process.env.CHAVE_SECRETA;
```

`.env`:
```
NOME_BANCO=banco.db
CHAVE_SECRETA=uma_frase_dificil_de_adivinhar
```

`process.env.NOME` (propriedade de objeto) em vez de `os.getenv("NOME")` (chamada de função). Mesma regra de nunca subir esse arquivo pro GitHub.

---

## 5. Hash de senha (bcrypt)

```bash
npm install bcrypt
```

```javascript
const bcrypt = require('bcrypt');

// registrar — ASYNC, precisa de await
const senhaHash = await bcrypt.hash(senha, 10);  // 10 = cost factor (quão lento/caro, de propósito)

// login — repara na ordem: senha digitada primeiro, hash salvo depois
const senhaCorreta = await bcrypt.compare(senha, resultado.senha);  // true/false
```

⚠️ **`bcrypt` é assíncrono** (diferente do `werkzeug.security`, que resolve na hora). Toda função que usa `await` precisa ser declarada `async`:
```javascript
app.post('/registrar', async (req, res) => {
    const senhaHash = await bcrypt.hash(senha, 10);
    // ...
});
```
Sem `await`, a variável fica com uma `Promise` pendente, não o resultado de verdade.

---

## 6. JWT (jsonwebtoken)

```bash
npm install jsonwebtoken
```

**Gerar o token (login):**
```javascript
const jwt = require('jsonwebtoken');

const token = jwt.sign({ nome: nome }, CHAVE_SECRETA, { expiresIn: '1h' });
res.send(token);
```

**Proteger uma rota:**
```javascript
app.get('/perfil', (req, res) => {
    const tokenCompleto = req.headers.authorization;  // "Bearer eyJ..."
    const token = tokenCompleto.split(' ')[1];

    try {
        const dados = jwt.verify(token, CHAVE_SECRETA);
        res.send(`Bem-vindo, ${dados.nome}!`);
    } catch (erro) {
        res.send('Token inválido ou expirado!');
    }
});
```

**Testar com curl:**
```bash
curl.exe http://localhost:8080/perfil -H "Authorization: Bearer SEU_TOKEN_AQUI"
```

---

## Tabela comparativa Python (Flask) × Node (Express)

| Conceito | Python / Flask | Node / Express |
|---|---|---|
| Criar app | `app = Flask(__name__)` | `const app = express();` |
| Definir rota | `@app.route("/x")` + função nomeada | `app.get('/x', (req, res) => {...})` |
| Método da rota | `methods=["POST"]` | `app.post(...)` (função própria por verbo) |
| Ler dado de formulário | `request.form.get("nome")` | `req.body.nome` (exige `express.urlencoded`) |
| Ler header | `request.headers.get("Authorization")` | `req.headers.authorization` |
| Responder texto | `return "texto"` | `res.send("texto")` |
| Responder JSON | `return str(lista)` (não é JSON de verdade) | `res.json(lista)` (JSON nativo) |
| Rodar servidor | `app.run(port=8080)` | `app.listen(8080, callback)` |
| Conectar banco | `sqlite3.connect("banco.db")` | `new DatabaseSync('banco.db')` |
| Executar sem retorno | `cursor.execute(sql)` | `db.exec(sql)` |
| Query parametrizada | `cursor.execute(sql, (a, b))` | `db.prepare(sql).run(a, b)` |
| Buscar todas as linhas | `cursor.fetchall()` → lista de tuplas | `.all()` → array de objetos |
| Buscar uma linha | `cursor.fetchone()` → tupla ou `None` | `.get(...)` → objeto ou `undefined` |
| Var. de ambiente | `os.getenv("X")` | `process.env.X` |
| Hash de senha | `generate_password_hash(s)` (síncrono) | `await bcrypt.hash(s, 10)` (assíncrono) |
| Checar senha | `check_password_hash(hash, senha)` | `await bcrypt.compare(senha, hash)` — ordem invertida! |
| Gerar JWT | `jwt.encode(payload, chave, algorithm="HS256")` | `jwt.sign(payload, chave, { expiresIn: '1h' })` |
| Validar JWT | `jwt.decode(token, chave, algorithms=[...])` | `jwt.verify(token, chave)` |
| Tratar erro | `try: / except TipoErro:` | `try { } catch (erro) { }` |
| Dividir string | `.split(" ")` | `.split(' ')` — idêntico |
| Verificar vazio/nulo | `if resultado is None:` | `if (!resultado) {` |

---

## Erros que já apareceram (Node)

- `Cannot POST /rota` — rota existe só pra outro método (ex: definida como `app.get` mas testando com POST), ou servidor não foi reiniciado depois da mudança
- `ReferenceError: X is not defined` — variável usada com nome diferente de como foi declarada (erro de digitação/renomeação incompleta)
- `Error: table X has no column named Y` — nome de coluna errado no SQL (confere digitação exata)
- `ExperimentalWarning: SQLite is an experimental feature` — só aviso, não impede o uso do `node:sqlite`
- Instalação de `better-sqlite3` falhando no Windows (`MSB8036`, Windows SDK não encontrado) — evitado usando `node:sqlite` nativo em vez de instalar esse pacote

---

*Complementa o [referencia-python-backend.md](./referencia-python-backend.md).*
