#include "desencriptacao.h"

long long calcular_mdc(long long a, long long b) 
{
    while (b != 0) 
    {
        long long temp = b;
        b = a % b;
        a = temp;
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
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;
    return x1;
}

long long calcular_exponenciacao_modular(long long base, long long exp, long long mod) 
{
    long long resultado = 1;
    base = base % mod;
    
    while (exp > 0) 
    {
        if (exp % 2 == 1)
            resultado = (resultado * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    
    return resultado;
}

void desencriptar(long long p, long long q, long long e) 
{
    long long n = p * q;
    long long phi = (p - 1) * (q - 1);
    
    long long d = calcular_inverso_modular(e, phi);    
    
    printf("Chave privada d calculada: %lld\n", d);
    
    char nome_arquivo[100];
    printf("Digite o nome do arquivo com a mensagem encriptada: ");
    scanf("%s", nome_arquivo);
    
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) 
    {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    
    char mensagem_decriptada[1000] = "";
    long long numero_encriptado;
    
    while (fscanf(arquivo, "%lld", &numero_encriptado) == 1) 
    {
        long long numero_decriptado = calcular_exponenciacao_modular(numero_encriptado, d, n);
        
        char caractere = (char)numero_decriptado;
        strncat(mensagem_decriptada, &caractere, 1);
    }
    
    fclose(arquivo);
    
    FILE *saida = fopen("mensagem_desencriptada.txt", "w");
    fprintf(saida, "%s", mensagem_decriptada);
    fclose(saida);
    
    printf("Mensagem desencriptada com sucesso!\n");
    printf("Mensagem: %s\n", mensagem_decriptada);
    printf("Salva em: mensagem_desencriptada.txt\n");
}
