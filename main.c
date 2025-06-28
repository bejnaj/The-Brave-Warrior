#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <math.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/multimapaItems.h"
#include "tdas/hashmap.h"
#include "tipoDato.h"
#include "random.h"
#include "lectura.h"
#include "interfaces.h"
#include "miscelaneo.h"
#include "combate.h"

void imprimirListaEnemigos(List *enemigos);

//// PROTOTIPO DE FUNCIONES

//// CLONADO

// Clona un enemigo aleatorio de una lista y le asigna el multiplicador correspondiente
Enemy *clonarEnemigo(List *L, float mult, bool esJefe);

// Obtiene un item aleatorio del mapa de items
Item *obtenerItem(multiMapa *mapaItems, Jugador *P);

//// LOOT

// asigna loot aleatorio a un enemigo segun la lista de items
void asignarLootAleatorio(Jugador *P, Enemy *enemigo, multiMapa *mapaItems);

//// CREACION DEL NIVEL
Escenario **crearMatriz(multiMapa *, List *, Jugador *, List *, float *mult); // Crea la matriz 5x5 de Escenario que representara al nivel actual junto a sus parametros predeterminados, ademas de establecer una nueva posicion aleatoria del jugador.


//// FUNCIONES DE LIMPIEZA
void limpiarListaEstado(List *L); // Limpia los elementos y nodos de una listas de estados
void limpiarListaHabilidades(List *L); // Limpia los elementos y nodos de una listas de habilidades
void limpiarEnemigo(Enemy *E); // Limpia los elementos de un enemigo
void borrarLibro(List *L, char *str); // Borra la primera ocurrencia de un libro en una lista
void borrarEquipable(List *L, char *nombreEquip); // Borra la primera ocurrencia de un equipable en una lista
void limpiarPiso(Escenario ***S); // Obtiene un puntero a una matriz de Escenario (Escenario ***), limpia todos sus elementos (Escenario contiene punteros y datos estaticos, por lo que no es necesario limpiar mas adentro) y luego marca el dato como NULL
void limpiarJuego(List *listaJefes, List *listaEnemeigos, HashMap *mapaStatus, multiMapa *mapaItems, Jugador *P, Escenario **S); // Limpia los datos del juego y cierra la aplicacion

//// MOVIMIENTO EN LA MAZMORRA
void movimientoMazmorra(Jugador *P, Escenario **S); // Funcion encargada de mover al jugador en la direccion deseada
int procesarTurno(Jugador *P, Escenario **S, float *mult); // Procesa el turno, retornando un valor segun sea el estado de este
// RETURN
// 0: Muerte, fin del juego
// 1: Avanzar piso
// 2: No hacer nada
void mostrarNivel(Jugador *P, Escenario **S); // Muestra el estado del nivel actual


//// RELACIONADAS A JUGADOR
Skill **crearArraySkills(List *, int *); // Crea un array que contenga las skills que se pueden aprender (usando los libros del inventario)
Item **crearArrayEquip(List *L, int *cantEquip); // Crea un array que contenga los objetos equipables (usando el inventario)
void aprenderSkill(Jugador *P, Skill *H); // Aprende una skill en el primer slot disponible o pregunta cual quiere reemplazar
void equiparObjeto(Jugador *P, Item *I); // Equipa el objeto elegido
void inventarioJugador(Jugador *P); // Muestra diferentes opciones al jugador y permite que este seleccione alguna
// 1) Ver todo el inventario
// 2) Ver objetos usables en combate
// 3) Ver habilidades aprendidas
// 4) Aprender habilidades
// 5) Salir del menu
void infoJugador(Jugador *P); // Muestra la informacion basica acerca del jugador (vida, habilidades, arma y armadura equipadas y los efectos activos)
void levelUp(Jugador *P); // aplica las mejoras al jugador
Jugador *inicializarJugador(char *str); // Inicializa un jugador con el nombre dado
void recalcularStats(Jugador *P); // Recalcula las stats principales del jugador acorde a lo que este posea


