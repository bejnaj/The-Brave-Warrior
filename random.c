#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Inicializa la semilla del generador de números aleatorios (solo una vez)
void init_random() {
    srand((unsigned int)time(NULL));
}

// Devuelve un número aleatorio entre min y max para ENTEROS
int randomRint(int min, int max) {
    for(size_t i = 0; i<2; i++){
        return min + rand() % (max - min + 1);
    }
}

// Devuelve un número aleatorio entre min y max para DECIMALES
double randomRdouble(double min, double max) {
    for(size_t i = 0; i<2; i++){
        return min + ((double)rand() / RAND_MAX) * (max - min);
    }
}
// Devuelve un número aleatorio con variación porcentual PARA ENTEROS
int randomVint(int base, double percent) {
    int min = (int)(base * (1.0 - percent / 100.0));
    int max = (int)(base * (1.0 + percent / 100.0));
    for(size_t i = 0; i<2; i++){
        return randomRint(min, max);
    }
}

// Devuelve un número aleatorio con variación porcentual PARA DECIMALES
double randomVdouble(double base, double percent) {
    double min = base * (1.0 - percent / 100.0);
    double max = base * (1.0 + percent / 100.0);
    for(size_t i = 0; i<2; i++){
        return randomRdouble(min, max);
    }
}