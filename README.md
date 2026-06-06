# Projeto criptografia RSA - Matematica discreta
projeto da academico sobre criptografia RSA com interface web em framework django de python e logica criptografica em C. 


## Stacks 
- Python3 + Django4.x
- HTML + CSS com templates Django
- C para encriptar, desencriptar e formar a chave publica integrado via ctypes
- SQLite(banco padrão do Django)

## Arquitetura do projeto
PROJETO-MATEMATICA-DISCRETA-CRIPTOGRAFIA/
├── .gitignore
├── requirements.txt
├── README.md
└── projetomd/
    ├── manage.py
    ├── projetomd/              ← configurações Django
    │   ├── settings.py
    │   ├── urls.py
    │   └── wsgi.py
    └── mdproject/              ← app principal
        ├── views.py            ← integração Python ↔ C via ctypes
        ├── urls.py
        ├── codigoC/
        │   ├── rsa.c           ← versão CLI original
        │   ├── menu.c          ← versão biblioteca 
        │   └── menu.so         ← compilado pelo gcc
        ├── static/             ← arquivos CSS
        │   ├── homepage.css
        │   ├── chavepublica.css
        │   ├── encriptar.css
        │   └── desencriptar.css
        └── templates
            ├── homepage.html
            ├── chavepublica.html
            ├── encriptar.html
            └── desencriptar.html



## Criar e ativar o ambiente virtual

```bash
python3 -m venv .venv
source .venv/bin/activate
```

> **VS Code:** `Ctrl+Shift+P` → *Python: Select Interpreter* → `.venv/bin/python`

##  Instalar dependências

```bash
pip install -r requirements.txt
```

## Compilar a biblioteca C

```bash
cd PROJETO-MATEMATICA-DISCRETA-CRIPTOGRAFIA
cd projetomd/mdproject/codigoC
gcc -shared -fPIC -o menu.so menu.c
cd ../../..
```
## Inicializar o banco e rodar

```bash
cd projetomd
python manage.py migrate
python manage.py runserver
```
Acesse: **http://127.0.0.1:8000/mdproject/**
