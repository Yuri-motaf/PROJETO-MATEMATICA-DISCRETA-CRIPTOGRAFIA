#include <stdio.h>

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


int main()
{
    long long primeiro_primo, segundo_primo;
    
    printf("Vamos salvar sua chave publica! Digite o primeiro numero primo: ");
    scanf("%lld", &primeiro_primo);
    
    printf("Agora o segundo numero primo: ");
    scanf("%lld", &segundo_primo);

    long long possivel_expoente = (primeiro_primo - 1) * (segundo_primo - 1);
    long long i;
    printf("Otimo, agora defina um expoente 'e' coprimo e menor que %lld, de MDC=1. Possibilidades abaixo:\n", possivel_expoente);
   
    for(i = 2; i < possivel_expoente; i++)
    {
        if (mdc(i, possivel_expoente) == 1)
        {
            printf("%lld\n", i);
        }
    }

    long long verdadeiro_expoente;
    scanf("%lld", &verdadeiro_expoente);

    long long n = primeiro_primo * segundo_primo;

    FILE *arquivo = fopen("chave_publica.txt", "w");
    fprintf(arquivo, "%lld %lld", n, verdadeiro_expoente);
    fclose(arquivo);

    return 0;
}