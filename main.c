#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/multimapaItems.h"
#include "tipoDato.h"
#include "random.h"
#include "interfaces.h"
#include "miscelaneo.h"
#include "combate.h"

//// PROTOTIPO DE FUNCIONES

//// CLONADO

// Clona un enemigo aleatorio de una lista y le asigna el multiplicador correspondiente
Enemy *clonarEnemigo(List *L, int mult);

// Obtiene un item aleatorio del mapa de items
Item *obtenerItem(mapaItems, P);

//// LOOT

// asigna loot aleatorio a un enemigo segun la lista de items
void asignarLootAleatorio(Jugador *P, Enemy *enemigo, multiMapa *mapaItems);

// CREACION DEL NIVEL
Escenario **crearMatriz(multiMapa *, List *, Jugador *, List *, float *mult); // Crea la matriz 5x5 de Escenario que representara al nivel actual junto a sus parametros predeterminados, ademas de establecer una nueva posicion aleatoria del jugador.


// FUNCIONES DE LIMPIEZA
void limpiarListaEstado(List *L); // Limpia los elementos y nodos de una listas de estados
void limpiarListaHabilidades(List *L); // Limpia los elementos y nodos de una listas de habilidades
void limpiarEnemigo(Enemy *E); // Limpia los elementos de un enemigo
void borrarLibro(List *L, char *str); // Borra la primera ocurrencia de un libro en una lista
void limpiarPiso(Escenario ***S); // Obtiene un puntero a una matriz de Escenario (Escenario ***), limpia todos sus elementos (Escenario contiene punteros y datos estaticos, por lo que no es necesario limpiar mas adentro) y luego marca el dato como NULL


//// MOVIMIENTO EN LA MAZMORRA
void movimientoMazmorra(Jugador *P, Escenario **S); // Funcion encargada de mover al jugador en la direccion deseada
int procesarTurno(Jugador *P, Escenario **S, float *mult); // Procesa el turno, retornando un valor segun sea el estado de este
// RETURN
// 0: Muerte, fin del juego
// 1: Avanzar piso
// 2: No hacer nada
void mostrarNivel(Jugador *P, Escenario **S); // Muestra el estado del nivel actual


// RELACIONADAS A JUGADOR
Skill **crearArraySkills(List *, int *); // Crea un array que contenga las skills que se pueden aprender (usando los libros del inventario)
void aprenderSkill(Jugador *P, Skill *H); // Aprende una skill en el primer slot disponible o pregunta cual quiere reemplazar
void inventarioJugador(Jugador *P); // Muestra diferentes opciones al jugador y permite que este seleccione alguna
// 1) Ver todo el inventario
// 2) Ver objetos usables en combate
// 3) Ver habilidades aprendidas
// 4) Aprender habilidades
// 5) Salir del menu
void infoJugador(Jugador *P); // Muestra la informacion basica acerca del jugador (vida, habilidades, arma y armadura equipadas y los efectos activos)
void levelUp(Jugador *P); // aplica las mejoras al jugador


//// INTERFAZ DE TESORO
void interfazDeTesoro(Jugador *, Item *);


//// PARA TESTEO

Item *crearItemPrueba() {
    //nombre,tipoConsumible{noConsumible|libroDeHabilidad|tipoPocion},tipoEquipable{noEquipable|arma|armadura},{vida;daño;defensa},vidaRecuperada,habilidadAprendida,descripción
    //Pan con Fiambre,tipoPocion,noEquipable,0;0;0,10,NULL,"Te curas 10 puntos de vida, no hay suficientes descripciones chistosas"
    Item *I = malloc(sizeof(Item));
    I -> nombre = strdup("Pan con Fiambre");
    I -> tipoCons = tipoPocion;
    I -> tipoEquip = noEquipable;
    I -> statBonus.vidaBonus = 0;
    I -> statBonus.AtaqueBonus = 0;
    I -> statBonus.DefensaBonus = 0;
    I -> vidaRecuperada = 10;
    I -> habilidadAprendida = NULL;
    I -> descripcion = strdup("Te curas 10 puntos de vida, no hay suficientes descripciones chistosas");
    return I;
} 

