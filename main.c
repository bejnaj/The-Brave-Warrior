#include <stdio.h>
#include <stdbool.h>

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
    vacio,
    enemigo,
    item,
    jefe
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
    int vidaRecuperada
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
} Skill;

typedef struct {
    int id;
    tipoStatus tipo;
    operacion op;
    float cantidad;
    int costeTurnos;
} Status;

int main {
    return 1;
}