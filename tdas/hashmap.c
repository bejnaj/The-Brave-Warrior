#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

unsigned int hash(const char *str, unsigned int rango) {
    return atoi(str) % rango;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {    
    int pos = hash(key, map -> capacity);
    if (map -> buckets[pos] == NULL || map -> buckets[pos] -> key == NULL) {
        map -> buckets[pos] = createPair(key, value);
    }
    else {
        while (map -> buckets[pos] != NULL) {
            if (strcmp(map -> buckets[pos] -> key, key) == 0) return;
            pos = (pos + 1) % (map -> capacity);
        }
        map -> buckets[pos] = createPair(key, value);
    }
    (map -> size)++;
    return;
}

void enlarge(HashMap * map) {
    Pair **arrayAntiguo = map -> buckets;
    (map -> capacity) *= 2;
    map -> size = 0;
    map -> current = -1;
    Pair **arrayNuevo = calloc(map -> capacity, sizeof(Pair *));
    map -> buckets = arrayNuevo;
    for(int i = 0 ; i < (map -> capacity)/2 ; i++) {
        if(arrayAntiguo[i] != NULL && arrayAntiguo[i] -> key != NULL) {
            insertMap(map, arrayAntiguo[i] -> key, arrayAntiguo[i] -> value);
            free(arrayAntiguo[i]);
        }
    }
    free(arrayAntiguo);
}


HashMap *createMap(long capacity) {
    HashMap *mapa = malloc(sizeof(HashMap));
    mapa -> buckets = calloc(capacity, sizeof(Pair));
    mapa -> size = 0;
    mapa -> capacity = capacity;
    mapa -> current = -1;
    return mapa;
}

void eraseMap(HashMap * map,  char * key) {    
    int pos = hash(key, map -> capacity);
    while (map -> buckets[pos] != NULL) {
        if (strcmp(map -> buckets[pos] -> key,key) == 0) {
            map -> buckets[pos] -> key = NULL;
            (map -> size)--;
            return;
        }
        pos = (pos + 1) % map -> capacity;
    }   
}

Pair *searchMap(HashMap * map,  char * key) {  
    int pos = hash(key, map -> capacity);
    while (map -> buckets[pos] != NULL) {
        if (strcmp(map -> buckets[pos] -> key, key) == 0) {
            map -> current = pos;
            return map -> buckets[pos];
        }
        pos = (pos + 1) % map -> capacity;
    }
    return NULL;
}

Pair *firstMap(HashMap *map) {
    for (int i = 0 ; i < map -> capacity ; i++) {
        if (map -> buckets[i] != NULL && map -> buckets[i] -> key != NULL) {
            map -> current = i;
            return map -> buckets[i];
        }
    }
    return NULL;
}

Pair *nextMap(HashMap *map) {
    if (map -> current == -1) return NULL;
    int pos = (map -> current) + 1;
    while (pos < map -> capacity) { 
        if (map -> buckets[pos] != NULL && map -> buckets[pos] -> key != NULL) {
            map -> current = pos;
            return map -> buckets[pos];
        }
        pos++;
    }
    return NULL;
}
