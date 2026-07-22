# Referência — Python Back-End (Sockets → Flask → SQL → Segurança)
O guia do Node, para comparar lado a lado:  https://github.com/PolimataDigital/referencias-pessoais/blob/main/referencia-node-backend.md


## 1. Sockets puros (o "por baixo" do HTTP)

Servidor mínimo, sem framework, aceitando múltiplas conexões e respondendo por rota:

```python
import socket

meu_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
meu_socket.bind(("localhost", 8080))
meu_socket.listen()

while True:
    conexao, endereco = meu_socket.accept()
    requisicao = conexao.recv(1024)
    requisicao_texto = requisicao.decode()

    primeira_linha = requisicao_texto.split("\r\n")[0]
    partes = primeira_linha.split(" ")  # ["GET", "/", "HTTP/1.1"]

    if partes[0] != "GET":
        status_line = "HTTP/1.1 405 Method Not Allowed\r\n"
        corpo = "Método não permitido\r\n"
    elif partes[1] == "/":
        corpo = "<h1>Ola mundo</h1>\r\n"
    else:
        corpo = "não encontrado!\r\n"
        status_line = "HTTP/1.1 404 Not Found\r\n"

    corpo_bytes = corpo.encode()
    headers = (
        "Content-Type: text/html; charset=utf-8\r\n"
        + "Connection: close\r\n"
        + f"Content-Length: {len(corpo_bytes)}\r\n"
    )
    linha_vazia = "\r\n"

    resposta = status_line + headers + linha_vazia + corpo
    conexao.send(resposta.encode())
    conexao.close()
```

**Formato de uma resposta HTTP crua:**
```
HTTP/1.1 200 OK\r\n
Content-Type: text/html; charset=utf-8\r\n
Connection: close\r\n
\r\n
<corpo aqui>
```

**Pontos-chave:**
- `bind()` reserva a porta; `listen()` deixa esperando; `accept()` bloqueia até alguém conectar
- Dados de rede são **bytes**: `.encode()` (string→bytes) e `.decode()` (bytes→string)
- Sem `recv()` antes do `send()`, a conexão pode fechar de forma abrupta (`ERR_CONNECTION_ABORTED`)
- Sem `Connection: close` + `conexao.close()`, o navegador fica esperando mais dados

---

## 2. Flask

Setup mínimo:
```python
from flask import Flask, request
app = Flask(__name__)

@app.route("/")
def home():
    return "Ola mundo"

app.run(port=8080)
```

Rota com método específico e leitura de dados enviados:
```python
@app.route('/cadastro', methods=["POST"])
def cadastro():
    nome = request.form.get("nome")
    return f"Cadastro: {nome}"
```

**O que o Flask já resolve sozinho** (que era manual no socket puro): parsing da requisição, roteamento por path, 404 automático, bloqueio de métodos não permitidos por padrão (só GET, a menos que `methods=[...]` diga o contrário), `Content-Length`, encoding.

**Testando POST/PUT/DELETE sem front-end (curl):**
```bash
curl.exe -X POST http://localhost:8080/cadastro -d "nome=Victor&idade=25"
curl.exe -X PUT http://localhost:8080/usuario -d "nome_antigo=A&nome_novo=B"
curl.exe -X DELETE http://localhost:8080/usuario -d "nome=A"
```
No PowerShell use sempre `curl.exe` (não só `curl`, que é alias de `Invoke-WebRequest` e quebra com `-X`/`-d`).

---

## 3. SQL / SQLite

**Conectar** (cria o arquivo se não existir, abre se já existir):
```python
import sqlite3
conexao = sqlite3.connect("banco.db")
cursor = conexao.cursor()
```

**CREATE** (uma vez só — dá erro se rodar de novo com a tabela já existindo):
```python
cursor.execute("""
    CREATE TABLE usuarios (
        id INTEGER PRIMARY KEY,
        nome TEXT,
        idade INTEGER,
        email TEXT,
        senha TEXT
    )
""")
```

**ALTER** (adicionar coluna numa tabela que já existe):
```python
cursor.execute("ALTER TABLE usuarios ADD COLUMN senha TEXT")
```

**INSERT** — sempre com `?`, nunca f-string:
```python
cursor.execute("INSERT INTO usuarios (nome, idade, email) VALUES (?, ?, ?)", (nome, idade, email))
conexao.commit()  # obrigatório pra persistir a mudança
```

**SELECT:**
```python
cursor.execute("SELECT * FROM usuarios")
resultados = cursor.fetchall()  # lista de tuplas: [(1, 'Victor', 25, ...), ...]

cursor.execute("SELECT senha FROM usuarios WHERE nome = ?", (nome,))
resultado = cursor.fetchone()  # uma tupla só, ou None se não achar
```

