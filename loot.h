#ifndef LOOT_H
#define LOOT_H
#include "tdas/list.h"
#include "tipoDato.h"
#include <stdbool.h>

// asigna loot aleatorio a un enemigo segun la lista de items
void asignarLootAleatorio(Enemy *enemigo, List *listaItems);

#endif 