Enemy *crearEnemigoPrueba() {
    //nombre,{vida;ataque;defensa},loot,esJefe(bool),habilidades
    //Slime,40;8;2,NULL,False,NULL
    Enemy *E = malloc(sizeof(Enemy));
    E -> nombre = strdup("Goblin");
    E -> vida = 40;
    E -> vidaActual = 40;
    E -> ataque = 10;
    E -> defensa = 2;
    E -> loot = NULL;
    E -> arma = NULL;
    E -> esJefe = false;
    E -> efecto = NULL;
    return E;
} 

Enemy *crearJefePrueba() {
    //nombre,{vida;ataque;defensa},loot,esJefe(bool),habilidades
    //Slime,40;8;2,NULL,False,NULL
    Enemy *E = malloc(sizeof(Enemy));
    E -> nombre = strdup("Goblin");
    E -> vida = 40;
    E -> vidaActual = 40;
    E -> ataque = 10;
    E -> defensa = 2;
    E -> loot = NULL;
    E -> arma = NULL;
    E -> esJefe = false;
    E -> efecto = NULL;
    return E;
} 

//// CLONADO

Enemy *clonarEnemigo(List *L, int mult) {
    int pos = randomRint(1, list_size(L)); // Se elige una posicion random de la lista
    Enemy *actual = list_get(L, pos);
    Enemy *E = malloc(sizeof(Enemy));
    E -> nombre = strdup(actual -> nombre); // Se clona el elemento
    E -> vida = actual -> vida * mult;
    E -> vidaActual = actual -> vidaActual * mult;
    E -> ataque = actual -> ataque * mult;
    E -> defensa = actual -> defensa * mult;
    E -> loot = NULL;
    E -> arma = NULL;
    E -> esJefe = actual -> esJefe;
    E -> efecto = actual -> efecto;
    for (int i = 0 ; i < 3 ; i++) {
        E -> habilidades[i] = copiaSkill(actual -> habilidades[i]);
    }
    return E;
}

Item *obtenerItem(mapaItems, P) {
    int poderAleatorio = randomRint(1, powerIndexPlayer(P)); // Indica la posicion aleatoria entre el rango 1 y el poder maximo que puede obtener el jugador actualmente
    multiPar *actual = buscarMultiMapa(mapaItems, poderAleatorio); // Busca si existen objetos en ese poder
    if(actual == NULL) {
        actual = anteriorMultiMapa(mapaItems);
        if (actual == NULL) return NULL; // No deberia entrar nunca aqui ya que existen items poder 1
    }
    List *auxLista = actual -> values; // Obtiene la lista de elementos
    Item *auxItem = list_get(auxLista, randomRint(1, list_size(auxLista))); // Obtiene un indice aleatorio de la lista de elementos
    return auxItem;
}


//// LOOT

void asignarLootAleatorio(Jugador *P, Enemy *enemigo, multiMapa *mapaItems) {
    if (!enemigo || !mapaItems || mapaItems -> size == 0) return;

    int chance = randomRint(1, 10); // genera numero aleatorio entre 1 y 100 para determinar si no deja loot (10% probabilidad de tener loot)
    if (chance == 1) {
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
    Item *auxItem = list_get(auxLista, randomRint(1, list_size(auxLista))); // Obtiene un indice aleatorio de la lista de elementos
    enemigo -> loot = auxItem;
    return;
}


//// CREACION DEL NIVEL

Escenario **crearMatriz(multiMapa *mapaItems, List *listaEnemigos, Jugador *P, List *listaJefes, float *mult) {
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
                    matriz[i][j].enemigo = clonarEnemigo(listaEnemigos, mult);
                    asignarLootAleatorio(P, matriz[i][j].enemigo, mapaItems);
                    matriz[i][j].objeto = NULL;
                    break;
            
                case ITEM:
                    //matriz[i][j].objeto = obtenerItem(mapaItems, P);
                    matriz[i][j].objeto = crearItemPrueba();
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
    //matriz[indX][indY].enemigo = clonarJefe(listaJefes, mult); 
    matriz[indX][indY].enemigo = crearJefePrueba();
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
    free(S);
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

    limpiarEstado(E -> efecto); // Limpia la lista de estados
    free(E -> efecto);
    E -> efecto = NULL;

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
                borrarLineas(2);
                puts("Mala suerte, esta sala esta vacia.");
                Sleep(500);
                borrarLineas(1); 
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
                } else
                    if (disc == 0) { // Si murio
                        pantallaGameOver();
                    }
                break;
            }
            case ITEM:
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
                else
                    printf("X ");
                   
            }
        }
        puts("");
    }
    puts("\nJ = Jugador, O = Visitado, X = No Visitado");
    Sleep(500);
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

