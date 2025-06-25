#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/map.h"
#include "tipoDato.h"
#include "random.h"
#include "interfaces.c"
#include "miscelaneo.h"

//// PROTOTIPO DE FUNCIONES

// CREACION DEL NIVEL
Escenario **crearMatriz(Map *, Map *, Jugador *, Map *); // Crea la matriz 5x5 de Escenario que representara al nivel actual junto a sus parametros predeterminados, ademas de establecer una nueva posicion aleatoria del jugador.

// FUNCIONES DE LIMPIEZA
void limpiarListaEstado(List *); // Limpia los elementos y nodos de una listas de estados
void limpiarListaHabilidades(List *); // Limpia los elementos y nodos de una listas de habilidades
void limpiarEnemigo(Enemy *); // Limpia los elementos de un enemigo
void borrarLibro(List *, char *); // Borra la primera ocurrencia de un libro en una lista
void limpiarPiso(Escenario ***); // Obtiene un puntero a una matriz de Escenario (Escenario ***), limpia todos sus elementos (Escenario contiene punteros y datos estaticos, por lo que no es necesario limpiar mas adentro) y luego marca el dato como NULL

//// MOVIMIENTO EN LA MAZMORRA
void movimientoMazmorra(Jugador *, Escenario **); // Funcion encargada de mover al jugador en la direccion deseada
int procesarTurno(Jugador *, Escenario **); // Procesa el turno, retornando un valor segun sea el estado de este
// RETURN
// 0: Muerte, fin del juego
// 1: Avanzar piso
// 2: No hacer nada

// INVENTARIO DEL JUGADOR
Skill **crearArraySkills(List *, int *); // Crea un array que contenga las skills que se pueden aprender (usando los libros del inventario)
void aprenderSkill(Jugador *, Skill *); // Aprende una skill en el primer slot disponible o pregunta cual quiere reemplazar
void inventarioJugador(Jugador *); // Muestra diferentes opciones al jugador y permite que este seleccione alguna
// 1) Ver todo el inventario
// 2) Ver objetos usables en combate
// 3) Ver habilidades aprendidas
// 4) Aprender habilidades
// 5) Salir del menu


//// CREACION DEL NIVEL

