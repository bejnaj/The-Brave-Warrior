#include "tipoDato.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>
#include <math.h>
#include "random.h"
#include "miscelaneo.h"
#include "tdas/extra.h"

const char* tipoToStr(tipoStatus t) {
    switch (t) {
        case dano: return "daño";
        case defensa: return "defensa";
        case saltarTurno: return "pierdes turno";
        case vida: return "vida";
    }
}

const char* tipoSkillStr(tipoSkill tipo) {
    switch (tipo) {
        case curacion: return "Curación";
        case estado: return "Estado";
        default: return "Desconocido";
    }
}

Status* copiarStatus(Status* original) {
    Status* copia = malloc(sizeof(Status));
    *copia = *original;
    copia->nombre = strdup(original->nombre);
    return copia;
}

void printearBarras(Jugador *P, Enemy *E){
    limpiarPantalla();
    printf("\n\n\nElige una opción...Sin titubear!!\n1) Pelear\n2) Objetos\n3) Huir\n\n");
    char vidatuya[P->vida + 1],vidaenemiga[E->vida + 1]; // +1 para el carácter nulo '\0'
    vidatuya[0] = '\0';
    vidaenemiga[0] = '\0';
    if (P->vidaActual > P->vida) P->vidaActual = P->vida;
    if (E->vidaActual > E->vida) E->vidaActual = E->vida;
    int barrastuya = P->vidaActual / 10;
    int barrasenemiga = E->vidaActual / 10;
    for (int i = 0; i < barrastuya; i++) {
        strcat(vidatuya, "|");
    }
    for (int i = 0; i < barrasenemiga; i++) {
        strcat(vidaenemiga, "|");
    }
    if ((P->habilidades[0] && P->habilidades[0]->cooldownActual > 0) ||
        (P->habilidades[1] && P->habilidades[1]->cooldownActual > 0)) {
        if (P->habilidades[0] && P->habilidades[0]->cooldownActual > 0)
            printf(" (%s: quedan %d turnos)", P->habilidades[0]->nombre, P->habilidades[0]->cooldownActual);
        if (P->habilidades[1] && P->habilidades[1]->cooldownActual > 0)
            printf(" (%s: quedan %d turnos)", P->habilidades[1]->nombre, P->habilidades[1]->cooldownActual);
    }
    printf("\n\n");
    printf("Tu Vida: %s (%d/%d) Defensa: %d", vidatuya, P->vidaActual, P->vida, P->defensa);
    if (P->efecto != NULL && P->efecto->duracion > 0) {
        printf(" %s", P->efecto->nombre);
        if (P->efecto->op == multiplicacion) printf(" (%sx%.1f)",tipoToStr(P->efecto->tipo),P->efecto->cantidad);
        if (P->efecto->op == suma){
            if (P->efecto->cantidad>0) printf(" (+%.0f %s)",P->efecto->cantidad,tipoToStr(P->efecto->tipo));
            if (P->efecto->cantidad<0) printf(" (-%.0f %s)",P->efecto->cantidad,tipoToStr(P->efecto->tipo));
        }
    }
    printf("\n\n");
    printf("Enemigo: %s (%d/%d) Defensa: %d", vidaenemiga, E->vidaActual, E->vida, E->defensa);
    if (E->efecto != NULL && E->efecto->duracion > 0) {
        printf(" %s", E->efecto->nombre);
        if (E->efecto->op == multiplicacion) printf(" (%sx%.1f)",tipoToStr(E->efecto->tipo),E->efecto->cantidad);
        if (E->efecto->op == suma) printf(" (+%.0f %s)",E->efecto->cantidad,tipoToStr(E->efecto->tipo));
    }
    printf("\n\n\n");
}

// Calcula el daño infligido considerando la defensa del enemigo
int Ataque(int dano, int defensaEnemiga, int fullVida) {
    double vidaTeorica = fullVida + defensaEnemiga;
    double porcentaje = (double)dano / vidaTeorica;
    int resultado = (int)round(porcentaje * fullVida);
    if (resultado < 0) resultado = 0;
    return resultado;
}

