# Referência — Tabelas Relacionais e ORM (Python vs Node)

> Complemento aos arquivos de referência anteriores (sockets/Flask/SQL/segurança e Express/node:sqlite/JWT). Cobre: chaves estrangeiras, JOIN, e ORM (SQLAlchemy no Python, Prisma no Node).

---

## 1. Tabelas relacionais (SQL cru)

### Conceitos
- **Chave primária (PK)**: campo único que identifica cada linha (`id`).
- **Chave estrangeira (FK)**: campo que guarda o `id` de um registro de outra tabela, criando um vínculo.
- Por padrão, o SQLite **não fiscaliza** FKs — é preciso ativar isso manualmente por conexão.

### Python (sqlite3)
```python
# Ativar fiscalização de FK (repetir em toda conexão nova)
conexao.execute("PRAGMA foreign_keys = ON")

# Criar tabela com FK
cursor.execute("""
    CREATE TABLE IF NOT EXISTS tarefas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        titulo TEXT NOT NULL,
        concluida INTEGER DEFAULT 0,
        usuario_id INTEGER NOT NULL,
        FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
    )
""")
```

### Node (node:sqlite)
```javascript
// Ativar fiscalização de FK
db.exec('PRAGMA foreign_keys = ON');

// Criar tabela com FK
db.exec(`
    CREATE TABLE IF NOT EXISTS tarefas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        titulo TEXT NOT NULL,
        concluida INTEGER DEFAULT 0,
        usuario_id INTEGER NOT NULL,
        FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
    )
`);
```

### JOIN — combinando duas tabelas numa consulta

**INNER JOIN** — só traz linhas que têm correspondência nas duas tabelas:
```sql
SELECT tarefas.id, tarefas.titulo, tarefas.concluida, usuarios.nome
FROM tarefas
JOIN usuarios ON tarefas.usuario_id = usuarios.id
```

**LEFT JOIN** — traz todos os registros da tabela da esquerda, mesmo sem correspondência (campo vem `NULL`/`None`):
```sql
SELECT usuarios.nome, tarefas.titulo
FROM usuarios
LEFT JOIN tarefas ON usuarios.id = tarefas.usuario_id
```

**Regra prática:** "quero ver X com ou sem Y" → `LEFT JOIN`. "Quero ver só quando X e Y existem juntos" → `INNER JOIN` (padrão).

---

## 2. ORM — Python (SQLAlchemy / Flask-SQLAlchemy)

### Instalação e configuração
```python
from flask_sqlalchemy import SQLAlchemy
import os

BASE_DIR = os.path.abspath(os.path.dirname(__file__))
app.config["SQLALCHEMY_DATABASE_URI"] = f"sqlite:///{os.path.join(BASE_DIR, NOME_BANCO)}"
db = SQLAlchemy(app)
```
> ⚠️ Usar caminho absoluto (`BASE_DIR`) evita o Flask criar um banco "fantasma" dentro de uma pasta `instance/`.

### Definindo modelos (classes = tabelas)
```python
class Usuario(db.Model):
    __tablename__ = "usuarios"  # nome real da tabela no banco
    id = db.Column(db.Integer, primary_key=True)
    nome = db.Column(db.String)
    idade = db.Column(db.Integer)
    email = db.Column(db.String)
    senha = db.Column(db.String)
    tarefas = db.relationship("Tarefa", backref="usuario")  # opcional se já declarado do outro lado

class Tarefa(db.Model):
    __tablename__ = "tarefas"
    id = db.Column(db.Integer, primary_key=True)
    titulo = db.Column(db.String)
    concluida = db.Column(db.Integer)
    usuario_id = db.Column(db.Integer, db.ForeignKey("usuarios.id"))
    usuario = db.relationship("Usuario", backref="tarefas")  # navegação: tarefa.usuario.nome
```
> `__tablename__` é obrigatório sempre que o nome da classe não bater exatamente com o nome real da tabela (SQLAlchemy pluraliza/minusculiza sozinho por padrão).

### CRUD com SQLAlchemy
```python
# CREATE
novo = Usuario(nome=nome, idade=idade, email=email)
db.session.add(novo)
db.session.commit()

# READ (todos)
usuarios = Usuario.query.all()

# READ (um, com filtro)
usuario = Usuario.query.filter_by(nome=nome).first()  # None se não achar

# UPDATE
usuario.nome = novo_nome
db.session.commit()

# DELETE
db.session.delete(usuario)
db.session.commit()

# JOIN via relationship (equivalente ao INNER JOIN)
for tarefa in Tarefa.query.all():
    print(tarefa.titulo, tarefa.usuario.nome)
```

---

## 3. ORM — Node (Prisma, v7 com driver adapter)

### Instalação e configuração
```powershell
npm install prisma @prisma/client
npm install @prisma/adapter-better-sqlite3 better-sqlite3
npx prisma init --datasource-provider sqlite
```

`.env`:
```dotenv
DATABASE_URL="file:./banco.db"
```

