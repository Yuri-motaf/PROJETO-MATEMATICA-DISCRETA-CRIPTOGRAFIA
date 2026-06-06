import ctypes
import os
from django.shortcuts import render

# ── Carregamento da biblioteca C ──────────────────────────────────────────────
# Caminho absoluto baseado na localização deste arquivo (views.py),
# independente de onde o manage.py runserver é executado.
_BASE     = os.path.dirname(os.path.abspath(__file__))
_LIB_PATH = os.path.join(_BASE, 'codigoC', 'menu.so')

# Diretório raiz do projeto (onde manage.py fica) — local dos .txt de saída
_ROOT = os.path.dirname(_BASE)

try:
    _lib = ctypes.CDLL(_LIB_PATH)

    # Declaração explícita de tipos — OBRIGATÓRIO para primos de 6+ dígitos.
    # Sem isso ctypes usa int 32-bit e os valores ficam corrompidos silenciosamente.
    _lib.salvar_chave.argtypes  = [ctypes.c_longlong, ctypes.c_longlong, ctypes.c_longlong]
    _lib.salvar_chave.restype   = ctypes.c_int

    _lib.encriptar.argtypes     = [ctypes.c_char_p, ctypes.c_longlong, ctypes.c_longlong]
    _lib.encriptar.restype      = ctypes.c_int

    _lib.desencriptar.argtypes  = [ctypes.c_longlong, ctypes.c_longlong, ctypes.c_longlong]
    _lib.desencriptar.restype   = ctypes.c_int

    _LIB_OK    = True
    _LIB_ERRO  = None

except OSError as exc:
    _LIB_OK   = False
    _LIB_ERRO = str(exc)


# Mensagens de erro retornadas pelo código C
_ERROS_C = {
    -1: 'p não é um número primo.',
    -2: 'q não é um número primo.',
    -3: 'p e q devem ser distintos.',
    -4: 'e não é coprimo a φ(n) = (p−1)(q−1).',
    -5: 'p × q deve ser maior que 127.',
    -6: 'Erro interno ao criar o arquivo de saída.',
    -7: 'Erro interno de memória.',
    -8: 'Erro ao gravar o arquivo de saída.',
}


def _erro_lib(request, template):
    """Renderiza mensagem de erro caso a biblioteca C não tenha carregado."""
    if not _LIB_OK:
        return render(request, template, {
            'erro': (
                f'Biblioteca C não encontrada em {_LIB_PATH}. '
                f'Compile com: gcc -shared -fPIC -o menu.so menu.c  '
                f'(Detalhe: {_LIB_ERRO})'
            )
        })
    return None


def _ler_arquivo(nome):
    """Lê um .txt gerado pela lib C no diretório raiz do projeto."""
    caminho = os.path.join(_ROOT, nome)
    try:
        with open(caminho, 'r') as f:
            return f.read().strip()
    except FileNotFoundError:
        return None


# ── Views ─────────────────────────────────────────────────────────────────────

def index(request):
    return render(request, 'mdproject/homepage.html')


def save_key(request):
    resultado = None
    erro      = None

    guard = _erro_lib(request, 'mdproject/chavepublica.html')
    if guard:
        return guard

    if request.method == 'POST':
        try:
            p = int(request.POST['p'])
            q = int(request.POST['q'])
            e = int(request.POST['e'])
        except (ValueError, KeyError):
            erro = 'Preencha todos os campos com números inteiros válidos.'
        else:
            ret = _lib.salvar_chave(p, q, e)
            if ret == 0:
                resultado = _ler_arquivo('chave_publica.txt')
                if resultado is None:
                    erro = 'Arquivo gerado mas não encontrado. Verifique permissões.'
            else:
                erro = _ERROS_C.get(ret, f'Erro desconhecido (código {ret}).')

    return render(request, 'mdproject/chavepublica.html', {
        'resultado': resultado,
        'erro':      erro,
    })


def encrypt(request):
    resultado = None
    erro      = None

    guard = _erro_lib(request, 'mdproject/encriptar.html')
    if guard:
        return guard

    if request.method == 'POST':
        try:
            mensagem = request.POST['plaintext'].encode('utf-8')
            e        = int(request.POST['public_key_e'])
            n        = int(request.POST['public_key_n'])
        except (ValueError, KeyError):
            erro = 'Preencha todos os campos corretamente.'
        else:
            ret = _lib.encriptar(mensagem, n, e)
            if ret == 0:
                resultado = _ler_arquivo('mensagem_criptografada.txt')
                if resultado is None:
                    erro = 'Arquivo de saída não encontrado após encriptação.'
            else:
                erro = _ERROS_C.get(ret, f'Erro desconhecido (código {ret}).')

    return render(request, 'mdproject/encriptar.html', {
        'resultado': resultado,
        'erro':      erro,
    })


def decrypt(request):
    resultado = None
    erro      = None

    guard = _erro_lib(request, 'mdproject/desencriptar.html')
    if guard:
        return guard

    if request.method == 'POST':
        try:
            p = int(request.POST['dec_p'])
            q = int(request.POST['dec_q'])
            e = int(request.POST['dec_e'])
        except (ValueError, KeyError):
            erro = 'Preencha todos os campos corretamente.'
        else:
            ret = _lib.desencriptar(p, q, e)
            if ret == 0:
                resultado = _ler_arquivo('mensagem_desencriptada.txt')
                if resultado is None:
                    erro = 'Arquivo de saída não encontrado. Encripte uma mensagem primeiro.'
            else:
                erro = _ERROS_C.get(ret, f'Erro desconhecido (código {ret}).')

    return render(request, 'mdproject/desencriptar.html', {
        'resultado': resultado,
        'erro':      erro,
    })