Escenario **crearMatriz(Map *mapaItems, Map *mapaEnemigos, Jugador *P, Map *mapaJefes) {
    Escenario **matriz = malloc(sizeof(Escenario *) * 5); // Se almacena memoria para cada Escenario *
    for (int i = 0 ; i < 5 ; i++) { // Se recorren todos los elementos del Escenario *
        matriz[i] = malloc(sizeof(Escenario) * 5); // Se almacena memoria para cada Escenario **

        for (int j = 0 ; j < 5 ; j++) {
            //matriz[i][j].tipo = rand() % 3; // Tipo random entre 0-2
            matriz[i][j].tipo = randomRint(0,2);
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

    // Una vez terminados todos los escenarios, elegir el jefe aleatoriamente
    //int aux = rand() % 25; // Obtener numero random entre 0-24
    int aux = randomRint(0,24);
    int indX = aux / 5; // Posicion en X
    int indY = aux % 5; // Posicion en Y
    int jefeX = indX;
    int jefeY = indY;

    // Cambiar datos para que ahora esta casilla sea de jefe
    matriz[indX][indY].tipo = JEFE;
    if (matriz[indX][indY].enemigo != NULL) { // Limpiar datos del enemigo previo si existia
         free(matriz[indX][indY].enemigo); 
    }
    matriz[indX][indY].enemigo = clonarJefe(mapaJefes, P); 
    matriz[indX][indY].objeto = NULL;

    // Actualizar los datos del jugador (spawn aleatorio)
    do { // Asegurar que no aparezca en la sala del jefe
        //aux = rand() % 25; // Obtener numero random entre 0-24
        aux = randomRint(0,24);
        indX = aux / 5; // Posicion en X
        indY = aux % 5; // Posicion en Y
    } while (jefeX == indX && jefeY == indY);
    P -> posicion.posX = indX;
    P -> posicion.posY = indY;
    return matriz;
}


//// FUNCIONES DE LIMPIEZA

void limpiarListaEstado(List *L) { 
    for (Status *actual = list_first(L); actual != NULL ; actual = list_first(L)) {
        list_popCurrent(L);
        free(actual);
    }
}

void limpiarListaHabilidades(List *L) { 
    for (Skill *actual = list_first(L); actual != NULL ; actual = list_first(L)) {
        free(actual -> nombre);
        list_popCurrent(L);
        free(actual);
    }
}

void limpiarEnemigo(Enemy *E) { 
    free(E -> nombre); //Limpia el nombre guardado dinamicamente
    list_clean(E -> loot); // Limpia la lista de loot, que contiene punteros a Item
    free(E -> loot);

    limpiarListaEstado(E -> estado); // Limpia la lista de estados
    free(E -> estado);
    E -> estado = NULL;

    limpiarListaHabilidades(E -> habilidades);
    free(E -> habilidades);
    free(E);
}

void borrarLibro(List *L, char *nombreSkill) {
    for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) {
        if (actual -> habilidadAprendida != NULL && strcmp((actual -> habilidadAprendida -> nombre), nombreSkill) == 0) {
            list_popCurrent(L);
            break;
        }
    }
}

void limpiarPiso(Escenario ***S) {
    // Sabiendo que es una matriz de 5x5
    for (int i = 0 ; i < 5 ; i++) {  // Recorrer cada elemento de la matriz (Escenario *) y lib
        for (int j = 0 ; j < 5 ; j++) {
            if ((*S)[i][j].enemigo != NULL)
                limpiarEnemigo((*S)[i][j].enemigo);
        } 
        free((*S)[i]);
        (*S)[i] = NULL;
    }
    free(*S);
    *S = NULL;
}


//// MOVIMIENTO EN LA MAZMORRA

void movimientoMazmorra(Jugador *P, Escenario **S) { // Funcion encargada de mover al jugador en la direccion deseada
    int num;
    bool flag = true; // Para controlar el loop hasta que se ingrese una opcion valida
    while (flag) {
        puts("Ingresa una direccion:");
        puts("1) Arriba / 2) Abajo / 3) Izquierda / 4) Derecha");
        verificarOpcion(&num, 4); // Obtiene y formatea la opcion elegida
        int tempX = P -> posicion.posX;
        int tempY = P -> posicion.posY;
        switch (num) {
            case 1:
                if (tempY > 0) { // Si es posible hacer el movimiento
                    P -> posicion.posY -= 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                break;

            case 2:
                if (tempY < 4) { // Si es posible hacer el movimiento
                    P -> posicion.posY += 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                break;

            case 3:
                if (tempX > 0) { // Si es posible hacer el movimiento
                    P -> posicion.posX -= 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                break;

            case 4:
                if (tempX < 4) { // Si es posible hacer el movimiento
                    P -> posicion.posX += 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                break;
        }
    }
}

int procesarTurno(Jugador *P, Escenario **S) { //Procesa el turno, retornando un valor segun sea el estado de este
    //returns:
    //0: muerte
    //1: avanzar piso
    //2: no hacer nada

    Escenario *aux = &S[P -> posicion.posX][P -> posicion.posY]; // Mucho texto
    if (aux -> visitado == false) { // Revisa si la casilla fue visitada
        aux -> visitado = true; // La marca como visitada
        switch (aux -> tipo) { // Revisa el tipo del escenario
            case VACIO:
                puts("Mala suerte, esta sala esta vacia.");
                break;

            case ENEMIGO:
                if (!interfazDeCombate(P, aux -> enemigo)) // Suponiendo que retorna 1 si fue victoria y 0 si fue derrota
                    return 0;
                break;

            case ITEM:
                interfazDeItem(P, aux -> objeto);
                break;

            case JEFE:
                int num;
                printf("Te encierra un aura maligna... El jefe %s te está esperando\n", aux -> enemigo -> nombre);
                printf("(Vida: %d, Daño: %d, Defensa: %d). ¿Estás listo para pelear?\n", aux -> enemigo -> vida, aux -> enemigo -> ataque, aux -> enemigo -> defensa);
                puts("1) Si / 2) No");
                verificarOpcion(&num, 2);
                if (num == 1)
                    comenzarPelea(P, aux -> enemigo);
                else {
                    puts("Decidiste no enfrentarte al jefe... pero la mazmorra castiga la cobardía alterando su estructura.");
                    limpiarPiso(S);
                    return 1;
                }
                break;
        }
    } else
        puts("Ya has vistado este escenario previamente.");
    return 2;
}


//// INVENTARIO DEL JUGADOR

Skill **crearArraySkills(List *L, int *cantSkills) { // Crea un array que contenga las skills que se pueden aprender (usando los libros del inventario)
    int cont = 0;   
    int max = 20;
    Skill **array = malloc(sizeof(Skill *) * max); // Array inicial de 20 elementos
     for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) { // Recorre la lista de items, buscando los libros de habilidades
        if (actual -> tipoCons == libroDeHabilidad) { // Solo obtiene libros de habilidad
            array[cont] = actual -> habilidadAprendida;
            cont++;
            if (cont >= max)  { // Si se llega al limite del array, se redimensiona.
                max *= 2;
                array = realloc(array, sizeof(Skill *) * max);
                if (array == NULL)
                    exit(EXIT_FAILURE);
            }
        }
    }
    *cantSkills = cont;
    return array;
}

void aprenderSkill(Jugador *P, Skill *H) { // Aprende una skill en el primer slot disponible o pregunta cual quiere reemplazar
    int pos;
    Skill **aux = P -> habilidades; // Crea un auxiliar para que sea mas facil el acceso
    if (aux[0] == NULL) pos = 0; // Revisa si existe alguna casilla vacia primero
    else {
        if (aux[1] == NULL) pos = 1;
        else { // Si no existe, da la opcion de intercambiar por alguna de las habilidades actuales
            puts("Selecciona que habilidad quieres reemplazar:");
            printf("1) %s \n", aux[0] -> nombre);
            printf("2) %s \n", aux[1] -> nombre);
            puts("3) Salir.\n");
            int num3;
            verificarOpcion(&num3, 3);
            switch (num3) {
                case 1:
                    pos = 0;
                    break;

                case 2:
                    pos = 1;
                    break;

                case 3:
                    return;
            }
        }
    }
    aux[pos] = H; // Agrega la habilidad y borra el primer libro que la contenga del inventario
    borrarLibro(P -> inventario, H -> nombre);
    printf("Aprendiste la Habilidad %s \n", H -> nombre);
}

void inventarioJugador(Jugador *P) {
    int num;
    while(1) {
        puts("Selecciona una opcion:");
        puts("1) Ver todo el inventario");
        puts("2) Ver objetos usables en combate");
        puts("3) Ver habilidades aprendidas");
        puts("4) Aprender habilidades");
        puts("5) Salir del menu");
        verificarOpcion(&num, 5);
        switch (num) {
            case 1: // 1) Ver todo el inventario
                for (Item *actual = list_first(P -> inventario); actual != NULL ; actual = list_next(P -> inventario)) { // Recorre la lista de items y los muestra todos, junto a su descripcion y stats segun sea necesario
                    puts(actual -> nombre);
                    puts(actual -> descripcion);
                    printf("Tipo: ");

                    // Muestra que tipo es el objeto actual
                    if (actual -> tipoCons == 1) { // Libro de habilidades
                        puts("Libro de Habilidades");
                    } else {
                        if (actual -> tipoCons == 2) { // Pocion
                            puts("Pocion");
                        } else { // No es consumible
                            if (actual -> tipoEquip == 1) {// Arma
                                puts("Arma");
                            }

                            else {// Armadura
                                puts("Armadura");
                            }
                        }
                    }
                }
                break;

            case 2: // 2) Ver objetos usables en combate (Solo tipo pocion y armaduras y armas)
                for (Item *actual = list_first(P -> inventario); actual != NULL ; actual = list_next(P -> inventario)) { // Recorre la lista de items y los muestra todos, junto a su descripcion y stats segun sea necesario
                    if (actual -> tipoCons == tipoPocion || actual -> tipoEquip != noEquipable) {
                        puts(actual -> nombre);
                        puts(actual -> descripcion);
                        printf("Tipo: ");
                        // Muestra que tipo es el objeto actual
                        if (actual -> tipoCons == tipoPocion) { // Pocion
                            puts("Pocion");
                        } else { // No es consumible
                            if (actual -> tipoEquip == ARMA) {// Arma
                                puts("Arma");
                            }

                            else {// Armadura
                                if (actual -> tipoEquip == ARMADURA)
                                puts("Armadura");
                            }
                        }
                    }
                }
                break;

            case 3: // 3) Ver habilidades aprendidas 
                bool flag = false;
                for (int i = 0 ; i < 2 ; i++) { // Recorre el array de dos habilidades
                    Skill *aux = P -> habilidades[i]; // Guarda en un auxiliar la habilidad actual
                    if (aux == NULL) continue; // Si no hay alguna habilidad en este espacio, revisa el siguiente
                    else flag = true;
                    printf("Nombre: %s \n", aux -> nombre); // Imprime informacion importante de la habilidad
                    printf("Enfriamiento: %d \n", aux -> cooldown);
                    printf("Tipo: ");
                    if (aux -> tipo) { // Dependiendo del tipo, lo muestra, junto a su informacion correspondiente
                        puts("Estado");
                        printf("Duracion: %d\n", aux -> duracion);
                    } else {
                        puts("Curacion");
                        printf("Vida recuperada: %d\n", aux -> vidaCurada);
                    }
                }
                if (!flag) {
                    puts("No tienes ninguna habilidad aprendida.");
                }
                break;

            case 4: // 4) Aprender habilidades
                int contadorSkills;
                Skill **array = crearArraySkills(P -> inventario, &contadorSkills); // Crea un array con todas las habilidades disponibles a partir del inventario
                puts("Habilidades disponibles:");
                for (int i = 0 ; i < contadorSkills ; i++) { // Printea las habilidaes
                    printf("%d) %s\n", (i + 1), array[i] -> nombre);
                }
                int num2;
                while (1) {
                    printf("Elige cual quieres aprender (Si quieres cancelar esta accion, escribe \"SALIR\")");
                    if (verificarOpcionConSalir(&num2, contadorSkills)) { // Verifica la opcion y el "SALIR"
                        break;
                    }
                    else {
                        aprenderSkill(P, array[num2-1]); // Aprende la skill elegida
                        break;
                    }
                    
                }
                free(array); // Limpia el array ya que no se va a seguir usando
                break;

            case 5:
            return;
        }
    }
}

//// MAIN

int main() {
    SetConsoleOutputCP(CP_UTF8);      // Para que la consola imprima UTF-8
    setlocale(LC_ALL, "");            // Para que printf maneje tildes y ñ

    init_random();
    //elGuerrero();
    printf("Bienvenido a la aventura del Guerrero más Bravo que hayas conocido\n");
    printf("Menú Principal Beta\n");
    
    // ...resto de tu código...
    return 0;
}