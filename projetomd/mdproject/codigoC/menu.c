/*
 * menu.c — Biblioteca RSA para integração com Django via ctypes
 *
 * Compilar (Linux):
 *   gcc -shared -fPIC -o menu.so menu.c
 *
 * Diferenças em relação ao rsa.c (versão CLI):
 *   - Sem função main()
 *   - Funções exportadas recebem parâmetros diretamente (sem scanf)
 *   - mult_mod usa __int128 para evitar overflow em primos de 6+ dígitos
 *   - Resultados retornados via arquivos txt no diretório de execução
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Funções matemáticas ─────────────────────────────────── */

long long mdc(long long a, long long b)
{
    while (b != 0) {
        long long resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

long long calcular_inverso_modular(long long a, long long m)
{
    long long m0 = m;
    long long x0 = 0, x1 = 1;
    if (m == 1) return 0;
    while (a > 1) {
        long long q = a / m;
        long long t = m;
        m  = a % m;
        a  = t;
        t  = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

/*
 * Multiplicação modular segura com __int128.
 * Sem isso, (a * b) pode transbordar para primos de 6+ dígitos,
 * pois n = p*q pode chegar a ~10^12.
 */
static long long mult_mod(long long a, long long b, long long mod)
{
    return ((__int128)a * b) % mod;
}

long long calcular_exponenciacao_modular(long long base, long long exp, long long mod)
{
    long long resultado = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            resultado = mult_mod(resultado, base, mod);
        exp  = exp >> 1;
        base = mult_mod(base, base, mod);
    }
    return resultado;
}

int verificar_primo(long long n)
{
    if (n < 2) return 0;
    for (long long i = 2; i * i <= n; i++)
        if (n % i == 0) return 0;
    return 1;
}

/* ── Funções exportadas (chamadas pelo Django via ctypes) ─── */

/*
 * salvar_chave(p, q, e)
 * Valida p, q, e e grava "chave_publica.txt" com "n e".
 * Retorna 0 em sucesso, código de erro negativo em falha.
 */
int salvar_chave(long long p, long long q, long long e)
{
    if (!verificar_primo(p)) return -1;   /* p não é primo        */
    if (!verificar_primo(q)) return -2;   /* q não é primo        */
    if (p == q)              return -3;   /* p e q iguais         */

    long long phi = (p - 1) * (q - 1);
    if (mdc(e, phi) != 1)   return -4;   /* e não coprimo a φ(n) */

    long long n = p * q;
    if (n <= 127)            return -5;   /* n muito pequeno      */

    FILE *f = fopen("chave_publica.txt", "w");
    if (!f)                  return -6;   /* erro ao criar arquivo */
    fprintf(f, "%lld %lld", n, e);
    fclose(f);
    return 0;
}

/*
 * encriptar(mensagem, n, e)
 * Encripta cada byte da mensagem e grava "mensagem_criptografada.txt".
 * Retorna 0 em sucesso, código negativo em falha.
 */
int encriptar(const char *mensagem, long long n, long long e)
{
    if (!mensagem || n <= 127) return -1;

    FILE *f = fopen("mensagem_criptografada.txt", "w");
    if (!f) return -2;

    for (int i = 0; mensagem[i] != '\0'; i++) {
        unsigned char c = (unsigned char)mensagem[i];
        if (c >= 32) {
            long long cifrado = calcular_exponenciacao_modular(c, e, n);
            fprintf(f, "%lld ", cifrado);
        }
    }
    fclose(f);
    return 0;
}

/*
 * desencriptar(p, q, e)
 * Lê "mensagem_criptografada.txt", calcula d e grava "mensagem_desencriptada.txt".
 * Retorna 0 em sucesso, código negativo em falha.
 */
int desencriptar(long long p, long long q, long long e)
{
    if (!verificar_primo(p)) return -1;
    if (!verificar_primo(q)) return -2;
    if (p == q)              return -3;

    long long n   = p * q;
    long long phi = (p - 1) * (q - 1);
    if (mdc(e, phi) != 1)   return -4;

    long long d = calcular_inverso_modular(e, phi);

    FILE *fin = fopen("mensagem_criptografada.txt", "r");
    if (!fin) return -5;

    int cap = 4096;
    char *buf = (char *)malloc(cap);
    if (!buf) { fclose(fin); return -6; }

    long long num;
    int idx = 0;
    while (fscanf(fin, "%lld", &num) == 1) {
        if (idx >= cap - 1) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap);
            if (!tmp) { free(buf); fclose(fin); return -7; }
            buf = tmp;
        }
        buf[idx++] = (char)calcular_exponenciacao_modular(num, d, n);
    }
    buf[idx] = '\0';
    fclose(fin);

    FILE *fout = fopen("mensagem_desencriptada.txt", "w");
    if (!fout) { free(buf); return -8; }
    fprintf(fout, "%s", buf);
    fclose(fout);
    free(buf);
    return 0;
}