//// INTERFAZ DE TESORO
void interfazDeTesoro(Jugador *, Item *);
void mostrarInfoItem(Item *item);

//// CLONADO

Enemy *clonarEnemigo(List *L, float mult, bool esJefe) {
    Enemy *actual;
    if (esJefe) {
        do {
            int pos = randomRint(0, list_size(L) - 1); // Se elige una posicion random de la lista
            actual = list_get(L, pos);
 
        } while (actual == NULL || actual->esJefe == false);
    }
    else {
        do {
            int pos = randomRint(0, list_size(L) - 1); // Se elige una posicion random de la lista
            actual = list_get(L, pos);
 
        } while (actual == NULL || actual->esJefe == true);
    }
    if (actual == NULL) return NULL;
    Enemy *E = malloc(sizeof(Enemy));
    if (actual->nombre == NULL) {
        printf("Error: enemigo sin nombre\n");
        E->nombre = strdup("enemigoSinNombre");
    } else {
        E->nombre = strdup(actual->nombre);
    }
    E -> vida = actual -> vida * mult;
    E -> vidaActual = E -> vida;
    E -> ataque = actual -> ataque * mult;
    E -> defensa = actual -> defensa * mult;
    E -> loot = NULL;
    E -> esJefe = actual -> esJefe;
    E -> efecto = actual -> efecto;

    for (int i = 0 ; i < 3 ; i++) {
        if (actual -> habilidades[i] != NULL)
            E -> habilidades[i] = copiaSkill(actual -> habilidades[i]);
        else
            E -> habilidades[i] = NULL;
    }
    return E;
}

Item *obtenerItem(multiMapa *mapaItems, Jugador *P) {
    if (!mapaItems || !P) return NULL;

    int poderMax = powerIndexPlayer(P);
    if (poderMax < 1) poderMax = 1;

    int poderAleatorio = randomRint(1, poderMax);

    multiPar *actual = buscarMultiMapa(mapaItems, poderAleatorio);

    // Si no hay items exactos con ese poder, buscar hacia abajo el primero que exista
    while (actual == NULL && poderAleatorio > 1) {
        poderAleatorio--;
        actual = buscarMultiMapa(mapaItems, poderAleatorio);
    }

    // Si no se encontró ningún ítem (muy raro si hay items con poder 1)
    if (actual == NULL) {
        return NULL;
    }

    List *auxLista = actual->values;
    int tam = list_size(auxLista);
    if (tam == 0) return NULL; // lista vacía, evitar crash

    int indice = randomRint(0, tam - 1);
    Item *auxItem = list_get(auxLista, indice);

    return auxItem;
}


//// LOOT

void asignarLootAleatorio(Jugador *P, Enemy *enemigo, multiMapa *mapaItems) {
    if (!enemigo || !mapaItems || mapaItems -> size == 0) return;

    int chance = randomRint(1, 10); // genera numero aleatorio entre 1 y 100 para determinar si no deja loot (30% probabilidad de tener loot)
    if (chance > 3) {
        enemigo->loot = NULL; // no deja loot
        return;
    }

    int poderAleatorio = randomRint(1, powerIndexPlayer(P)); // Indica la posicion aleatoria entre el rango 1 y el poder maximo que puede obtener el jugador actualmente
    multiPar *actual = buscarMultiMapa(mapaItems, poderAleatorio); // Busca si existen objetos en ese poder
    if(actual == NULL) {
        actual = anteriorMultiMapa(mapaItems);
        if (actual == NULL) return;
    }
    List *auxLista = actual -> values; // Obtiene la lista de elementos
    Item *auxItem = list_get(auxLista, randomRint(0, list_size(auxLista) - 1)); // Obtiene un indice aleatorio de la lista de elementos
    enemigo -> loot = auxItem;
    return;
}

void lootearEnemigo(Jugador *P, Enemy *E) {
    if (E -> loot) { // Si el enemigo tiene loot
        if (randomRint(1,100) <= 15) // Drop de item, con 15% probabilidad
            list_pushBack(P -> inventario, E -> loot);
    }
}


//// CREACION DEL NIVEL