// Borra la primera ocurrencia de un consumible en el inventario
void borrarConsumible(List *I, char *nmb) {
    for (Item *actual = list_first(I); actual != NULL ; actual = list_next(I)) {
        if (strcmp(actual -> nombre, nmb) == 0) {
            list_popCurrent(I);
            Sleep(500);
            break;
        }
    }
}

// Usa un objeto del inventario (solo consumibles)
int usarObjeto(Jugador *P,Enemy*E) {
    // RETURNS
    // 0: Curacion normal, el juego sigue el transcurso normal
    // 1: Muerte, fin del juego
    // 2: No considerado ya que no poseia items suficientes
    List *L = P -> inventario;
    if (list_first(L) == NULL) {
        printb("No tienes items en tu inventario.\n");
        Sleep(2000);

        return 2;
    }
    int max = 20;
    Item **arrayTemp = malloc(sizeof(Item *) * max); // Crea el array con los objetos consumibles
    int i = 0;
    for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) { // Recorre el inventario
        if (actual -> tipoCons == tipoPocion) { // Si es un objeto consumible, se agrega al array y se muestra
            arrayTemp[i] = actual;
            i++;
            printf("%d) %s\n", i, actual -> nombre);
        }
        if (i == max) { // Si el array se llena, se redimensiona
            max *= 2;
            arrayTemp = realloc(arrayTemp, sizeof(Item *) * max);
            if (arrayTemp == NULL)
                exit(EXIT_FAILURE);
        }
    }
    if (i == 0) { // Si el array quedo vacio (no habian consumibles)
        printearBarras(P,E);
        printf("Es tu turno\n\n");
        printb("\nNo tienes items consumibles.");
        Sleep(1500);
        return 2;
    }

    int num;
    printb("Elige que item usar: \n");
    verificarOpcionConBorrado(&num, i);
    num -= 1;

    if(arrayTemp[num] -> vidaRecuperada > 0) { // Verifica si el item curaba o restaba vida
        printearBarras(P,E);
        printf("Es tu turno\n\n");
        printb("Te has recuperado %d puntos de vida\n", arrayTemp[num] -> vidaRecuperada);
        Sleep(1500);
    } else { 
        if (arrayTemp[num] -> vidaRecuperada < 0) {
            printearBarras(P,E);
            printf("Es tu turno\n\n");
            printb("Has perdido %d puntos de vida\n", abs(arrayTemp[num] -> vidaRecuperada));
            Sleep(1500);
        }
        else {
            printearBarras(P,E);
            printf("Es tu turno\n\n");
            printb("No ocurrio nada...\n");
            Sleep(1500);
        }
    }
    P -> vidaActual += arrayTemp[num] -> vidaRecuperada; // Agregar la vida
    if (P -> vidaActual > P -> vida) {
        P -> vidaActual = P -> vida;
    }
    else
        if (P -> vidaActual <= 0) {
            P -> vidaActual = 0;
            return 1;
        }
    borrarConsumible(P -> inventario, arrayTemp[num] -> nombre);
    return 0;
}

bool intentarHuir(Jugador *P, Enemy *E){
    borrarLineas(1);
    double porcentajeVida = (double)E->vidaActual / E->vida; // entre 0 y 1
    int probabilidad = 20 + (int)((1.0 - porcentajeVida) * 70); // 20% a 90%
    if (probabilidad > 90) probabilidad = 90;
    if (probabilidad < 20) probabilidad = 20;
    printb("Probabilidad de escape: %d%%",probabilidad);
    for (size_t i= 0; i<3;i++){
        Sleep(500);
        printf(".");
    }
    Sleep(500);
    printf("\n");
    printb("Intentas escapar");
    for (size_t i= 0; i<3;i++){
        Sleep(500);
        printf(".");
    }
    Sleep(500);
    printf("\n");
    Sleep(1000);
    //borrarLineas(2);
    if(randomRint(1,100)<=probabilidad){
        printf("¡¡¡Escapas exitosamente!!!");
        Sleep(4000);
        borrarLineas(4);
        return true;
    } else{
        borrarLineas(2);
        printb("NO LOGRAS ESCAPAR!!\n");
        Sleep(500);
        printb("PIERDES EL TURNO!!\n\n");
        Sleep(1000);
        borrarLineas(3);
        return false;
    }
}

