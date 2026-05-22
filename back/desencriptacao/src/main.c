#include <stdio.h>
#include "desencriptacao.h"

int main(void)
{
    long long p, q, e;

    printf("=== DESENCRIPTAÇÃO RSA ===\n\n");
    printf("Digite o número primo p: ");
    scanf("%lld", &p);
    
    printf("Digite o número primo q: ");
    scanf("%lld", &q);
    
    printf("Digite o expoente e: ");
    scanf("%lld", &e);
    
    long long phi = (p - 1) * (q - 1);
    if (calcular_mdc(e, phi) != 1) 
    {
        printf("Erro: e não é relativamente primo a (p-1)(q-1)!\n");
        return 1;
    }
    
    desencriptar(p, q, e);
    
    return 0;
}