void inventarioJugador(Jugador *P) {
    int num, cont;
    bool flag = false;
    while(1) {
        puts("Selecciona una opcion:");
        puts("1) Ver todo el inventario");
        puts("2) Ver objetos usables en combate");
        puts("3) Ver habilidades aprendidas");
        puts("4) Aprender habilidades");
        puts("5) Volver al movimiento en la mazmorra");
        verificarOpcionConBorrado(&num, 5);
        borrarLineas(7);
        flag = false;
        cont = 0;
        switch (num) {
            case 1: {// 1) Ver todo el inventario
                
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
                Sleep(500);
                borrarLineas(cont + 1);
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
                borrarLineas(cont + 1);
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
                borrarLineas(cont + 1);
                break;
            }
            case 4: { // 4) Aprender habilidades
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
            }
            case 5:
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
        printf("Bono de Ataque: %d\n", item->statBonus.AtaqueBonus);
        cont += 2;
    }
    else if (item->tipoEquip == ARMADURA) {
        printf("Tipo: Armadura\n");
        printf("Bono de Defensa: %d\n", item->statBonus.DefensaBonus);
        cont += 2;
    }
    esperarAccion();
    
    borrarLineas(cont + 2);
}

void interfazDeTesoro(Jugador *P, Item *I) {
    borrarLineas(5);
    mostrarInfoItem(I);
    list_pushBack(P -> inventario, I);
}


//// MAIN

int main() {
    SetConsoleOutputCP(65001); // Consola en UTF-8
    SetConsoleCP(CP_UTF8);
    init_random();


    elGuerrero();
    printf("Bienvenido a la aventura del Guerrero más Bravo que hayas conocido\n");
    printf("Menú Principal Beta\n");
    printf("Elige una opción:\n1) Jugar\n2) Cómo jugar\n3) Salir\n");
    int coso;
    verificarOpcionConBorrado(&coso, 3);
    switch (coso)
    {
    case 1:{
        limpiarPantalla();
        Jugador player;
        printf("\n\nIngresa el nombre de tu guerrero:");
        char str[40];
        fgets(str, sizeof(str), stdin);
        limpiarSTDIN();
        strcpy(player.nombre,str);
        player.vida = 100;
        player.vidaActual = player.vida;
        player.ataque = 20;
        player.defensa = 10;
        player.nivel = 1;
        player.arma = NULL;
        player.armadura = NULL;
        player.inventario = list_create();
        player.habilidades[0] = NULL;
        player.habilidades[1] = NULL;
        player.efecto = list_create();
        player.posicion.posX = 0;
        player.posicion.posY = 0;
        interfazComienzo(str);
        limpiarPantalla();
        Escenario **nivelActual = crearMatriz(&player);
        float mult = 1;
        while(1) {
            mostrarNivel(&player, nivelActual);
            movimientoMazmorra(&player, nivelActual);
            procesarTurno(&player, nivelActual, &mult);
        }
        break;
    }
    case 3:{
        break;
    }
    return 0;
}
}

//void leer_status(List *listaStatus);

//void leer_skills(List *listaSkills);

//void leer_items(List *listaItems);

//void leer_Enemies(List *listaEnemigos);