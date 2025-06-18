#define TIPOS_H
#include <stdbool.h>
#include <tdas/list.h>
#include <tdas/map.h>
#include <tdas/extra.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    noEquipable,
    arma,
    armadura
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
    daño,
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

typedef struct {
    int vidaBonus;
    int AtaqueBonus;
    int DefensaBonus;
} Bonus;

typedef struct {
    char *nombre;
    char *descripcion;
    tipoConsumible tipoCons;
    tipoEquipable tipoEquip;
    Bonus statBonus;
    int vidaRecuperada;
} Item;

typedef struct {
    char *nombre;
    int vida;
    int vidaActual;
    int ataque;
    int defensa;
    List *loot;
    Item *arma;
    bool esJefe;
    List *estado;
    List *habilidades;
} Enemy;

typedef struct {
    char *nombre;
    int cooldown;
    int duracion;
    tipoSkill tipo;
    int vidaCurada;
    List *estado;
    int hacia;
} Skill;

typedef struct {
    int id;
    tipoStatus tipo;
    operacion op;
    float cantidad;
    int costeTurnos;
} Status;

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
    int nivel;
    List *inventario;
    List *habilidades;
    List *efectos;
    Punto posicion;
} Jugador;

typedef struct {
    tipoEvento tipo;
    Enemy *enemigo;
    Item *objeto;
    Punto coords;
    bool visitado;
} Escenario;

