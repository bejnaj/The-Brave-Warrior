#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "multimapaItems.h"
#include "../tipoDato.h"
#include "../balance.h"


multiPar *crearMultiPar(int key, void * value) { 
    multiPar *new = (multiPar *)malloc(sizeof(multiPar));
    new -> key = key;
    new -> values = list_create();
    list_pushBack(new -> values, value);
    return new;
}


void insertarMultiMapa(multiMapa *map, int key, void *value) { // Usando poder del item como key
    int keyHash = hash(key, map -> capacity);
    if (map -> pares[keyHash] == NULL || map -> pares[keyHash] -> values == NULL) { // Si no existe
        map -> pares[keyHash] = crearMultiPar(keyHash, value);
        map -> current = keyHash;
        (map -> size)++;
    }
    else { // Si existe
        list_pushBack(map -> pares[keyHash] -> values, value); // Agrega al multimapa
    }
    if ((double) map -> size / map -> capacity >= 0.7) agrandarMapa(map);
}

void agrandarMapa(multiMapa * map) {
    multiPar **arrayAntiguo = map -> pares;
    (map -> capacity) *= 2;
    map -> size = 0;
    map -> current = -1;
    multiPar **arrayNuevo = calloc(map -> capacity, sizeof(multiPar *));
    map -> pares = arrayNuevo;
    for(int i = 0 ; i < (map -> capacity)/2 ; i++) {
        if(arrayAntiguo[i] != NULL && arrayAntiguo[i] -> key != NULL) {
            for (void *actual = list_first(arrayAntiguo[i]) ; actual != NULL ; actual = list_first(arrayAntiguo[i])) {
                Item *elem = list_popFront(arrayAntiguo[i]);
                insertarMultiMapa(map, elem -> poder, elem);
            }
            arrayAntiguo[i] = NULL;
        }
    }
    free(arrayAntiguo);
    arrayAntiguo = NULL;
}


multiMapa *crearMultiMapa(long capacity) {
    multiMapa *mapa = malloc(sizeof(multiMapa));
    mapa -> pares = calloc(capacity, sizeof(multiPar *));
    mapa -> size = 0;
    mapa -> capacity = capacity;
    mapa -> current = -1;
    return mapa;
}

multiPar *buscarMultiMapa(multiMapa *map, int key) {  
    int pos = hash(key, map -> capacity);
    multiPar *par = map->pares[pos];

    if (par != NULL && (par -> key == key)) {
        map -> current = pos;
        return par;
    }
    return NULL;
}

multiPar *primerElemMultiMapa(multiMapa *map) {
    map -> current = -1;
    for (int i = 0 ; i < map -> capacity ; i++) {
        if (map -> pares[i] != NULL) {
            map -> current = i;
            return map -> pares[i];
        }
    }
    return NULL;
}

multiPar *siguienteMultiMapa(multiMapa *map) {
    if (map -> current == -1) return NULL;
    int pos = (map -> current) + 1; 
    while (pos < map -> capacity) {
        if (map -> pares[pos] != NULL) {
            map -> current = pos;
            return map -> pares[pos];
        }
        pos++;
    }
    return NULL;
}
