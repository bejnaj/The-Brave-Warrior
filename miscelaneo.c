#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void limpiarSTDIN() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void verificarOpcion(int *num, int limite) {
    char str[3];
    while (1) {
        fgets(str, sizeof(str), stdin);
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

void verificarOpcionConBorrado(int *num, int limite) {
    char str[3];
    while (1) {
        fgets(str, sizeof(str), stdin);
        if (str[strlen(str) - 1] != '\n') { //Se revisa si el usuario escribio mas de 2 caracteres
            limpiarSTDIN(); // Limpiar stdin para leer correctamente el proximo input
            printf("\033[F");
            printf("\033[2K");
        }
        else {
            if (isdigit(str[0]) && str[1] == '\n') { //En caso de que el numero ingresado no sea valido
                *num = str[0] - '0';
                if (*num > 0 && *num <= limite) {
                    printf("\033[F");
                    printf("\033[2K");
                    break;
                }
            }
        }
    }
}
    

int verificarOpcionConSalir(int *num, int limite) {
    // return
    // 0: No contiene la palabra clave "SALIR"
    // 1: Contiene la palabra clave 
    char str[7];
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