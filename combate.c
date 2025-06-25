#include "tipoDato.h"
#include <stdio.h>
#include <stdlib.h>
#include "random.h"
#include <stdarg.h>
#include <windows.h>
#include "miscelaneo.h"

// printb: imprime texto animado, igual que printf pero mostrando cada carácter uno a uno.
// Recibe los mismos argumentos que printf (formato y variables), pero el texto aparece animado.
// Puedes ajustar el tiempo de animación cambiando el valor de Sleep (en milisegundos).
void printb(const char *fmt, ...) {
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    for (int i = 0; buffer[i] != '\0'; i++) {
        putchar(buffer[i]);
        fflush(stdout);
        Sleep(10); // Tiempo de espera entre caracteres en milisegundos (ajusta a tu gusto)
    }
}

void printearBarras(Jugador *P, Enemy *E){
    char vidatuya[P->vida + 1],vidaenemiga[E->vida + 1]; // +1 para el carácter nulo '\0'
    vidatuya[0] = '\0';
    vidaenemiga[0] = '\0';
    int barrastuya = P->vidaActual / 10;
    int barrasenemiga = E->vidaActual / 10;
    for (int i = 0; i < barrastuya; i++) {
        strcat(vidatuya, "|");
    }
    for (int i = 0; i < barrasenemiga; i++) {
        strcat(vidaenemiga, "|");
    }
    printf("Tu Vida: %s (%d/%d) Defensa: %d\n", vidatuya, P->vidaActual, P->vida, P->defensa);
    printf("Enemigo: %s (%d/%d) Defensa: %d\n\n", vidaenemiga, E->vidaActual, E->vida, E->defensa);
}


void borrarLineas(int x) {
    for (int i = 0; i < x; i++) {
        printf("\033[F"); // Cursor una línea arriba
        printf("\033[2K"); // Borra la línea completa
    }
}

