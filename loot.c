#include "random.h"
#include <stdbool.h>
#include "tdas/list.h"
#include "tipoDato.h"

bool itemYaEnLista(List *lista, Item *item) {
    Node *nodo = lista->head;
    while (nodo) {
        if (nodo->data == item) return true;
        nodo = nodo->next;
    }
    return false;
}

void asignarLootAleatorio(Enemy *enemigo, List *listaItems) {
    if (!enemigo || !listaItems || list_size(listaItems) == 0) return;

    int chance = randomRint(1, 100); // genera numero aleatorio entre 1 y 100 para determinar si no deja loot (20% probabilidad)
    if (chance <= 20) {
        enemigo->loot = NULL; // no deja loot
        return;
    }

    enemigo->loot = list_create(); // crea una lista para el loot del enemigo
    int cantidadLoot = randomRint(1, 3); // decide cuanto loot soltara el enemigo (entre 1 y 3 items)
    int totalItems = list_size(listaItems);
    int intentosMax = 100; // limitar la cantidad de intentos para evitar bucles infinitos

    for (int i = 0; i < cantidadLoot && intentosMax > 0; ) {  // asignar items unicos al loot hasta completar la cantidad o agotar intentos
        int index = randomRint(0, totalItems - 1);
        Item *item = list_get(listaItems, index);

        if (!itemYaEnLista(enemigo->loot, item)) { // si el item no esta en el loot, lo agrega y aumenta el contador
            list_pushBack(enemigo->loot, item);
            i++;
        }

        intentosMax--;
    }
}