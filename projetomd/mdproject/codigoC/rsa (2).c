#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// FUNÇÕES PARA GERAR A CRIPTOGRAFIA


long long mdc(long long a, long long b)
{
    while (b != 0)
    {
        long long resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

long long calcular_inverso_modular(long long a, long long m)
{
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;
    if (m == 1) return 0;
    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

long long mult_mod(long long a, long long b, long long mod)
{
    long long resultado = 0;
    a = a % mod;
    while (b > 0)
    {
        if (b % 2 == 1)
            resultado = (resultado + a) % mod;
        a = (a * 2) % mod;
        b = b >> 1;
    }
    return resultado;
}

long long calcular_exponenciacao_modular(long long base, long long exp, long long mod)
{
    long long resultado = 1;
    base = base % mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            resultado = mult_mod(resultado, base, mod);
        exp = exp >> 1;
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


// OPÇÃO 1 — GERAR E SALVAR CHAVE PÚBLICA


void gerar_chave_publica(void)
{
    long long p, q;

    scanf("%lld", &p);
    if (!verificar_primo(p))
    {
        return;
    }

    scanf("%lld", &q);
    if (!verificar_primo(q))
    {
        return;
    }

    if (p == q)
    {
        return;
    }

    long long phi = (p - 1) * (q - 1);

    long long e;
    printf("Digite o expoente 'e' coprimo a %lld: ", phi);
    scanf("%lld", &e);

    if (e <= 0)
    {
        return;
    }

    if (mdc(e, phi) != 1)
    {
        return;
    }

    long long n = p * q;

    if (n <= 127)
    {
        return;
    }

    FILE *arquivo = fopen("chave_publica.txt", "w");
    if (arquivo == NULL)
    {
        return;
    }
    fprintf(arquivo, "%lld %lld", n, e);
    fclose(arquivo);
}


// OPÇÃO 2 — ENCRIPTAR MENSAGEM


void encriptar(void)
{
    long long e, n;

    scanf("%lld", &e);
    if (e <= 0)
    {
        return;
    }

    scanf("%lld", &n);
    if (n <= 127)
    {
        return;
    }

    FILE *arq_entrada = fopen("mensagem.txt", "r");
    if (arq_entrada == NULL)
    {
        return;
    }

    FILE *arq_saida = fopen("mensagem_criptografada.txt", "w");
    if (arq_saida == NULL)
    {
        fclose(arq_entrada);
        return;
    }

    int caractere;

    while ((caractere = fgetc(arq_entrada)) != EOF)
    {
        if (caractere >= 32)
        {
            long long numero_criptografado = calcular_exponenciacao_modular(caractere, e, n);
            fprintf(arq_saida, "%lld ", numero_criptografado);
        }
    }

    fclose(arq_entrada);
    fclose(arq_saida);
 }


// OPÇÃO 3 — DESENCRIPTAR MENSAGEM


void desencriptar(void)
{
    long long p, q, e;

    scanf("%lld", &p);
    if (!verificar_primo(p))
    {
        return;
    }

    scanf("%lld", &q);
    if (!verificar_primo(q))
    {
        return;
    }

    if (p == q)
    {
        return;
    }

    if (p * q <= 127)
    {
        return;
    }

    scanf("%lld", &e);
    if (e <= 0)
    {
        return;
    }

    long long phi = (p - 1) * (q - 1);
    if (mdc(e, phi) != 1)
    {
        return;
    }

    long long n = p * q;
    long long d = calcular_inverso_modular(e, phi);

    char nome_arquivo[100];
    scanf("%99s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL)
    {
        return;
    }

    int capacidade = 1000;
    char *mensagem_decriptada = (char *)malloc(capacidade * sizeof(char));
    if (mensagem_decriptada == NULL)
    {
        fclose(arquivo);
        return;
    }

    long long numero_encriptado;
    int index = 0;

    while (fscanf(arquivo, "%lld", &numero_encriptado) == 1)
    {
        if (index >= capacidade - 1)
        {
            capacidade *= 2;
            char *temp = (char *)realloc(mensagem_decriptada, capacidade * sizeof(char));
            if (temp == NULL)
            {
                free(mensagem_decriptada);
                fclose(arquivo);
                return;
            }
            mensagem_decriptada = temp;
        }

        long long numero_decriptado = calcular_exponenciacao_modular(numero_encriptado, d, n);
        mensagem_decriptada[index] = (char)numero_decriptado;
        index++;
    }
    mensagem_decriptada[index] = '\0';
    fclose(arquivo);

    FILE *arquivo_saida = fopen("mensagem_desencriptada.txt", "w");
    if (arquivo_saida != NULL)
    {
        fprintf(arquivo_saida, "%s", mensagem_decriptada);
        fclose(arquivo_saida);
    }

    free(mensagem_decriptada);
}


// MENU PRINCIPAL


int main(void)
{
    int opcao;

    do
    {
        if (scanf("%d", &opcao) != 1)
        {
            opcao = -1;
            while (getchar() != '\n');
        }

        if (opcao == 1)
            gerar_chave_publica();
        else if (opcao == 2)
            encriptar();
        else if (opcao == 3)
            desencriptar();
        else if (opcao != 0)
    } while (opcao != 0);

    return 0;
}
