#ifndef multimapaItems_h
#define multimapaItems_h
#include "list.h"

typedef struct multiMapa multiMapa;


typedef struct multiPar {
     int key;
     List *values;
} multiPar;

struct multiMapa {
    multiPar **pares;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

multiPar *crearMultiPar(int key, void *value);

multiMapa *crearMultiMapa(long capacity);

void insertarMultiMapa(multiMapa * table, int key, void * value);

void limpiarMapa(multiMapa * table);

void sizeDown(multiMapa* map);

multiPar *buscarMultiMapa(multiMapa * table, int key);

multiPar *primerElemMultiMapa(multiMapa * table);

multiPar *siguienteMultiMapa(multiMapa * table);

multiPar *anteriorMultiMapa(multiMapa *map);

void agrandarMapa(multiMapa * map);

#endif
