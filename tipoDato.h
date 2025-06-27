#ifndef TIPOS_H
#define TIPOS_H
#include <stdbool.h>
#include "tdas/list.h"

//// ENUMS

typedef enum {
    noEquipable,
    ARMA,
    ARMADURA
} tipoEquipable;

typedef enum {
    noConsumible,
    libroDeHabilidad,
    tipoPocion
} tipoConsumible;

typedef enum {
    curacion,
    estado
} tipoSkill;

typedef enum {
    vida,
    dano,
    defensa,
    saltarTurno
} tipoStatus;

typedef enum {
    suma,
    multiplicacion
} operacion;

typedef enum {
    VACIO,
    ENEMIGO,
    ITEM,
    JEFE
} tipoEvento;

//// STRUCTS

typedef struct {
    int vida;
    int ataque;
    int defensa;
} Stats;

typedef struct {
    int id;
    char *nombre;
    tipoStatus tipo;
    operacion op;
    float cantidad;
    int costeTurnos;
    int duracion;
} Status;

typedef struct {
    char *nombre;
    int cooldown;
    int cooldownActual;
    int duracion;
    tipoSkill tipo;
    int vidaCurada;
    Status *estado;
    int hacia;
} Skill;

typedef struct {
    char *nombre;
    char *descripcion;
    tipoConsumible tipoCons;
    tipoEquipable tipoEquip;
    Stats statBonus;
    Skill *habilidadAprendida;
    int vidaRecuperada;
    int poder;
} Item;

typedef struct {
    char *nombre;
    int vida;
    int vidaActual;
    int ataque;
    int defensa;
    Item *loot;
    Item *arma;
    bool esJefe;
    Status *efecto;
    Skill *habilidades[3];
} Enemy;

typedef struct {
    int posX;
    int posY;
} Punto;

typedef struct {
    char *nombre;
    int vida;
    int vidaActual;
    Item *arma;
    Item *armadura;
    int ataque;
    int defensa;
    Stats statsBase;
    int nivel;
    int xp;
    List *inventario;
    Skill *habilidades[2];
    Status *efecto;
    Punto posicion;
} Jugador;

typedef struct {
    tipoEvento tipo;
    Enemy *enemigo;
    Item *objeto;
    Punto coords;
    bool visitado;
} Escenario;

#endif // TIPOS_H