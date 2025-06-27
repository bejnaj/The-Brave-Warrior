#ifndef COMBATE_H
#define COMBATE_H

#include "tipoDato.h"
#include <stdbool.h>

// Imprime texto animado, igual que printf pero mostrando cada carácter uno a uno.
void printb(const char *fmt, ...);

// Imprime barras de vida y defensa para el jugador y el enemigo.
void printearBarras(Jugador *P, Enemy *E);

// Borra x líneas en la consola.
void borrarLineas(int x);

// Calcula el daño infligido considerando la defensa del enemigo.
int Ataque(int dano, int defensaEnemiga, int fullVida);

// Borra la primera ocurrencia de un consumible en el inventario.
void borrarConsumible(List *I, char *nmb);

// Usa un objeto del inventario (solo consumibles).
int usarObjeto(Jugador *P);

// Intenta huir del combate.
bool intentarHuir(Jugador *P, Enemy *E);

// Menú de pelea del jugador.
void Pelear(Jugador *P, Enemy *E);

// Inicia el combate entre el jugador y el enemigo.
int comenzarPelea(Jugador *P, Enemy *E);

#endif // COMBATE_H