**UPDATE:**
```python
cursor.execute("UPDATE usuarios SET nome = ? WHERE nome = ?", (nome_novo, nome_antigo))
conexao.commit()
```

**DELETE:**
```python
cursor.execute("DELETE FROM usuarios WHERE nome = ?", (nome,))
conexao.commit()
```

⚠️ **Tupla de um elemento só precisa da vírgula:** `(nome,)`. Sem a vírgula, `(nome)` é só o valor entre parênteses — o SQLite trata a string como uma sequência de caracteres individuais (erro clássico: "incorrect number of bindings").

⚠️ **SQL Injection:** nunca montar o comando com f-string usando dado vindo de fora (`f"... WHERE nome = '{nome}'"`). Usar sempre `?` + tupla separada — assim o valor é sempre tratado como dado puro, nunca como parte do comando.

---

## 4. Variáveis de ambiente (`.env`)

**Instalar:** `py -m pip install python-dotenv`

**Arquivo `.env`** (raiz do projeto, nome exato — sem nada antes do ponto, sem extensão extra):
```
NOME_BANCO=banco.db
CHAVE_SECRETA=uma_frase_dificil_de_adivinhar
```

**Uso no código:**
```python
from dotenv import load_dotenv
import os

load_dotenv()
NOME_BANCO = os.getenv("NOME_BANCO")  # sem aspas ao usar — é variável, não string fixa
```

**Nunca sobe pro GitHub.** Via terminal (`git add`), adiciona no `.gitignore`:
```
.env
```
Via upload manual (interface web do GitHub), basta nunca selecionar o `.env` na hora de enviar.

---

## 5. Hash de senha

```python
from werkzeug.security import generate_password_hash, check_password_hash

senha_hash = generate_password_hash(senha)                  # ao registrar
check_password_hash(senha_hash, senha_digitada)              # ao logar → True/False
```

Hash é **irreversível** — nunca se "descriptografa" a senha salva. Pra conferir, se faz hash do que a pessoa digitou agora e compara os dois hashes.

---

## 6. JWT (autenticação)

**Instalar:** `py -m pip install pyjwt`

**Gerar o token** (no login, depois da senha validada):
```python
import jwt
import datetime

token = jwt.encode(
    {"nome": nome, "exp": datetime.datetime.now(datetime.timezone.utc) + datetime.timedelta(hours=1)},
    CHAVE_SECRETA,
    algorithm="HS256"
)
```

**Proteger uma rota** (exigir token válido):
```python
@app.route('/perfil', methods=["GET"])
def perfil():
    token_completo = request.headers.get("Authorization")  # "Bearer eyJ..."
    token = token_completo.split(" ")[1]

    try:
        dados = jwt.decode(token, CHAVE_SECRETA, algorithms=["HS256"])
        return f"Bem-vindo, {dados['nome']}!"
    except jwt.ExpiredSignatureError:
        return "Token expirado!"
    except jwt.InvalidTokenError:
        return "Token inválido!"
```

**Testar com curl:**
```bash
curl.exe http://localhost:8080/perfil -H "Authorization: Bearer SEU_TOKEN_AQUI"
```

⚠️ O payload do JWT (2ª parte do token, entre os pontos) é só **codificado**, não criptografado — qualquer um consegue ler o conteúdo. Nunca colocar dado sensível ali (senha, cartão). Só a 3ª parte (assinatura) depende da chave secreta, e é o que impede forjar um token.

---

## Métodos Python usados com frequência aqui

| O que faz | Sintaxe |
|---|---|
| Bytes → string | `.decode()` |
| String → bytes | `.encode()` |
| Dividir string em lista | `.split(separador)` |
| Posição de um valor numa lista | `lista.index(valor)` |
| Remover valor de uma lista | `lista.remove(valor)` |
| Checar existência | `valor in lista` |
| Número → string | `str(numero)` |
| Formatar com variáveis | `f"texto {variavel}"` |
| Tentar/capturar erro | `try: ... except TipoDeErro: ...` |

---

## Erros que já apareceram e o que significam

- `ERR_CONNECTION_REFUSED` — servidor não está rodando (ou já terminou)
- `ERR_INVALID_HTTP_RESPONSE` — a resposta enviada não segue o formato HTTP esperado
- `ERR_CONNECTION_ABORTED` — conexão fechada sem ler a requisição primeiro (faltou `recv()`)
- `TypeError: can't concat str to bytes` — misturou string com bytes numa soma
- `sqlite3.ProgrammingError: Incorrect number of bindings` — geralmente falta vírgula numa tupla de 1 elemento: `(nome,)`
- `AttributeError: 'NoneType' object has no attribute...` — tentou usar `.algumacoisa()` num valor que veio `None` (ex: usuário ou coluna que não existe)
- `TypeError: View function did not return a response` — uma rota Flask terminou sem nenhum `return`