void Habilidad(Jugador *P, Enemy *E, int h, int lanzaSkill) {
    Skill *skill = NULL;
    if (lanzaSkill == 1) { // Jugador lanza la habilidad
        skill = P->habilidades[h];
        if (!skill) return;
        skill->cooldownActual = skill->cooldown;
        if (skill->tipo == curacion) {            // CURACION
            if (skill->hacia == 1) { // Uno mismo (JUGADOR)
                P->vidaActual += skill->vidaCurada;
                if (P->vidaActual > P->vida) P->vidaActual = P->vida;
                printearBarras(P,E);
                printf("Es tu turno\n\n");
                printb("%s se ha curado %d puntos de vida.\n", P->nombre, skill->vidaCurada);
                Sleep(2000);
            } else {                // Al enemigo (JUGADOR)
                E->vidaActual += skill->vidaCurada;
                if (E->vidaActual > E->vida) E->vidaActual = E->vida;
                printearBarras(P,E);
                printf("Es tu turno\n\n");
                printb("%s ha usado %s a contra %s e infligió %d puntos de vida.\n", P->nombre,skill->nombre, E->nombre, skill->vidaCurada);
                Sleep(2000);
            }
        } else if (skill->tipo == estado) {       // ESTADO
            if (skill->hacia == 1) { // Uno mismo (JUGADOR)
                if (P->efecto){ // devuelve valores predeterminados y NULLea y reemplaza efecto anterior como en pokemon
                    if (P->efecto->op == suma){
                        if (P->efecto->tipo==defensa) P->defensa -= P->efecto->cantidad;
                        if (P->efecto->tipo==dano) P->ataque -= P->efecto->cantidad;
                    }
                    if (P->efecto->op== multiplicacion){
                        if (P->efecto->tipo==vida) P->vida = (int)(P->vida / P->efecto->cantidad);
                        if (P->efecto->tipo==defensa) P->defensa = (int)(P->defensa / P->efecto->cantidad);
                        if (P->efecto->tipo==dano) P->ataque = (int)(P->ataque / P->efecto->cantidad);
                    }
                    free(P->efecto->nombre);
                    free(P->efecto);
                    P->efecto = NULL;
                }
                P->efecto = copiarStatus(skill->estado); // se copia en el objetivo el efecto
                P->efecto->duracion = skill->duracion;   // se agrega la duracion al efecto
                if (P->efecto->op == suma){              // se suman/multiplican los bonuses adecuadamente
                    if (P->efecto->tipo==defensa) P->defensa += P->efecto->cantidad;
                    if (P->efecto->tipo==dano) P->ataque += P->efecto->cantidad;
                }
                if (P->efecto->op== multiplicacion){
                    if (P->efecto->tipo==vida) P->vida = (int)(P->vida * P->efecto->cantidad);
                    if (P->efecto->tipo==defensa) P->defensa = (int)(P->defensa * P->efecto->cantidad);
                    if (P->efecto->tipo==dano) P->ataque = (int)(P->ataque * P->efecto->cantidad);
                }
                printearBarras(P,E);
                printf("Es tu turno\n\n");
                printb("%s ahora tiene el estado: %s\n", P->nombre, skill->estado->nombre);
                Sleep(2000);
            } else {                // Al enemigo (JUGADOR)
                if (E->efecto){ // devuelve valores predeterminados y NULLea y reemplaza efecto anterior como en pokemon
                    if (E->efecto->op == suma){
                        if (E->efecto->tipo==defensa) E->defensa -= E->efecto->cantidad;
                        if (E->efecto->tipo==dano) E->ataque -= E->efecto->cantidad;
                    }
                    if (E->efecto->op== multiplicacion){
                        if (E->efecto->tipo==vida) E->vida = (int)(E->vida / E->efecto->cantidad);
                        if (E->efecto->tipo==defensa) E->defensa = (int)(E->defensa / E->efecto->cantidad);
                        if (E->efecto->tipo==dano) E->ataque = (int)(E->ataque / E->efecto->cantidad);
                    }
                    free(E->efecto->nombre);
                    free(E->efecto);
                    E->efecto = NULL;
                }
                E->efecto = copiarStatus(skill->estado); // se copia en el objetivo el efecto
                E->efecto->duracion = skill->duracion;   // se agrega la duracion al efecto
                if (E->efecto->op == suma){              // se suman/multiplican los bonuses adecuadamente
                    if (E->efecto->tipo==defensa) E->defensa += E->efecto->cantidad;
                    if (E->efecto->tipo==dano) E->ataque += E->efecto->cantidad;
                }
                if (E->efecto->op== multiplicacion){
                    if (E->efecto->tipo==vida) E->vida = (int)(E->vida * E->efecto->cantidad);
                    if (E->efecto->tipo==defensa) E->defensa = (int)(E->defensa * E->efecto->cantidad);
                    if (E->efecto->tipo==dano) E->ataque = (int)(E->ataque * E->efecto->cantidad);
                }
                printearBarras(P,E);
                printf("Es tu turno\n\n");
                printb("%s ha aplicado el estado %s a %s\n", P->nombre, skill->estado->nombre, E->nombre);
                Sleep(2000);
            }
        }
    } else { // Enemigo lanza la habilidad
        skill = E->habilidades[h];
        skill->cooldownActual = skill->cooldown;
        if (skill->tipo == curacion) {            // CURACION
            if (skill->hacia == 1) { // Uno mismo (ENEMIGO)
                E->vidaActual += skill->vidaCurada;
                if (E->vidaActual > E->vida) E->vidaActual = E->vida;
                printearBarras(P,E);
                printf("Turno del enemigo\n\n");
                printb("%s se ha curado %d puntos de vida.\n", E->nombre, skill->vidaCurada);
                Sleep(2000);
            } else {                // Al enemigo (ENEMIGO)
                P->vidaActual += skill->vidaCurada;
                if (P->vidaActual > P->vida) P->vidaActual = P->vida;
                printearBarras(P,E);
                printf("Turno del enemigo\n\n");
                printb("%s ha curado a %s %d puntos de vida.\n", E->nombre, P->nombre, skill->vidaCurada);
                Sleep(2000);
            }
        } else if (skill->tipo == estado) {      // ESTADO
            if (skill->hacia == 1) { // Uno mismo (ENEMIGO)
                if (E->efecto){ // devuelve valores predeterminados y NULLea y reemplaza efecto anterior como en pokemon
                    if (E->efecto->op == suma){
                        if (E->efecto->tipo==defensa) E->defensa -= E->efecto->cantidad;
                        if (E->efecto->tipo==dano) E->ataque -= E->efecto->cantidad;
                    }
                    if (E->efecto->op== multiplicacion){
                        if (E->efecto->tipo==vida) E->vida = (int)(E->vida / E->efecto->cantidad);
                        if (E->efecto->tipo==defensa) E->defensa = (int)(E->defensa / E->efecto->cantidad);
                        if (E->efecto->tipo==dano) E->ataque = (int)(E->ataque / E->efecto->cantidad);
                    }
                    free(E->efecto->nombre);
                    free(E->efecto);
                    E->efecto = NULL;
                }
                E->efecto = copiarStatus(skill->estado); // se copia en el objetivo el efecto
                E->efecto->duracion = skill->duracion;   // se agrega la duracion al efecto
                if (E->efecto->op == suma){              // se suman/multiplican los bonuses adecuadamente
                    if (E->efecto->tipo==defensa) E->defensa += E->efecto->cantidad;
                    if (E->efecto->tipo==dano) E->ataque += E->efecto->cantidad;
                }
                if (E->efecto->op== multiplicacion){
                    if (E->efecto->tipo==vida) E->vida = (int)(E->vida * E->efecto->cantidad);
                    if (E->efecto->tipo==defensa) E->defensa = (int)(E->defensa * E->efecto->cantidad);
                    if (E->efecto->tipo==dano) E->ataque = (int)(E->ataque * E->efecto->cantidad);
                }
                printearBarras(P,E);
                printf("Turno del enemigo\n\n");
                printb("%s ahora tiene el estado: %s\n", E->nombre, skill->estado->nombre);
                Sleep(2000);
            } else {                // Al enemigo (ENEMIGO)
                if (P->efecto){ // devuelve valores predeterminados y NULLea y reemplaza efecto anterior como en pokemon
                    if (P->efecto->op == suma){
                        if (P->efecto->tipo==defensa) P->defensa -= P->efecto->cantidad;
                        if (P->efecto->tipo==dano) P->ataque -= P->efecto->cantidad;
                    }
                    if (P->efecto->op== multiplicacion){
                        if (P->efecto->tipo==vida) P->vida = (int)(P->vida / P->efecto->cantidad);
                        if (P->efecto->tipo==defensa) P->defensa = (int)(P->defensa / P->efecto->cantidad);
                        if (P->efecto->tipo==dano) P->ataque = (int)(P->ataque / P->efecto->cantidad);
                    }
                    free(P->efecto->nombre);
                    free(P->efecto);
                    P->efecto = NULL;
                }
                P->efecto = copiarStatus(skill->estado); // se copia en el objetivo el efecto
                P->efecto->duracion = skill->duracion;   // se agrega la duracion al efecto
                if (P->efecto->op == suma){              // se suman/multiplican los bonuses adecuadamente
                    if (P->efecto->tipo==defensa) P->defensa += P->efecto->cantidad;
                    if (P->efecto->tipo==dano) P->ataque += P->efecto->cantidad;
                }
                if (P->efecto->op== multiplicacion){
                    if (P->efecto->tipo==vida) P->vida = (int)(P->vida * P->efecto->cantidad);
                    if (P->efecto->tipo==defensa) P->defensa = (int)(P->defensa * P->efecto->cantidad);
                    if (P->efecto->tipo==dano) P->ataque = (int)(P->ataque * P->efecto->cantidad);
                }
                printearBarras(P,E);
                printf("Turno del enemigo\n\n");
                printb("%s ha aplicado el estado %s a %s\n", E->nombre, skill->estado->nombre, P->nombre);
                Sleep(2000);
            }
        }
    }
}

