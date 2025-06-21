#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Inicializa la semilla del generador de números aleatorios (solo una vez)
void init_random() {
    srand((unsigned int)time(NULL));
}

// Devuelve un número aleatorio entre min y max (ambos incluidos)
int randomRint(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Devuelve un número aleatorio entre min y max para decimales
double randomRdouble(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}
// Devuelve un número entero aleatorio con variación porcentual (por ejemplo, 50% de 100 es entre 50 y 150)
int randomVint(int base, double percent) {
    int min = (int)round(base * (1.0 - percent / 100.0));
    int max = (int)round(base * (1.0 + percent / 100.0));
    return randomRint(min, max);
}

// Devuelve un número aleatorio con variación porcentual (por ejemplo, 50% de 100 es entre 50 y 150)
double randomVdouble(double base, double percent) {
    double min = base * (1.0 - percent / 100.0);
    double max = base * (1.0 + percent / 100.0);
    return randomRdouble(min, max);
}