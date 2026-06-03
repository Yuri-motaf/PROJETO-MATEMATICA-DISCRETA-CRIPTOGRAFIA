#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//  FUNÇÕES PARA GERAR A CRIPTOGRAFIA


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
    __int128 result = (__int128)a * b;
    return (long long)(result % mod);
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


 //  OPÇÃO 1 — GERAR E SALVAR CHAVE PÚBLICA


void gerar_chave_publica(void)
{
    long long p, q;

    printf("\n=== GERAR CHAVE PÚBLICA ===\n\n");

    printf("Digite o primeiro número primo (p): ");
    scanf("%lld", &p);
    if (!verificar_primo(p))
    {
        printf("Erro: p nao e primo!\n");
        return;
    }

    printf("Digite o segundo número primo (q): ");
    scanf("%lld", &q);
    if (!verificar_primo(q))
    {
        printf("Erro: q nao e primo!\n");
        return;
    }

    if (p == q)
    {
        printf("Erro: p e q devem ser distintos!\n");
        return;
    }

    long long phi = (p - 1) * (q - 1);

    long long e;
    printf("Digite o expoente 'e' coprimo a %lld: ", phi);
    scanf("%lld", &e);

    if (mdc(e, phi) != 1)
    {
        printf("Erro: e nao e coprimo a phi(n)!\n");
        return;
    }

    long long n = p * q;

    FILE *arquivo = fopen("chave_publica.txt", "w");
    if (arquivo == NULL)
    {
        printf("Erro ao criar o arquivo!\n");
        return;
    }
    fprintf(arquivo, "%lld %lld", n, e);
    fclose(arquivo);

    printf("Chave publica salva em 'chave_publica.txt': n=%lld, e=%lld\n", n, e);
}


//   OPÇÃO 2 — ENCRIPTAR MENSAGEM


void encriptar(void)
{
    long long e, n;

    printf("\n=== ENCRIPTAÇÃO RSA ===\n\n");

    printf("Digite o expoente publico 'e': ");
    scanf("%lld", &e);
    if (e <= 0)
    {
        printf("Erro: e deve ser positivo!\n");
        return;
    }

    printf("Digite o modulo 'n': ");
    scanf("%lld", &n);
    if (n <= 127)
    {
        printf("Erro: n deve ser maior que 127!\n");
        return;
    }

    FILE *arq_entrada = fopen("mensagem.txt", "r");
    if (arq_entrada == NULL)
    {
        printf("Erro: arquivo 'mensagem.txt' nao encontrado!\n");
        printf("Crie um arquivo 'mensagem.txt' com o texto a encriptar.\n");
        return;
    }

    FILE *arq_saida = fopen("mensagem_criptografada.txt", "w");
    if (arq_saida == NULL)
    {
        printf("Erro ao criar arquivo de saida!\n");
        fclose(arq_entrada);
        return;
    }

    int caractere;
    printf("\nCriptografando os caracteres:\n");

    while ((caractere = fgetc(arq_entrada)) != EOF)
    {
        if (caractere >= 32)
        {
            printf("Letra: '%c' (ASCII: %d) ", caractere, caractere);
            long long numero_criptografado = calcular_exponenciacao_modular(caractere, e, n);
            printf("-> Criptografado: %lld\n", numero_criptografado);
            fprintf(arq_saida, "%lld ", numero_criptografado);
        }
    }

    fclose(arq_entrada);
    fclose(arq_saida);
    printf("\nSucesso! Mensagem salva em 'mensagem_criptografada.txt'\n");
}


//   OPÇÃO 3 — DESENCRIPTAR MENSAGEM


void desencriptar(void)
{
    long long p, q, e;

    printf("\n=== DESENCRIPTAÇÃO RSA ===\n\n");

    printf("Digite o número primo p: ");
    scanf("%lld", &p);
    if (!verificar_primo(p))
    {
        printf("Erro: p nao e primo!\n");
        return;
    }

    printf("Digite o número primo q: ");
    scanf("%lld", &q);
    if (!verificar_primo(q))
    {
        printf("Erro: q nao e primo!\n");
        return;
    }

    if (p == q)
    {
        printf("Erro: p e q devem ser distintos!\n");
        return;
    }

    if (p * q <= 127)
    {
        printf("Erro: p*q deve ser maior que 127!\n");
        return;
    }

    printf("Digite o expoente e: ");
    scanf("%lld", &e);
    if (e <= 0)
    {
        printf("Erro: e deve ser positivo!\n");
        return;
    }

    long long phi = (p - 1) * (q - 1);
    if (mdc(e, phi) != 1)
    {
        printf("Erro: e nao e relativamente primo a phi(n)!\n");
        return;
    }

    long long n = p * q;
    long long d = calcular_inverso_modular(e, phi);
    printf("Chave privada d calculada: %lld\n", d);

    char nome_arquivo[100];
    printf("Digite o nome do arquivo com a mensagem encriptada: ");
    scanf("%99s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    int capacidade = 1000;
    char *mensagem_decriptada = (char *)malloc(capacidade * sizeof(char));
    if (mensagem_decriptada == NULL)
    {
        printf("Erro: falha ao alocar memoria!\n");
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
                printf("Erro: falha ao realocar memoria!\n");
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

    printf("\nMensagem desencriptada:\n%s\n", mensagem_decriptada);

    FILE *arquivo_saida = fopen("mensagem_desencriptada.txt", "w");
    if (arquivo_saida != NULL)
    {
        fprintf(arquivo_saida, "%s", mensagem_decriptada);
        fclose(arquivo_saida);
        printf("\nA mensagem tambem foi salva em 'mensagem_desencriptada.txt'\n");
    }

    free(mensagem_decriptada);
}


  // MENU PRINCIPAL
  

int main(void)
{
    int opcao;

    do
    {
        printf("\n=============================\n");
        printf("        RSA - MENU\n");
        printf("=============================\n");
        printf("1. Gerar e salvar chave publica\n");
        printf("2. Encriptar mensagem\n");
        printf("3. Desencriptar mensagem\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1)
            gerar_chave_publica();
        else if (opcao == 2)
            encriptar();
        else if (opcao == 3)
            desencriptar();
        else if (opcao != 0)
            printf("Opcao invalida!\n");

    } while (opcao != 0);

    printf("Encerrando...\n");
    return 0;
}
