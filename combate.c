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
        case saltarTurno: return "turnos perdidos";
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

Skill* copiarSkill(Skill* original) {
    Skill* copia = malloc(sizeof(Skill));
    *copia = *original;
    copia->nombre = strdup(original->nombre);
    if (original->estado)
        copia->estado = copiarStatus(original->estado);
    else
        copia->estado = NULL;
    return copia;
}

void printearBarras(Jugador *P, Enemy *E){
    limpiarPantalla();
    printf("\n\n\nElige una opción...Sin titubear!!\n\n1) Pelear\n2) Objetos\n3) Huir\n\n");
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
    printf("Tu Vida: %s (%d/%d) Defensa: %d\n", vidatuya, P->vidaActual, P->vida, P->defensa);
    if (P->efecto != NULL && P->efecto->duracion > 0) {
        printf(" %s", P->efecto->nombre);
        if (P->efecto->op == multiplicacion) printf(" (%sx%.1f)",tipoToStr(P->efecto->tipo),P->efecto->cantidad);
        if (P->efecto->op == suma){
            if (P->efecto->cantidad>0) printf(" (+%.0f %s)",P->efecto->cantidad,tipoToStr(P->efecto->tipo));
            if (P->efecto->cantidad<0) printf(" (%.0f %s)",P->efecto->cantidad,tipoToStr(P->efecto->tipo));
        }
    }
    printf("\n\n\n");
    printf("Enemigo: %s (%d/%d) Defensa: %d\n", vidaenemiga, E->vidaActual, E->vida, E->defensa);
    if (E->efecto != NULL && E->efecto->duracion > 0) {
        printf(" %s", E->efecto->nombre);
        if (E->efecto->op == multiplicacion) printf(" (%sx%.1f)",tipoToStr(E->efecto->tipo),E->efecto->cantidad);
        if (E->efecto->op == suma) {
            if (E->efecto->cantidad>0) printf(" (+%.0f %s)",E->efecto->cantidad,tipoToStr(E->efecto->tipo));
            if (E->efecto->cantidad<0) printf(" (%.0f %s)",E->efecto->cantidad,tipoToStr(E->efecto->tipo));
        }
    }
    printf("\n\n\n\n");
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
    int killme = 0;                         // lo hice asqueroso, pero funciona
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
    printb("Sin titubear!!\n\n");
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
    printb("\n\nTu Vida: %s (%d/%d) Defensa: %d\n\n\n\n", vidatuya, P->vidaActual, P->vida, P->defensa);
    printb("Enemigo: %s (%d/%d) Defensa: %d\n\n\n\n", vidaenemiga, E->vidaActual, E->vida, E->defensa);
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
        if(P->efecto!=NULL&&P->efecto->duracion){
            P->efecto->duracion--;
        }
        if(P->efecto!=NULL&&(P->efecto->tipo == saltarTurno)){
            P->efecto->cantidad--;
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
        if (P->vidaActual <= 0) {
            P->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", P->nombre);
            Sleep(1000);
            limpiarPantalla();
            return 0;
        }
        if (E->vidaActual <= 0) {
            E->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", E->nombre);
            Sleep(1000);
            limpiarPantalla();
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
            if (E->efecto && E->efecto->duracion){
                E->efecto->duracion--;
            }
            if(E->efecto!=NULL&&(E->efecto->tipo == saltarTurno)){
                E->efecto->cantidad--;
                printb("%s está %s,",E->nombre,E->efecto->nombre);
                Sleep(500);
                printb("¡PIERDE EL TURNO!\n");
                Sleep(1500);
                printearBarras(P,E);
            }else{
                printb("Turno del enemigo\n");
                Sleep(1000);
                int accionEnemigo = randomRint(0,1);
                if (E->efecto&&(((E->efecto->cantidad>0&&E->efecto->op == suma)||(E->efecto->cantidad>=1&&E->efecto->op == multiplicacion)))) accionEnemigo = 1;
                    if(accionEnemigo){ // combo en el hocico
                    printb("El enemigo ha decidido darte un combo en el hocico\n");
                    Sleep(1000);
                    int ataque;
                    for (size_t i = 0; i<2;i++){
                        ataque = randomVint(Ataque(E->ataque,P->defensa,P->vida),25);
                    }
                    P->vidaActual -= ataque;
                    printearBarras(P,E);
                    printf("Turno del enemigo\n");
                    if (ataque>(Ataque(E->ataque,P->defensa,P->vida) * 1.2)){
                        printb("GOLPE CRÍTICO!!\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                        
                    } else {
                        printb("\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                    }
                    Sleep(2000);
                    borrarLineas(3);
                }else {

                    int maxHabilidades = E->esJefe ? 3 : 1;
                    int disponibles[3];
                    int contador = 0;
                    for (int i = 0; i < maxHabilidades; i++) {
                        if (E->habilidades[i] && E->habilidades[i]->cooldownActual == 0) {
                            disponibles[contador++] = i;
                        }
                    }

                    if (contador > 0) {
                        printb("El enemigo te tiene mala asi que te lanza una habilidad");
                        Sleep(1000);
                        int idx = randomRint(0, contador - 1);
                        Habilidad(P, E, disponibles[idx], 0); // 0 indica que lanza el enemigo
                    } else {
                        printb("El enemigo ha decidido darte un combo en el hocico\n");
                        Sleep(1000);
                        int ataque;
                        for (size_t i = 0; i < 2; i++) {
                            ataque = randomVint(Ataque(E->ataque, P->defensa, P->vida), 25);
                        }
                        P->vidaActual -= ataque;
                        printearBarras(P, E);
                        printf("Turno del enemigo\n");
                        if (ataque > (Ataque(E->ataque, P->defensa, P->vida) * 1.2)) {
                            printb("GOLPE CRÍTICO!!\n%s ha recibido %d puntos de daño!\n", P->nombre, ataque);
                            
                        } else {
                            printb("\n%s ha recibido %d puntos de daño!\n", P->nombre, ataque);
                            
                        }
                        Sleep(2000);
                        borrarLineas(3);
                    }
                }
            }
        }
        if (E->vidaActual <= 0) {
            E->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", E->nombre);
            Sleep(1000);
            limpiarPantalla();
            return 1;
        }
        if (P->vidaActual <= 0) {
            P->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", P->nombre);
            Sleep(1000);
            limpiarPantalla();
            return 0;
        }
    }
}

int main(){
    init_random();
    SetConsoleOutputCP(65001); // Consola en UTF-8
    //STATUS

    //18,Blindaje,defensa,,suma,30
    Status *statusBlindaje = malloc(sizeof(Status));
    statusBlindaje->id = 18;
    statusBlindaje->nombre = strdup("Blindaje");
    statusBlindaje->tipo = defensa;
    statusBlindaje->op = suma;
    statusBlindaje->cantidad = 30;
    statusBlindaje->costeTurnos = 0;
    statusBlindaje->duracion = 0;

    //6,Fuerza Potenciada,daño,,mult,1.5
    Status *statusFuerzaPotenciada = malloc(sizeof(Status));
    statusFuerzaPotenciada->id = 6;
    statusFuerzaPotenciada->nombre = strdup("Fuerza Potenciada");
    statusFuerzaPotenciada->tipo = dano;
    statusFuerzaPotenciada->op = multiplicacion;
    statusFuerzaPotenciada->cantidad = 1.5;
    statusFuerzaPotenciada->costeTurnos = 0;
    statusFuerzaPotenciada->duracion = 0;

    //8,Defensa de Hierro,defensa,,mult,2
    Status *statusDefensaHierro = malloc(sizeof(Status));
    statusDefensaHierro->id = 8;
    statusDefensaHierro->nombre = strdup("Defensa de Hierro");
    statusDefensaHierro->tipo = defensa;
    statusDefensaHierro->op = multiplicacion;
    statusDefensaHierro->cantidad = 2;
    statusDefensaHierro->costeTurnos = 0;
    statusDefensaHierro->duracion = 0;

    //16,Inspirado,daño,,suma,10
    Status *statusInspirado = malloc(sizeof(Status));
    statusInspirado->id = 16;
    statusInspirado->nombre = strdup("Inspirado");
    statusInspirado->tipo = dano;
    statusInspirado->op = suma;
    statusInspirado->cantidad = 10;
    statusInspirado->costeTurnos = 0;
    statusInspirado->duracion = 0;

    //14,Congelado,saltarTurno,1,,1
    Status *statusCongelado = malloc(sizeof(Status));
    statusCongelado->id = 14;
    statusCongelado->nombre = strdup("Congelado");
    statusCongelado->tipo = saltarTurno;
    statusCongelado->op = suma; // o el valor que corresponda si tienes otro enum para "sin operación"
    statusCongelado->cantidad = 2;
    statusCongelado->costeTurnos = 2;
    statusCongelado->duracion = 0;

    //2,Regeneración,vida,,suma,10
    Status *statusRegeneracion = malloc(sizeof(Status));
    statusRegeneracion->id = 2;
    statusRegeneracion->nombre = strdup("Regeneración");
    statusRegeneracion->tipo = vida;
    statusRegeneracion->op = suma;
    statusRegeneracion->cantidad = 10;
    statusRegeneracion->costeTurnos = 0;
    statusRegeneracion->duracion = 0;

    //1,Envenenado,vida,,suma,-10
    Status *statusEnvenenado = malloc(sizeof(Status));
    statusEnvenenado->id = 1;
    statusEnvenenado->nombre = strdup("Envenenado");
    statusEnvenenado->tipo = vida;
    statusEnvenenado->op = suma;
    statusEnvenenado->cantidad = -10;
    statusEnvenenado->costeTurnos = 0;
    statusEnvenenado->duracion = 0;
    
    //SKILLS

    //Defensa de Hierro,5,2,estado,0,8,1
    Skill *skillDefensaHierro = malloc(sizeof(Skill));
    skillDefensaHierro->nombre = strdup("Defensa de Hierro");
    skillDefensaHierro->cooldown = 5;
    skillDefensaHierro->cooldownActual = 0;
    skillDefensaHierro->duracion = 3;
    skillDefensaHierro->tipo = estado;
    skillDefensaHierro->vidaCurada = 0;
    skillDefensaHierro->estado = copiarStatus(statusDefensaHierro); // id 8
    skillDefensaHierro->hacia = 1; // propio

    //Fuerza Potenciada,5,2,estado,0,6,1
    Skill *skillFuerzaPotenciada = malloc(sizeof(Skill));
    skillFuerzaPotenciada->nombre = strdup("Fuerza Potenciada");
    skillFuerzaPotenciada->cooldown = 5;
    skillFuerzaPotenciada->cooldownActual = 0;
    skillFuerzaPotenciada->duracion = 3;
    skillFuerzaPotenciada->tipo = estado;
    skillFuerzaPotenciada->vidaCurada = 0;
    skillFuerzaPotenciada->estado = copiarStatus(statusFuerzaPotenciada); // id 6
    skillFuerzaPotenciada->hacia = 1; // propio

    //Blindaje,4,2,estado,0,18,1
    Skill *skillBlindaje = malloc(sizeof(Skill));
    skillBlindaje->nombre = strdup("Blindaje");
    skillBlindaje->cooldown = 4;
    skillBlindaje->cooldownActual = 0;
    skillBlindaje->duracion = 3;
    skillBlindaje->tipo = estado;
    skillBlindaje->vidaCurada = 0;
    skillBlindaje->estado = copiarStatus(statusBlindaje); // id 18
    skillBlindaje->hacia = 1; // propio

    //Congelar,5,1,estado,0,14,0
    Skill *skillCongelar = malloc(sizeof(Skill));
    skillCongelar->nombre = strdup("Congelar");
    skillCongelar->cooldown = 5;
    skillCongelar->cooldownActual = 0;
    skillCongelar->duracion = 2;
    skillCongelar->tipo = estado;
    skillCongelar->vidaCurada = 0;
    skillCongelar->estado = copiarStatus(statusCongelado); // llama al status id 14
    skillCongelar->hacia = 0; // enemigo

    //Curación Mayor,4,0,curacion,50,,1
    Skill *skillCuracionMayor = malloc(sizeof(Skill));
    skillCuracionMayor->nombre = strdup("Curación Mayor");
    skillCuracionMayor->cooldown = 4;
    skillCuracionMayor->cooldownActual = 0;
    skillCuracionMayor->duracion = 0;
    skillCuracionMayor->tipo = curacion;
    skillCuracionMayor->vidaCurada = 50;
    skillCuracionMayor->estado = NULL;
    skillCuracionMayor->hacia = 1; // propio

    //Inspirar,3,2,estado,0,16,1
    Skill *skillInspirar = malloc(sizeof(Skill));
    skillInspirar->nombre = strdup("Inspirar");
    skillInspirar->cooldown = 3;
    skillInspirar->cooldownActual = 0;
    skillInspirar->duracion = 1;
    skillInspirar->tipo = estado;
    skillInspirar->vidaCurada = 0;
    skillInspirar->estado = copiarStatus(statusInspirado); // llama al status id 16
    skillInspirar->hacia = 1; // propio

    //Regenerar,3,2,estado,0,2,1
    Skill *skillRegeneracion = malloc(sizeof(Skill));
    skillRegeneracion->nombre = strdup("Regeneración");
    skillRegeneracion->cooldown = 3;
    skillRegeneracion->cooldownActual = 0;
    skillRegeneracion->duracion = 3;
    skillRegeneracion->tipo = estado;
    skillRegeneracion->vidaCurada = 0;
    skillRegeneracion->estado = copiarStatus(statusRegeneracion); // llama al status id 2
    skillRegeneracion->hacia = 1; // propio

    //Envenenar,3,2,estado,0,1,0
    Skill *skillEnvenenar = malloc(sizeof(Skill));
    skillEnvenenar->nombre = strdup("Envenenar");
    skillEnvenenar->cooldown = 3;
    skillEnvenenar->cooldownActual = 0;
    skillEnvenenar->duracion = 3;
    skillEnvenenar->tipo = estado;
    skillEnvenenar->vidaCurada = 0;
    skillEnvenenar->estado = copiarStatus(statusEnvenenado); // llama al status id 1
    skillEnvenenar->hacia = 0; // enemigo

    
    Item itemPrueba;
    itemPrueba.nombre = strdup("Completo con Chucrut");
    itemPrueba.descripcion = strdup("Te curas 200 puntos de vida, esta infravalorado");
    itemPrueba.tipoCons = tipoPocion;
    itemPrueba.tipoEquip = noEquipable;
    itemPrueba.statBonus.vida = 0;
    itemPrueba.statBonus.ataque = 0;
    itemPrueba.statBonus.defensa = 25;
    itemPrueba.habilidadAprendida = NULL;
    itemPrueba.vidaRecuperada = 200;
    
    Item itemPrueba2;
    itemPrueba2.nombre = strdup("Espada de Hierro");
    itemPrueba2.descripcion = strdup("Una espada básica de hierro, +25 de daño");
    itemPrueba2.tipoCons = noConsumible;
    itemPrueba2.tipoEquip = ARMA;
    itemPrueba2.statBonus.vida = 0;
    itemPrueba2.statBonus.ataque = 25;
    itemPrueba2.statBonus.defensa = 0;
    itemPrueba2.habilidadAprendida = NULL;
    itemPrueba2.vidaRecuperada = 0;
    List *L = list_create();
    list_pushBack(L, &itemPrueba);
    list_pushBack(L, &itemPrueba2);
    
    Jugador jugadorPrueba;
    jugadorPrueba.nombre = "Felipe";
    jugadorPrueba.arma = &itemPrueba2;
    jugadorPrueba.armadura = NULL;
    jugadorPrueba.nivel = 10;
    jugadorPrueba.vida = (int)(100 * pow(1.03, ((float)jugadorPrueba.nivel - 1)))
    + (jugadorPrueba.armadura ? jugadorPrueba.armadura->statBonus.vida : 0)
    + (jugadorPrueba.arma ? jugadorPrueba.arma->statBonus.vida : 0);
    jugadorPrueba.vidaActual = jugadorPrueba.vida;
    jugadorPrueba.ataque = (int)(25 * pow(1.03, ((float)jugadorPrueba.nivel - 1)))
    + (jugadorPrueba.armadura ? jugadorPrueba.armadura->statBonus.ataque : 0)
    + (jugadorPrueba.arma ? jugadorPrueba.arma->statBonus.ataque : 0);
    jugadorPrueba.defensa = (int)(10 * pow(1.03, ((float)jugadorPrueba.nivel - 1)))
    + (jugadorPrueba.armadura ? jugadorPrueba.armadura->statBonus.defensa : 0)
    + (jugadorPrueba.arma ? jugadorPrueba.arma->statBonus.defensa : 0);
    jugadorPrueba.inventario = L;
    jugadorPrueba.habilidades[0] = copiarSkill(skillDefensaHierro);
    jugadorPrueba.habilidades[1] = copiarSkill(skillCongelar)/*bolaDeFuego*/; // o puedes poner otra skill si quieres
    jugadorPrueba.efecto = NULL;
    jugadorPrueba.posicion.posX = 0;
    jugadorPrueba.posicion.posY = 0;
    
    Enemy enemigoPrueba;
    enemigoPrueba.nombre = "Orco";
    enemigoPrueba.vida = (int)(500 * pow(1.05, ((float)jugadorPrueba.nivel - 1)));
    enemigoPrueba.vidaActual = enemigoPrueba.vida;
    enemigoPrueba.ataque = (int)(5 * pow(1.05, ((float)jugadorPrueba.nivel - 1)));
    enemigoPrueba.defensa = (int)(100 * pow(1.05, ((float)jugadorPrueba.nivel - 1)));
    enemigoPrueba.esJefe = true;
    enemigoPrueba.loot = NULL;
    enemigoPrueba.efecto = NULL;
    enemigoPrueba.habilidades[0] = copiarSkill(skillCongelar);
    enemigoPrueba.habilidades[1] = NULL/*copiarSkill(skillFuerzaPotenciada)*/;
    enemigoPrueba.habilidades[2] = NULL/*copiarSkill(skillInspirar)*/;
    
    
    comenzarPelea(&jugadorPrueba, &enemigoPrueba/*, NULL*/); // Llama al combate
    
}