`schema.prisma`:
```prisma
generator client {
  provider = "prisma-client-js"
}

datasource db {
  provider = "sqlite"
}

model Usuario {
  id      Int      @id @default(autoincrement())
  nome    String?
  idade   Int?
  email   String?
  senha   String?
  tarefas Tarefa[]

  @@map("usuarios")
}

model Tarefa {
  id         Int     @id @default(autoincrement())
  titulo     String
  concluida  Int     @default(0)
  usuario_id Int
  usuario    Usuario @relation(fields: [usuario_id], references: [id])

  @@map("tarefas")
}
```
> `@@map("nome_real")` é o equivalente ao `__tablename__` do SQLAlchemy — obrigatório quando o nome do `model` não bate com o nome real da tabela.

Sincronizar schema com o banco (sem histórico de migrations, direto):
```powershell
npx prisma generate
npx prisma db push
```

### Inicializando o client (Prisma 7 exige driver adapter)
```javascript
const { PrismaClient } = require('@prisma/client');
const { PrismaBetterSqlite3 } = require('@prisma/adapter-better-sqlite3');

const adapter = new PrismaBetterSqlite3({ url: process.env.DATABASE_URL });
const prisma = new PrismaClient({ adapter });
```
> ⚠️ A partir do Prisma 7, `new PrismaClient()` sozinho **não funciona mais** — é obrigatório passar um adapter.

### CRUD com Prisma
```javascript
// CREATE
const novo = await prisma.usuario.create({
    data: { nome, idade: parseInt(idade), email }
});

// READ (todos)
const usuarios = await prisma.usuario.findMany();

// READ com relação (equivalente ao INNER JOIN)
const tarefas = await prisma.tarefa.findMany({
    include: { usuario: true }
});

// UPDATE — atenção: .update() exige campo único no `where`.
// Para filtrar por campo não-único, usar .updateMany()
const atualizado = await prisma.usuario.updateMany({
    where: { nome: nomeAntigo },
    data: { nome: nomeNovo }
});

// DELETE — mesma regra: .delete() exige único, senão .deleteMany()
const deletado = await prisma.usuario.deleteMany({
    where: { nome }
});
```
> ⚠️ `req.body` sempre entrega texto. Campos `Int` no schema (`idade`, `id`, `usuario_id`, `concluida`) precisam de `parseInt(...)` antes de ir pro Prisma, senão dá erro de validação de tipo.

---

## 4. Tabela comparativa — Python (SQLAlchemy) vs Node (Prisma)

| Conceito | SQLAlchemy (Python) | Prisma (Node) |
|---|---|---|
| Define tabela | `class Usuario(db.Model):` | `model Usuario { ... }` (arquivo `schema.prisma`, sintaxe própria) |
| Nome real da tabela | `__tablename__ = "usuarios"` | `@@map("usuarios")` |
| Chave primária | `db.Column(db.Integer, primary_key=True)` | `Int @id @default(autoincrement())` |
| Chave estrangeira | `db.ForeignKey("usuarios.id")` | `@relation(fields: [usuario_id], references: [id])` |
| Navegar relação | `db.relationship(...)` → `tarefa.usuario.nome` | campo de relação no model → `tarefa.usuario.nome` |
| Buscar todos | `Usuario.query.all()` | `await prisma.usuario.findMany()` |
| Buscar um (filtro) | `Usuario.query.filter_by(nome=x).first()` | `await prisma.usuario.findFirst({ where: { nome: x } })` |
| Criar registro | `db.session.add(obj)` + `db.session.commit()` | `await prisma.usuario.create({ data: {...} })` |
| Atualizar (campo não-único) | `obj.campo = valor` + `db.session.commit()` | `await prisma.usuario.updateMany({ where, data })` |
| Deletar (campo não-único) | `db.session.delete(obj)` + `commit()` | `await prisma.usuario.deleteMany({ where })` |
| JOIN / dados relacionados | automático via `relationship` | `include: { usuario: true }` no `findMany` |
| Geração de cliente | não precisa (classes já são o "cliente") | `npx prisma generate` |
| Sincronizar schema → banco | `db.create_all()` (só cria o que falta) | `npx prisma db push` |
| Assíncrono? | Não (Flask/SQLAlchemy padrão é síncrono) | Sim — toda operação usa `await` |

---

## 5. Erros comuns e como resolver

| Erro | Causa | Solução |
|---|---|---|
| `no such table: usuario` (SQLAlchemy) | Nome da classe ≠ nome real da tabela | Adicionar `__tablename__` |
| `no such table: usuarios` mesmo com `__tablename__` certo | Flask criando banco duplicado em `instance/` (caminho relativo) | Usar caminho absoluto com `BASE_DIR = os.path.abspath(os.path.dirname(__file__))` |
| `Argument where needs at least one of id arguments` (Prisma) | `.update()`/`.delete()` usados com campo não-único no `where` | Trocar por `.updateMany()`/`.deleteMany()` |
| `Expected Int, provided String` (Prisma) | `req.body` sempre é texto, campo é `Int` no schema | `parseInt(req.body.campo)` |
| `A driver adapter is required` (Prisma 7) | Prisma 7 não conecta sozinho ao banco | Instalar `@prisma/adapter-better-sqlite3` e passar `{ adapter }` no `new PrismaClient()` |
| `Drift detected` / pede reset (Prisma Migrate) | Tabelas criadas manualmente fora do controle do Prisma | Usar `npx prisma db push` em vez do fluxo de migrations, ou fazer baseline (`migrate diff` + `migrate resolve --applied`) |

---

*Gerado a partir da Fase 10 do roadmap de back-end (tabelas relacionais + ORM), Python e Node.*
