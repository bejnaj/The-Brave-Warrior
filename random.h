#ifndef RANDOM_H
#define RANDOM_H

//Para usar randoms:
// Inicializa la semilla en la que se basarán TODOS los números randoms con:
//      void init_random();
// IMPORTANTE: DEBES USARLA AL INICIO DEL MAIN 1 SOLA VEZ

void init_random();
int randomRint(int min, int max);               // número random entero de [min] a [max]
double randomRdouble(double min, double max);   // número random decimal de [min] a [max]
int randomVint(int base, double perc);          // número random entero de base [base] con variación [perc]
double randomVdouble(double base, double per);  // número random entero de base [base] con variación [perc]

#endif