void Pelear(Jugador *P, Enemy *E){ //FALTA GESTIONAR LAS HABILIDADES, SI NO HAY ENTONCES SE INHABILITAN OPCIONES 2 U 3 DEPENDIENDO DE LA CANTIDAD.
    printf("----- Opciones de ataque -----\n");
    printb("  1) Ataque básico\n");
    int contador=1;
    int num;
    int killme = 0;
    // Mostrar habilidades (máximo 2)
    for (int i = 0; i < 2; i++) {
        if (P->habilidades[i] != NULL&&P->habilidades[i]->cooldownActual==0) {
            printb("  %d) %s (Cooldown: %d turnos)\n",
                i + 2 + killme,
                P->habilidades[i]->nombre,
                P->habilidades[i]->cooldown);
            contador++;
        } else killme = -1;
    }
    int dos = 2;
    int tres = 3;
    if ((!P->habilidades[0]) || ((P->habilidades[0]->cooldownActual > 0))) {
        dos = 3;
        tres = 2;
    }
    verificarOpcionConBorrado(&num,contador);
    
    if(num==1){
        int ataque;
        for (size_t i = 0; i<2;i++){
                ataque = randomVint(Ataque(P->ataque,E->defensa,E->vida),25);
            }
        if ((float)(ataque-(Ataque(P->ataque,E->defensa,E->vida) * 1.2))>0) ataque*=2;
        E->vidaActual -= ataque; // <-- Aplica el daño al enemigo
        printearBarras(P,E);
        printf("Es tu turno\n");
        if (ataque>(Ataque(P->ataque,E->defensa,E->vida) * 1.2)){
            printb("GOLPE CRÍTICO!!\n");
            Sleep(750);
            printb("%s ha recibido %d puntos de daño!\n",E->nombre,ataque);
            Sleep(1000);
        } else {
            printb("\n%s ha recibido %d puntos de daño!\n",E->nombre,ataque);
        }
        Sleep(2000);
        //borrarLineas(4);
    }
    if(num == dos && P->habilidades[0]) { // Habilidad 1
        Habilidad(P,E,0,1);
        printearBarras(P,E);
    }
    if(num == tres && P->habilidades[1]) { // Habilidad 2
        Habilidad(P,E,1,1);
        printearBarras(P,E);
    }
}

