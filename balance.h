#ifndef BALANCE_H
#define BALANCE_H

// Obtiene el poder bruto de un arma
int powerIndex(Item *I); 

// Obtiene la posicion de un elemento en un mapa, llendo de valores de 1 a valorMAX
int hash(int clave, int valorMAX); 

// Funcion inversa de hash
int unhash(int hashValor, int indiceMAX);
#endif