Escenario **crearMatriz(multiMapa *mapaItems, List *listaEnemigos, Jugador *P, List *listaJefes, float *mult) {
    Escenario **matriz = malloc(sizeof(Escenario *) * 5); // Se almacena memoria para los punteros a cada fila de Escenario
    if (matriz == NULL) {
        printf("Error al asignar memoria para la matriz de escenarios.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0 ; i < 5 ; i++) { // Se recorren todos los elementos del Escenario *
        matriz[i] = malloc(sizeof(Escenario) * 5); // Se almacena memoria para cada fila (array de Escenario)
        if (matriz[i] == NULL) {
            printf("Error al asignar memoria para matriz[%d]\n", i);
            exit(EXIT_FAILURE);
        }
        
        for (int j = 0 ; j < 5 ; j++) {
            matriz[i][j].tipo = randomRint(0,2);
            switch (matriz[i][j].tipo) { // Depende del tipo seleccionado
                case VACIO:
                    matriz[i][j].enemigo = NULL;
                    matriz[i][j].objeto = NULL;
                    break;

                case ENEMIGO:
                    matriz[i][j].enemigo = clonarEnemigo(listaEnemigos, *mult, false);
                    asignarLootAleatorio(P, matriz[i][j].enemigo, mapaItems);
                    matriz[i][j].objeto = NULL;
                    break;
            
                case ITEM:
                    matriz[i][j].objeto = obtenerItem(mapaItems, P);
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
    matriz[indX][indY].enemigo = clonarEnemigo(listaJefes, *mult, true);
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

void limpiarEstado(Status *S) { 
    free(S -> nombre);
}

void limpiarHabilidad(Skill *H) {
    if (H != NULL) {
        free(H->nombre);
        if (H->estado != NULL)
            free(H->estado);
        free(H);
    }
}

void limpiarEnemigo(Enemy *E) {
    if (E == NULL) return;

    // Liberar nombre
    free(E->nombre);

   
    if (E->efecto != NULL) { // Liberar el efecto
        limpiarEstado(E -> efecto);
    }

    // Liberar las habilidades
    for (int i = 0; i < 3; i++) {
        Skill *aux = E->habilidades[i];
        if ((aux) != NULL) {
            limpiarHabilidad(aux);
        }
    }

    // Liberar la estructura Enemy
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

void borrarEquipable(List *L, char *nombreEquip) {
    for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) {
        if (strcmp((actual -> nombre), nombreEquip) == 0) {
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

void limpiarJuego(List *listaJefes, List *listaEnemigos, HashMap *mapaStatus, multiMapa *mapaItems, Jugador *P, Escenario **S) { // Limpia los datos del juego y cierra la aplicacion
    limpiarPiso(&S);
    // Limpiado de enemigos
    for (Enemy *actual = list_popFront(listaJefes) ; actual != NULL ; actual = list_popFront(listaJefes)) {
        limpiarEnemigo(actual);
    }
    for (Enemy *actual = list_popFront(listaEnemigos) ; actual != NULL ; actual = list_popFront(listaEnemigos)) {
        limpiarEnemigo(actual);
    }
    
}


//// MOVIMIENTO EN LA MAZMORRA

void movimientoMazmorra(Jugador *P, Escenario **S) { // Funcion encargada de mover al jugador en la direccion deseada
    int num;
    bool flag = true; // Para controlar el loop hasta que se ingrese una opcion valida
    while (flag) {
        puts("Ingresa una direccion:");
        puts("1) Arriba / 2) Abajo / 3) Izquierda / 4) Derecha / 5) Entrar al apartado de inventario");
        verificarOpcionConBorrado(&num, 5); // Obtiene y formatea la opcion elegida
        borrarLineas(3);
        int tempX = P -> posicion.posX;
        int tempY = P -> posicion.posY;
        switch (num) {
            case 3:
                if (tempY > 0) { // Si es posible hacer el movimiento
                    P -> posicion.posY -= 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                    Sleep(500);
                    borrarLineas(1);
                break;

            case 4:
                if (tempY < 4) { // Si es posible hacer el movimiento
                    P -> posicion.posY += 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                    Sleep(500);
                    borrarLineas(1); 
                break;

            case 1:
                if (tempX > 0) { // Si es posible hacer el movimiento
                    P -> posicion.posX -= 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                    Sleep(500);
                    borrarLineas(1); 
                break;

            case 2:
                if (tempX < 4) { // Si es posible hacer el movimiento
                    P -> posicion.posX += 1;
                    flag = false;
                }
                else
                    puts("No es posible realizar ese movimiento.");
                    Sleep(500);
                    borrarLineas(1); 
                break;
            case 5:
                borrarLineas(7);
                inventarioJugador(P);
                limpiarPantalla();
                recalcularStats(P);
                mostrarNivel(P, S);
                break;
        }
    }
}

int procesarTurno(Jugador *P, Escenario **S, float *mult) { //Procesa el turno, retornando un valor segun sea el estado de este
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
                Sleep(750);
                break;

            case ENEMIGO: {
                borrarLineas(6);
                int disc = comenzarPelea(P, aux -> enemigo); // Suponiendo que retorna 1 si fue victoria, 0 si fue derrota y 2 si huyo
                if (disc == 1) {
                    P -> xp++; // Sube un punto de xp
                    if((P -> xp) % 3 == 0) { // Pregunta si se puede subir de nivel
                        P -> nivel++;
                        P -> xp = 0;
                        levelUp(P);
                    }
                    lootearEnemigo(P, aux -> enemigo);
                } else
                    if (disc == 0) { // Si murio
                        return 0;
                    }
                break;
            }
            case ITEM:
                limpiarPantalla();
                interfazDeTesoro(P, aux -> objeto);
                break;

            case JEFE: {
                borrarLineas(6); 
                int num;
                printf("Te encierra un aura maligna... El jefe %s te está esperando\n", aux -> enemigo -> nombre);
                printf("(Vida: %d, Daño: %d, Defensa: %d). ¿Estás listo para pelear?\n", aux -> enemigo -> vida, aux -> enemigo -> ataque, aux -> enemigo -> defensa);
                puts("1) Si / 2) No");
                verificarOpcionConBorrado(&num, 2);
                if (num == 1) {
                    if(comenzarPelea(P, aux -> enemigo))
                        levelUp(P);
                    else
                        return 0;

                    (*mult) *= 1.07;
                    printb("¡Has vencido al jefe! El aire se despeja y la mazmorra cambia de forma...\n");
                    printb("La dificultad ha aumentado. Multiplicador actual: %.2fx\n", *mult);
                    Sleep(1500);
                    borrarLineas(2);
                } else {
                    printb("Decidiste no enfrentarte al jefe... pero la mazmorra castiga la cobardía alterando su estructura.\n");
                    Sleep(1000);
                    borrarLineas(1);
                }
                limpiarPiso(&S);
                return 1;
            }
        }
    } else {
        puts("Ya has vistado este escenario previamente.");
        borrarLineas(7);
    }
    limpiarPantalla();
    return 2;
}

void mostrarNivel(Jugador *P, Escenario **E) {
    for (int i = 0 ; i < 5 ; i++) {
        for (int j = 0 ; j < 5 ; j++) {
            if ((i == P -> posicion.posX) && (j == P -> posicion.posY))
                printf("J ");
            else {
                if (E[i][j].visitado)
                    printf("O ");
                else {
                    printf("X ");
                }
            }
        }
        puts("");
    }
    puts("\nJ = Jugador, O = Visitado, X = No Visitado");
}


//// RELACIONADAS A JUGADOR

Skill **crearArraySkills(List *L, int *cantSkills) { // Crea un array que contenga las skills que se pueden aprender (usando los libros del inventario)
    int cont = 0;
    int max = 20;
    Skill **array = malloc(sizeof(Skill *) * max);
    if (array == NULL) {
        perror("Error al asignar memoria para array de skills");
        exit(EXIT_FAILURE);
    }
    for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) {
        if (actual->tipoCons == libroDeHabilidad && actual->habilidadAprendida != NULL) {
            array[cont++] = actual->habilidadAprendida;
            if (cont >= max) {
                max *= 2;
                Skill **tmp = realloc(array, sizeof(Skill *) * max);
                if (tmp == NULL) {
                    free(array);
                    perror("Error al redimensionar array de skills");
                    exit(EXIT_FAILURE);
                }
                array = tmp;
            }
        }
    }

    *cantSkills = cont;
    return array;
}

Item **crearArrayEquip(List *L, int *cantEquip) { // Crea un array que contenga los objetos equipables (usando el inventario)
    int cont = 0;   
    int max = 30;
    Item **array = malloc(sizeof(Item *) * max); // Array inicial de 20 elementos
     for (Item *actual = list_first(L); actual != NULL ; actual = list_next(L)) { // Recorre la lista de items, buscando los equipables
        if (actual -> tipoEquip == ARMA || actual -> tipoEquip == ARMADURA) { // Solo obtiene equipables
            array[cont] = actual;
            cont++;
            if (cont >= max)  { // Si se llega al limite del array, se redimensiona.
                max *= 2;
                array = realloc(array, sizeof(Item *) * max);
                if (array == NULL)
                    exit(EXIT_FAILURE);
            }
        }
    }
    *cantEquip = cont;
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
            verificarOpcionConBorrado(&num3, 3);
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

void equiparObjeto(Jugador *P, Item *I) { // Equipa el objeto elegido
    // Calcula la diferencia de stats bonus
    int difVida = 0, difAtaque = 0, difDefensa = 0;
    if (I -> tipoEquip == ARMA) {
        if (P -> arma != NULL) {
            difVida = (I -> statBonus.vida) - (P -> arma -> statBonus.vida);
            difAtaque = (I -> statBonus.ataque) - (P -> arma -> statBonus.ataque);
            difDefensa = (I -> statBonus.defensa) - (P -> arma -> statBonus.defensa);
        } else {
            difVida = (I -> statBonus.vida);
            difAtaque = (I -> statBonus.ataque);
            difDefensa = (I -> statBonus.defensa);
        }    
    } else {
        if (P -> armadura != NULL) {
            difVida = (I -> statBonus.vida) - (P -> armadura -> statBonus.vida);
            difAtaque = (I -> statBonus.ataque) - (P -> armadura -> statBonus.ataque);
            difDefensa = (I -> statBonus.defensa) - (P -> armadura -> statBonus.defensa);
         } else {
            difVida = (I -> statBonus.vida);
            difAtaque = (I -> statBonus.ataque);
            difDefensa = (I -> statBonus.defensa);
        }    
    }

    // Cambia el item
    borrarEquipable(P -> inventario, I -> nombre);

    Item **aux = (I->tipoEquip == ARMA) ? &(P->arma) : &(P->armadura);

    if (*aux != NULL)
        list_pushBack(P -> inventario, *aux);
    *aux = I;

    // Reajusta las estadisticas
    P -> vidaActual += difVida;
    if (P -> vidaActual <= 0) {
        P -> vidaActual = 1;
    }
    P -> ataque += difAtaque;
    P -> defensa += difDefensa;
}

void inventarioJugador(Jugador *P) {
    int num, cont;
    bool flag = false;
    while(1) {
        puts("Selecciona una opcion:");
        puts("1) Ver todo el inventario");
        puts("2) Ver objetos usables en combate");
        puts("3) Ver habilidades aprendidas");
        puts("4) Aprender habilidades");
        puts("5) Equipar algun objeto");
        puts("6) Volver al movimiento en la mazmorra");
        verificarOpcionConBorrado(&num, 6);
        limpiarPantalla();
        flag = false;
        cont = 0;
        switch (num) {
            case 1: { // 1) Ver todo el inventario
                for (Item *actual = list_first(P -> inventario); actual != NULL ; actual = list_next(P -> inventario)) { // Recorre la lista de items y los muestra todos, junto a su descripcion y stats segun sea necesario
                    flag = true;
                    puts(actual -> nombre);
                    puts(actual -> descripcion);
                    printf("Tipo: ");
                    cont += 3;
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
                if (!flag) {
                    puts("No tienes ningun objeto en tu inventario.");
                    cont++;
                }
                esperarAccion();
                limpiarPantalla();
                break;
            }
            case 2: // 2) Ver objetos usables en combate (Solo tipo pocion y armaduras y armas)
                for (Item *actual = list_first(P -> inventario); actual != NULL ; actual = list_next(P -> inventario)) { // Recorre la lista de items y los muestra todos, junto a su descripcion y stats segun sea necesario
                    if (actual -> tipoCons == tipoPocion || actual -> tipoEquip != noEquipable) {
                        cont++;
                        flag = true;
                        puts(actual -> nombre);
                        puts(actual -> descripcion);
                        printf("Tipo: ");
                        cont += 3;
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
                if (!flag) {
                    puts("No tienes ningun objeto en tu inventario.");
                    cont++;
                }
                esperarAccion();
                limpiarPantalla();
                break;

            case 3: { // 3) Ver habilidades aprendidas 
                bool flag = false;
                for (int i = 0 ; i < 2 ; i++) { // Recorre el array de dos habilidades
                    Skill *aux = P -> habilidades[i]; // Guarda en un auxiliar la habilidad actual
                    if (aux == NULL) continue; // Si no hay alguna habilidad en este espacio, revisa el siguiente
                    else flag = true;
                    printf("Nombre: %s \n", aux -> nombre); // Imprime informacion importante de la habilidad
                    printf("Enfriamiento: %d \n", aux -> cooldown);
                    printf("Tipo: ");
                    cont += 4;
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
                esperarAccion();
                limpiarPantalla();
                break;
            }
            case 4: { // 4) Aprender habilidades
                int contadorSkills;
                Skill **array = crearArraySkills(P -> inventario, &contadorSkills); // Crea un array con todas las habilidades disponibles a partir del inventario
                if (contadorSkills == 0) {
                    free(array);
                    puts("No tienes habilidades para aprender");
                    esperarAccion();
                    limpiarPantalla();
                    break;
                }
                puts("Habilidades disponibles:");
                for (int i = 0 ; i < contadorSkills ; i++) { // Printea las habilidaes
                    printf("%d) %s\n", (i + 1), array[i] -> nombre);
                }
                int num2;
                while (1) {
                    printf("Elige cual quieres aprender (Si quieres cancelar esta accion, escribe \"SALIR\")\n");
                    if (verificarOpcionConSalir(&num2, contadorSkills) == 1) { // Verifica la opcion y el "SALIR"
                        break;
                    }
                    else {
                        aprenderSkill(P, array[num2-1]); // Aprende la skill elegida
                        break;
                    }
                    
                }
                free(array); // Limpia el array ya que no se va a seguir usando
                break;
            }
            case 5:{
                int contadorEquip;
                Item **array = crearArrayEquip(P -> inventario, &contadorEquip); // Crea un array con todos los equipables disponibles a partir del inventario
                if (contadorEquip == 0) {
                    free(array);
                    puts("No tienes objetos para equipar");
                    esperarAccion();
                    limpiarPantalla();
                    break;
                }
                puts("Equipables disponibles:");
                for (int i = 0 ; i < contadorEquip ; i++) { // Printea los equipables
                    printf("%d) %s, Tipo: ", (i + 1), array[i] -> nombre);
                    if (array[i] -> tipoEquip == ARMA)
                        printf("Arma\n");
                    else
                        printf("Armadura\n");
                }
                int num2;
                while (1) {
                    printf("Elige cual objeto quieres equipar (Si quieres cancelar esta accion, escribe \"SALIR\")\n");
                    if (verificarOpcionConSalir(&num2, contadorEquip) == 1) { // Verifica la opcion y el "SALIR"
                        break;
                    }
                    else {
                        equiparObjeto(P, array[num2-1]); // Equipa el objeto elegido
                        break;
                    }
                }
                free(array); // Limpia el array ya que no se va a seguir usando
                break;
            }
            case 6:
                return;
        }
    }
}

void infoJugador(Jugador *P) {
    puts("=======================");
    printf("Vida: %d/%d\n", P -> vidaActual, P -> vida);

    printf("Arma: ");
    P -> arma == NULL ? printf("Sin Arma\n") : printf("%s\n",P -> arma -> nombre);
    printf("Armadura: ");
    P -> armadura == NULL ? printf("Sin Armadura\n") : printf("%s\n", P -> armadura -> nombre);

    bool flag = false;
    puts("Habilidades disponibles:");
    for (int i = 0 ; i < 2 ; i++) {
        Skill *aux = P ->habilidades[i];
        if (aux != NULL) {
            printf("%s | Enfriamiento restante: %d\n", aux -> nombre, aux -> cooldownActual);
            flag = true;
        }
    }
    if (flag == false)
        puts("Sin habilidades disponibles.");

    puts("Efecto Activo:");
        printf("%s | Duracion: %d turnos \n", P -> efecto -> nombre, P -> efecto -> duracion);
    puts("=======================");
}

Jugador *inicializarJugador(char *str) { // Inicializa un jugador con el nombre dado
    Jugador *P = malloc(sizeof(Jugador)); // Inicializa una estructura Jugador y le asigna los valores preterminados
    P -> nombre = strdup(str);
    P -> vida = 200;
    P -> statsBase.vida = 200;
    P -> statsBase.ataque = 25;
    P -> statsBase.defensa = 10;
    P -> vidaActual = P -> vida;
    P -> ataque = 25;
    P -> defensa = 10;
    P -> nivel = 1;
    P -> arma = NULL;
    P -> armadura = NULL;
    P -> inventario = list_create();
    P -> habilidades[0] = NULL;
    P -> habilidades[1] = NULL;
    P -> efecto = NULL;
    P -> posicion.posX = 0;
    P -> posicion.posY = 0;
    return P;
}

void levelUp(Jugador *P) {
    recalcularStats(P);
    P -> vidaActual += 50;
    if ((P -> vidaActual) > (P -> vida))
        P -> vidaActual = P -> vida;
}

void recalcularStats(Jugador *P) {
    // Recalcula las stats bases
    P -> statsBase.vida = 200 * pow(1.03, P -> nivel);
    P -> statsBase.ataque = 25 * pow(1.03, P -> nivel);
    P -> statsBase.defensa = 10 * pow(1.03, P -> nivel);

    // Obtiene stats de objetos
    int sumaVida = 0, sumaAtaque = 0, sumaDefensa = 0;
    if (P -> arma != NULL) {
        sumaVida += P -> arma -> statBonus.vida;
        sumaAtaque += P -> arma -> statBonus.ataque;
        sumaDefensa += P -> arma -> statBonus.defensa;
    }
    if (P -> armadura != NULL) {
        sumaVida += P -> armadura -> statBonus.vida;
        sumaAtaque += P -> armadura -> statBonus.ataque;
        sumaDefensa += P -> armadura -> statBonus.defensa;
    }

    // Actualiza las stats principales
    P -> vida = P -> statsBase.vida + sumaVida;
    P -> ataque = P -> statsBase.ataque + sumaAtaque;
    P -> defensa = P -> statsBase.defensa + sumaDefensa;
}

//// INTERFAZ DE TESORO

void mostrarInfoItem(Item *item) {
    if (item == NULL) {
        printf("Item nulo.\n");
        Sleep(500);
        borrarLineas(2);
        return;
    }
    int cont = 2;
    printf("Has encontrado %s\n", item->nombre);
    printf("Descripción: %s\n", item->descripcion);
    puts("");
    // Mostrar si es consumible
    if (item->tipoCons == tipoPocion) {
        printf("Tipo: Poción\n");
        printf("Vida que recupera: %d\n", item->vidaRecuperada);
        cont += 2;
    }
    else if (item->tipoCons == libroDeHabilidad && item->habilidadAprendida != NULL) {
        printf("Tipo: Libro de Habilidad\n");
        Skill *h = item->habilidadAprendida;
        printf("Nombre habilidad: %s\n", h->nombre);
        printf("Tipo: %s\n", (h->tipo == curacion ? "Curación" : "Estado"));
        printf("Cooldown: %d turnos\n", h->cooldown);
        cont += 4;
        if (h->tipo == curacion) {
            printf("Vida que cura: %d\n", h->vidaCurada);
            cont++;
        } else if (h->estado != NULL) {
            printf("Estado aplicado: %s\n", 
                h->estado->tipo == vida ? "Vida" :
                h->estado->tipo == dano ? "Daño" :
                h->estado->tipo == defensa ? "Defensa" : "Saltar Turno");
            printf("Duración: %d turnos\n", h->estado->duracion);
            printf("Operación: %s %.2f\n", 
                h->estado->op == suma ? "Suma" : "Multiplica", 
                h->estado->cantidad);
            cont += 3;
        }
    }

    // Mostrar si es equipable
    if (item->tipoEquip == ARMA) {
        printf("Tipo: Arma\n");
        printf("Bono de Ataque: %d\n", item->statBonus.ataque);
        cont += 2;
    }
    else if (item->tipoEquip == ARMADURA) {
        printf("Tipo: Armadura\n");
        printf("Bono de Defensa: %d\n", item->statBonus.defensa);
        printf("Bono de Vida: %d\n", item->statBonus.vida);
        cont += 3;
    }
    esperarAccion();
    
    borrarLineas(cont + 2);
}

void interfazDeTesoro(Jugador *P, Item *I) {
    mostrarInfoItem(I);
    list_pushBack(P -> inventario, I);
}


//// MAIN

int main() {
    SetConsoleOutputCP(65001); // Consola en UTF-8
    SetConsoleCP(CP_UTF8);
    init_random();



    // Proceso de lectura de datos
    List *listaJefes = list_create();
    HashMap *mapaStatus = leer_status("data/Status.csv");
    List *listaSkills = leer_skills("data/Skills.csv", mapaStatus);
    multiMapa *mapaItems = leer_items("data/Items.csv", listaSkills);
    List *listaEnemigos = leer_Enemies("data/Enemies.csv", listaSkills, listaJefes);


    elGuerrero();
    printf("Bienvenido a la aventura del Guerrero más Bravo que hayas conocido\n");
    while(1) {
        printf("Elige una opción:\n1) Jugar\n2) Cómo jugar\n3) Salir\n");
        int coso;
        verificarOpcionConBorrado(&coso, 3);
        switch (coso) {
        case 1:{
            limpiarPantalla();
            printf("\n\nIngresa el nombre de tu guerrero:");
            char str[40];
            fgets(str, sizeof(str), stdin);
            if (str[strlen(str) - 1] != '\n') {
                limpiarSTDIN(); 
            }
            Jugador *player = inicializarJugador(str);
            //interfazComienzo(str);
            limpiarPantalla();
            float mult = 1; // Controla la dificultad del piso
            Escenario **nivelActual = crearMatriz(mapaItems, listaEnemigos, player, listaJefes, &mult);
            while(1) {
                mostrarNivel(player, nivelActual);
                movimientoMazmorra(player, nivelActual);
                int retorno = procesarTurno(player, nivelActual, &mult);
                if (retorno == 0) break;
                else
                    if (retorno == 1) {
                        nivelActual = crearMatriz(mapaItems, listaEnemigos, player, listaJefes, &mult);
                    }
                
            }
            printb("Tu viaje termina aquí, en las sombras de la mazmorra.\n Pero cada caída deja una enseñanza...");
            limpiarJuego(listaJefes, listaEnemigos, mapaStatus, mapaItems, player, nivelActual);
            break;
        }
        case 2: {
            comoJugar();
            break;
        }
        case 3:{
            limpiarJuego(listaJefes, listaEnemigos, mapaStatus, mapaItems, NULL, NULL);
            return 0;
        }
        return 0;
        }
    }
}