int comenzarPelea(Jugador *P, Enemy *E/*, Map *status*/) {
    // Ejemplo simple de combate por turnos
    printb("\n\n\n¡Te has encontrado a %s!", E->nombre);
    Sleep(1500);
    printf("\033[2K\r");// borra la linea y deja el cursor en el principio de la misma
    //printf("\033[E"); // Mueve el cursor una línea siguiente
    //printf("\033[F"); // Mueve el cursor una línea arriba
    printb("Comienza la batalla entre %s y %s...",P->nombre,E->nombre);
    Sleep(1500);
    printf("\033[2K\r"); // borra la linea y deja el cursor en el principio de la misma
    printb("Elige una opción...");
    Sleep(500);
    printb("Sin titubear!!\n");
    Sleep(500);
    printb("1) Pelear\n2) Objetos\n3) Huir\n\n");
    char vidatuya[P->vida + 1],vidaenemiga[E->vida + 1]; // +1 para el carácter nulo '\0'
    vidatuya[0] = '\0';
    vidaenemiga[0] = '\0';
    int barrastuya = P->vidaActual / 10;
    int barrasenemiga = E->vida / 10;
    for (int i = 0; i < barrastuya; i++) {
        strcat(vidatuya, "|");
    }
    for (int i = 0; i < barrasenemiga; i++) {
        strcat(vidaenemiga, "|");
    }
    printb("\n\nTu Vida: %s (%d/%d) Defensa: %d\n\n", vidatuya, P->vidaActual, P->vida, P->defensa);
    printb("Enemigo: %s (%d/%d) Defensa: %d\n\n\n", vidaenemiga, E->vidaActual, E->vida, E->defensa);
    int num,obj;
    bool escape = false;
    while (P->vidaActual > 0 && E->vidaActual > 0) {
        // Turno del jugador
        if (P->efecto && P->efecto->tipo == vida && P->efecto->op==suma) {  // liberar y poner a NULL
            P->vidaActual += (int)(P->efecto->cantidad);
            P->efecto->duracion--; // <--- REDUCE LA DURACIÓN AQUÍ
            if (P->efecto->duracion <= 0){
                free(P->efecto->nombre);
                free(P->efecto);
                P->efecto= NULL;
            }
        } else if(P->efecto && P->efecto->duracion <= 0){
            // Revertir bonus/multiplicador si corresponde
            if (P->efecto->op == suma) {
                if (P->efecto->tipo == defensa) P->defensa -= (int)(P->efecto->cantidad);
                if (P->efecto->tipo == dano) P->ataque -= (int)(P->efecto->cantidad);
            }
            if (P->efecto->op == multiplicacion) {
                if (P->efecto->tipo == vida) P->vidaActual = (int)(P->vidaActual / P->efecto->cantidad);
                if (P->efecto->tipo == defensa) P->defensa = (int)(P->defensa / P->efecto->cantidad);
                if (P->efecto->tipo == dano) P->ataque = (int)(P->ataque / P->efecto->cantidad);
            }
            free(P->efecto->nombre);
            free(P->efecto);
            P->efecto = NULL;
        }
        if (P->habilidades[0] && P->habilidades[0]->cooldownActual)
        P->habilidades[0]->cooldownActual--;
        if (P->habilidades[1] && P->habilidades[1]->cooldownActual)
        P->habilidades[1]->cooldownActual--;
        printearBarras(P,E);

        if(P->efecto!=NULL&&(P->efecto->tipo == saltarTurno)){
            P->efecto->costeTurnos-=1;
            printb("Estás %s,",P->efecto->nombre);
            Sleep(500);
            printb("¡PIERDES EL TURNO!\n");
            Sleep(1500);
        } else {
            printb("Es tu turno\n");
            verificarOpcionConBorrado(&num,3);
            borrarLineas(1);
            switch (num)
            {
            case 1:
                Pelear(P,E);
                printearBarras(P,E);
                break;
            case 2:
                obj = usarObjeto(P,E);
                printearBarras(P,E);
                break;
            case 3:
                escape = intentarHuir(P,E);
                if(escape){
                    break;
                } else;
            default:
                break;
            }
            if (escape) return 2;
        }
        if (E->vidaActual <= 0) {
            E->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", E->nombre);
            Sleep(1000);
            return 1;
        } else {
            // Turno del enemigo
            for (size_t i=0;i<3;i++){
                if (E->habilidades[i] && E->habilidades[i]->cooldownActual)
                E->habilidades[i]->cooldownActual--;
            }
            if (E->efecto && E->efecto->tipo == vida && E->efecto->op==suma) {  // liberar y poner a NULL
                E->vidaActual += (int)(E->efecto->cantidad);
                E->efecto->duracion--; // <--- REDUCE LA DURACIÓN AQUÍ
                if (E->efecto->duracion <= 0){
                    free(E->efecto->nombre);
                    free(E->efecto);
                    E->efecto= NULL;
                }
            } else if(E->efecto && E->efecto->duracion <= 0){
                // Revertir bonus/multiplicador si corresponde
                if (E->efecto->op == suma) {
                    if (E->efecto->tipo == defensa) E->defensa -= (int)(E->efecto->cantidad);
                    if (E->efecto->tipo == dano) E->ataque -= (int)(E->efecto->cantidad);
                }
                if (E->efecto->op == multiplicacion) {
                    if (E->efecto->tipo == vida) E->vidaActual = (int)(E->vidaActual / E->efecto->cantidad);
                    if (E->efecto->tipo == defensa) E->defensa = (int)(E->defensa / E->efecto->cantidad);
                    if (E->efecto->tipo == dano) E->ataque = (int)(E->ataque / E->efecto->cantidad);
                }
                free(E->efecto->nombre);
                free(E->efecto);
                E->efecto = NULL;
            }
            printearBarras(P,E);
            printb("Turno del enemigo\n");
            Sleep(1000);
            if(1){ // agrega el randomRint(0,1)
                printb("El enemigo ha decidido darte un combo en el hocico\n");
                Sleep(1000);
                int ataque;
                for (size_t i = 0; i<2;i++){
                    ataque = randomVint(Ataque(E->ataque,P->defensa,P->vida),25);
                }
                    P->vidaActual -= ataque; // <-- Aplica el daño al enemigo
                borrarLineas(5);
                printearBarras(P,E);
                printf("Turno del enemigo\n");
                if (ataque>(Ataque(E->ataque,P->defensa,P->vida) * 1.2)){
                    printb("GOLPE CRÍTICO!!\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                    Sleep(1000);
                } else {
                    printb("\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                    Sleep(1000);
                }
                Sleep(2000);
                borrarLineas(3);
            } else {
                printb("El enemigo te tiene mala asi que te lanza una habilidad");
                Sleep(1000);
            }
        }
        if (P->vidaActual <= 0) {
            P->vidaActual = 0;
            borrarLineas(11);
            printb("%s ha sido derrotado.\n", P->nombre);
            Sleep(1000);
            return 0;
            
        }
    }
}
