#include <stdio.h>
#include <stdbool.h>
#include <tdas/list.h>
#include <tdas/extra.h>
#include <tdas/map.h>
#include <stdlib.h>
#include <tipoDato.h>

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

    // Una vez terminados todos los escenarios, elegir el jefe aleatoriamente
    int aux = rand() % 25; // Obtener numero random entre 0-24
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
        aux = rand() % 25; // Obtener numero random entre 0-24
        indX = aux / 5; // Posicion en X
        indY = aux % 5; // Posicion en Y
    } while (jefeX == indX && jefeY == indY);
    P -> posicion.posX = indX;
    P -> posicion.posY = indY;
    return matriz;
}

void limpiarSTDIN() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void verificarOpcion(int *num, int limite) { // Verifica que una opcion numerica este entre 1 y "limite"
    char str[3];
    limpiarSTDIN();
    while (1) {
        fgets(str, 3, stdin);
        if (str[strlen(str) - 1] != '\n') { //Se revisa si el usuario escribio mas de 2 caracteres
            limpiarSTDIN(); // Limpiar stdin para leer correctamente el proximo input
        } 
        else {
            if (isdigit(str[0]) && str[1] == '\n') { //En caso de que el numero ingresado no sea valido
                *num = str[0] - '0';
                if (*num > 0 && *num <= limite) break;
            }
        }
        puts("Ingresa una opcion Valida");
    }
}

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

int procesarTurno(Jugador *P, Escenario **S) {
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
                if (interfazDeCombate(P, aux -> enemigo)) // Suponiendo que retorna 1 si fue victoria y 0 si fue derrota
                    break;
                else
                    return 0;

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
        puts("Este escenario ya fue visitado previamente.");
    return 2;
}

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

int verificarOpcionConSalir(int *num, int limite) { // Verifica que una opcion numerica este entre 1 y "limite" o contenga palabra clave "SALIR"
    char str[7];
    limpiarSTDIN();
    while (1) {
        fgets(str, sizeof(str), stdin);

        if (str[strlen(str) - 1] != '\n') { //Se revisa si el usuario escribio mas de 2 caracteres
            limpiarSTDIN(); // Limpiar stdin para leer correctamente el proximo input
        }
        str[strcspn(str, "\n")] = '\0'; // Quita el salto de linea

        if (strcmp(str, "SALIR") == 0) return 1; // Contiene palabra clave
    
         // Verificar si es un número válido
        if (strlen(str) == 1 && isdigit(str[0])) {
            *num = str[0] - '0';
            if (*num > 0 && *num <= limite) {
                break;
            }
        }
        puts("Ingresa una opcion Valida");
    }
    return 0; // No contiene palabra clave
}

void borrarLibro(Jugador *P, char *nombreSkill) {
    for (Item *actual = list_first(P -> inventario); actual != NULL ; actual = list_next(P -> inventario)) {
        if (actual -> habilidadAprendida != NULL && strcmp(actual -> habilidadAprendida -> nombre, nombreSkill) == 0) {
            list_popCurrent(P -> inventario);
            break;
        }
    }
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

void inventarioJugador (Jugador *P) {
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
                    }
                    
                }
                free(array); // Limpia el array ya que no se va a seguir usando
                break;

            case 5:
            return;
        }
    }
}

int main() {
    return 1;
}