// Calcula el daño infligido considerando la defensa del enemigo
int Ataque(int dano, int defensaEnemiga, int fullVida) {
    double vidaTeorica = fullVida + defensaEnemiga;
    double porcentaje = (double)dano / vidaTeorica;
    int resultado = (int)(porcentaje * fullVida);
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
int usarObjeto(Jugador *P) {
    // RETURNS
    // 0: Curacion normal, el juego sigue el transcurso normal
    // 1: Muerte, fin del juego
    // 2: No considerado ya que no poseia items suficientes
    List *L = P -> inventario;
    if (list_first(L) == NULL) {
        printb("No tienes items en tu inventario.\n");
        borrarLineas(2);
        Sleep(1000);
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
        printb("No tienes items consumibles.");
        Sleep(500);
        borrarLineas(2);
        return 2;
    }

    int num;
    printb("Elige que item usar: \n");
    Sleep(1000);
    verificarOpcion(&num, i);
    num -= 1;
    borrarLineas(i+1);
    if(arrayTemp[num] -> vidaRecuperada > 0) { // Verifica si el item curaba o restaba vida
        printb("Te has recuperado %d puntos de vida\n", arrayTemp[num] -> vidaRecuperada);
        Sleep(500);
        borrarLineas(2);
    } else { 
        if (arrayTemp[num] -> vidaRecuperada < 0) {
            printb("Has perdido %d puntos de vida\n", abs(arrayTemp[num] -> vidaRecuperada));
            Sleep(500);
            borrarLineas(2);
        }
        else {
            printb("No ocurrio nada...\n");
            Sleep(500);
            borrarLineas(2);
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

void Habilidad(Jugador *P, Enemy E,int h, int lanzaSkill){ //lanzaSkill es quién lanza la habilidad (enemigo = 0 | jugador = 1) 
    //STATUS
    char nombreEfecto[40] = P->habilidades[h]->estado->nombre;  // h es la posicion de la habilidad
    int valor = P->habilidades[h]->estado->cantidad;
    int costeTurnos = P->habilidades[h]->estado->costeTurnos;
    int duracion= P->habilidades[h]->estado->duracion;
    P->habilidades[h]->estado->tipo; //vida|dano|defensa|saltarTurno
    P->habilidades[h]->estado->op; //suma|multiplicacion
    //HABILIDADES
    int vidaCurada = P->habilidades[h]->vidaCurada;
    int duracion = P->habilidades[h]->duracion;
    P->habilidades[h]->tipo;//curacion|estado
    char nombre[40] = P->habilidades[h]->nombre;
    int hacia = P->habilidades[h]->hacia; // 1 tiMismo | 0 Enemigo
    int cooldown = P->habilidades[h]->cooldown;
    int cooldownActual = P->habilidades[h]->cooldownActual;
    
}

void Pelear(Jugador *P, Enemy *E){ //FALTA GESTIONAR LAS HABILIDADES, SI NO HAY ENTONCES SE INHABILITAN OPCIONES 2 U 3 DEPENDIENDO DE LA CANTIDAD.
    printf("----- Opciones de ataque -----\n");
    printb("  1) Ataque básico\n");
    int contador=1;
    int num;
    // Mostrar habilidades (máximo 2)
    for (int i = 0; i < 2; i++) {
        if (P->habilidades[i] != NULL) {
            printb("%d) %s (Tipo: %s, Cooldown: %d turnos)\n",
                i + 2,
                P->habilidades[i]->nombre,
                P->habilidades[i]->tipo,
                P->habilidades[i]->cooldown);
        }
        contador++;
    }
    if(num>contador) num=4;
    verificarOpcionConBorrado(&num,3);
    switch (num)
    {
    case 1: {
        int ataque;
        for (size_t i = 0; i<2;i++){
                ataque = randomVint(Ataque(P->ataque,E->defensa,E->vida),50);
            }
        E->vidaActual -= ataque; // <-- Aplica el daño al enemigo
        borrarLineas(7);
        printearBarras(P,E);
        printf("Es tu turno\n");
        if (ataque>(Ataque(P->ataque,E->defensa,E->vida) * 1.3)){
            printb("GOLPE CRÍTICO!!\n");
            Sleep(750);
            printb("%s ha recibido %d puntos de daño!\n",E->nombre,ataque);
            Sleep(1000);
        } else {
            printb("\n%s ha recibido %d puntos de daño!\n",E->nombre,ataque);
        }
        Sleep(2000);
        //borrarLineas(2);
        break;
    }
    case 2:{//Habilidad 1
        //Habilidad(P,E,0,1);
    }
    default:
        break;
    }
}

void comenzarPelea(Jugador *P, Enemy *E/*, Map *status*/) {
    // Ejemplo simple de combate por turnos
    printb("¡Te has encontrado a %s!", E->nombre);
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
    printb("Tu Vida: %s (%d/%d) Defensa: %d\n", vidatuya, P->vidaActual, P->vida, P->defensa);
    printb("Enemigo: %s (%d/%d) Defensa: %d\n\n", vidaenemiga, E->vidaActual, E->vida, E->defensa);
    int num,obj;
    bool escape = false;
    while (P->vidaActual > 0 && E->vidaActual > 0) {
        // Turno del jugador
        printb("Es tu turno\n");
        Sleep(1000);
        verificarOpcionConBorrado(&num,3);
        switch (num)
        {
        case 1:
            Pelear(P,E);
            borrarLineas(3);
            break;
        case 2:
            obj = usarObjeto(P);
            borrarLineas(3);
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
        if (escape) break;
        if (E->vidaActual <= 0) {
            E->vidaActual = 0;
            borrarLineas(4);
            printb("%s ha sido derrotado.\n", E->nombre);
            Sleep(1000);
            break;
        } else {
            // Turno del enemigo
            printb("Turno del enemigo\n");
            Sleep(1000);
            if(1){ // agrega el randomRint(0,1)
                printb("El enemigo ha decidido darte un combo en el hocico\n");
                Sleep(1000);
                int ataque;
                for (size_t i = 0; i<2;i++){
                    ataque = randomVint(Ataque(E->ataque,P->defensa,P->vida),50);
                }
                    P->vidaActual -= ataque; // <-- Aplica el daño al enemigo
                borrarLineas(5);
                printearBarras(P,E);
                printf("Turno del enemigo\n");
                if (ataque>(Ataque(P->ataque,E->defensa,E->vida) * 1.3)){
                    printb("GOLPE CRÍTICO!!\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                    Sleep(1000);
                } else {
                    printb("\n%s ha recibido %d puntos de daño!\n",P->nombre,ataque);
                    Sleep(1000);
                }
                Sleep(2000);
                borrarLineas(3);
            }
        }
        if (P->vidaActual <= 0) {
            P->vidaActual = 0;
            borrarLineas(11);
            printb("%s ha sido derrotado.\n", P->nombre);
            Sleep(1000);
            printb("GAME OVER");
            break;
        }
    }
}

/*int main(){
    init_random();
    SetConsoleOutputCP(65001); // Consola en UTF-8

    Enemy enemigoPrueba;
    enemigoPrueba.nombre = "Orco";
    enemigoPrueba.vida = 80;
    enemigoPrueba.vidaActual = enemigoPrueba.vida;
    enemigoPrueba.ataque = 15;
    enemigoPrueba.defensa = 5;
    enemigoPrueba.arma = NULL;
    enemigoPrueba.esJefe = false;
    enemigoPrueba.loot = NULL;
    enemigoPrueba.estado = NULL;
    enemigoPrueba.habilidades = NULL;

    Item itemPrueba;
    itemPrueba.nombre = strdup("Completo con Chucrut");
    itemPrueba.descripcion = strdup("Te curas 200 puntos de vida, esta infravalorado");
    itemPrueba.tipoCons = tipoPocion;
    itemPrueba.tipoEquip = noEquipable;
    itemPrueba.statBonus.vidaBonus = 0;
    itemPrueba.statBonus.AtaqueBonus = 0;
    itemPrueba.statBonus.DefensaBonus = 25;
    itemPrueba.habilidadAprendida = NULL;
    itemPrueba.vidaRecuperada = 200;

    Item itemPrueba2;
    itemPrueba2.nombre = strdup("Espada de Hierro");
    itemPrueba2.descripcion = strdup("Una espada básica de hierro, +25 de daño");
    itemPrueba2.tipoCons = noConsumible;
    itemPrueba2.tipoEquip = ARMA;
    itemPrueba2.statBonus.vidaBonus = 0;
    itemPrueba2.statBonus.AtaqueBonus = 25;
    itemPrueba2.statBonus.DefensaBonus = 0;
    itemPrueba2.habilidadAprendida = NULL;
    itemPrueba2.vidaRecuperada = 0;
    List *L = list_create();
    list_pushBack(L, &itemPrueba);
    list_pushBack(L, &itemPrueba2);



    Jugador jugadorPrueba;
    jugadorPrueba.nombre = "Felipe";
    jugadorPrueba.vida = 100;
    jugadorPrueba.vidaActual = jugadorPrueba.vida;
    jugadorPrueba.ataque = 20;
    jugadorPrueba.defensa = 10;
    jugadorPrueba.nivel = 1;
    jugadorPrueba.arma = NULL;
    jugadorPrueba.armadura = NULL;
    jugadorPrueba.inventario = L;
    jugadorPrueba.habilidades[0] = NULL;
    jugadorPrueba.habilidades[1] = NULL;
    jugadorPrueba.efectos = NULL;
    jugadorPrueba.posicion.posX = 0;
    jugadorPrueba.posicion.posY = 0;

    
    
    comenzarPelea(&jugadorPrueba, &enemigoPrueba, NULL); // Llama al combate

}*/