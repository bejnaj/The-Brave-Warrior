#include <stdio.h>
#include <stdbool.h>
#include <tdas/list.h>
#include <tdas/extra.h>
#include <tdas/map.h>
#include <stdlib.h>

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

Escenario **crearMatriz(Map *mapaItems, Map *mapaEnemigos, Jugador *P, Map *mapaJefes) {
    Escenario **matriz = malloc(sizeof(Escenario *) * 5); // Se almacena memoria para cada Escenario *
    for (int i = 0 ; i < 5 ; i++) { // Se recorren todos los elementos del Escenario *
        matriz[i] = malloc(sizeof(Escenario) * 5); // Se almacena memoria para cada Escenario **
        for (int j = 0 ; j < 5 ; j++) {
            matriz[i][j].tipo = rand() % 3; // Tipo random entre 0-2
            switch (matriz[i][j].tipo) { // Depende del tipo seleccionado
                case VACIO:
                    matriz[i][j].enemigo = NULL;
                    matriz[i][j].objeto = NULL;
                    break;

                case ENEMIGO:
                    matriz[i][j].enemigo = clonarEnemigo(mapaEnemigos, P);
                    matriz[i][j].objeto = NULL;
                    break;
            
                case ITEM:
                    matriz[i][j].objeto = clonarItem(mapaItems, P);
                    matriz[i][j].enemigo = NULL;
                    break;
            }
            // Se colocan posicion y visitado en defecto
            matriz[i][j].coords.posX = i;
            matriz[i][j].coords.posY = j;
            matriz[i][j].visitado = false;
        }
    }
    // Una vez terminados todos los escenarios, elegir el jefe
    int aux = rand() % 25; // Obtener numero random entre 0-24
    int indX = aux / 5;
    int indY = aux % 5;
    matriz[indX][indY].tipo = JEFE;
    matriz[indX][indY].enemigo = clonarJefe(mapaJefes, P);
    matriz[indX][indY].objeto = NULL;
}

int main() {

    return 1;
}