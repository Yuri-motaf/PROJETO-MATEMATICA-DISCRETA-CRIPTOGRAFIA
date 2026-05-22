#ifndef DESENCRIPTACAO_H
#define DESENCRIPTACAO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long long calcular_mdc(long long a, long long b);

long long calcular_inverso_modular(long long a, long long m);

long long calcular_exponenciacao_modular(long long base, long long exp, long long mod);

void desencriptar(long long p, long long q, long long e);

#endif
