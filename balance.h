#ifndef BALANCE_H
#define BALANCE_H

// Obtiene el poder bruto de un item segun sus stats
int powerIndexItems(Item *I); 

// Obtiene el poder aproximado de un jugador segun su nivel
int powerIndexPlayer(Jugador *P);

// Obtiene la posicion de un elemento en un mapa, llendo de valores de 1 a valorMAX
int hash(int clave, int valorMAX); 

// Funcion inversa de hash
int unhash(int hashValor, int indiceMAX);
#endif