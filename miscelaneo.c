#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>
#include <stdarg.h>
#include "tipoDato.h"

void limpiarSTDIN() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void verificarOpcion(int *num, int limite) {
    char str[20]; // Espacio suficiente para números grandes
    while (1) {
        fgets(str, sizeof(str), stdin);

        // Limpiar buffer si input excede el tamaño
        if (str[strlen(str) - 1] != '\n') {
            limpiarSTDIN(); 
        }

        // Intentar convertir a número
        char *endptr;
        long val = strtol(str, &endptr, 10);

        // Verifica si se convirtió correctamente y no hay caracteres extra
        if (endptr != str && *endptr == '\n') {
            if (val > 0 && val <= limite) {
                *num = (int)val;
                break;
            }
        }

        puts("Ingresa una opción válida");
    }
}

void verificarOpcionConBorrado(int *num, int limite) {
    char str[3];
    while (1) {
        fgets(str, sizeof(str), stdin);
        if (str[strlen(str) - 1] != '\n') { // Se revisa si el usuario escribió más de 2 caracteres
            limpiarSTDIN(); // Limpiar stdin para leer correctamente el próximo input
            printf("\033[F");
            printf("\033[2K");
        }
        else {
            if (isdigit(str[0]) && str[1] == '\n') { // Si el número ingresado es válido
                *num = str[0] - '0';
                if (*num > 0 && *num <= limite) {
                    break; // No borra nada si es correcto
                }
            }
            // Si no es válido, borra la línea
            printf("\033[F");
            printf("\033[2K");
        }
    }
}
    
int verificarOpcionConSalir(int *num, int limite) {
    // return
    // 0: No contiene la palabra clave "SALIR"
    // 1: Contiene la palabra clave 
     char str[30]; // Espacio suficiente para entrada
    while (1) {
        fgets(str, sizeof(str), stdin);

        // Limpiar buffer si input excede el tamaño
        if (str[strlen(str) - 1] != '\n') {
            limpiarSTDIN();
        }

        // Eliminar salto de línea al final
        str[strcspn(str, "\n")] = '\0';

        // Convertir a mayúsculas para comparar con "SALIR"
        char upperStr[30];
        for (int i = 0; str[i] && i < 29; i++)
            upperStr[i] = toupper((unsigned char)str[i]);
        upperStr[strlen(str)] = '\0';

        if (strcmp(upperStr, "SALIR") == 0) {
            return 1; // Contenia palabra clave
        }

        // Intentar convertir a número
        char *endptr;
        long val = strtol(str, &endptr, 10);
        if (endptr != str && *endptr == '\0') {
            if (val > 0 && val <= limite) {
                *num = (int)val;
                return 0; // Entrada numérica válida
            }
        }

        puts("Ingresa un número válido o escribe 'SALIR' para salir.");
    }
}


void esperarAccion() {
    puts("Presione enter para continuar");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    puts(""); 
}

void printcv(int vel,const char *fmt, ...) { //printb con velocidad "vel".
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    for (int i = 0; buffer[i] != '\0'; i++) {
        putchar(buffer[i]);
        fflush(stdout);
        Sleep(vel); // Tiempo de espera entre caracteres en milisegundos (ajusta a tu gusto)
    }
}

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

void borrarLineas(int x) {
    for (int i = 0; i < x; i++) {
        printf("\033[F"); // Cursor una línea arriba
        printf("\033[2K"); // Borra la línea completa
    }
}

Skill *copiaSkill(Skill *elem) {
    Skill *copia = malloc(sizeof(Skill));
    copia -> nombre = elem -> nombre;
    copia -> tipo = elem -> tipo;
    copia -> duracion = elem -> duracion;
    copia -> cooldown = elem -> cooldown;
    copia -> cooldownActual = 0;
    copia -> vidaCurada = elem -> vidaCurada;
    copia -> estado = elem -> estado;
    copia -> hacia = elem -> hacia;
    return copia;
}

int cmpSkill(Skill *a, char *b) {
    Skill *sa = (Skill *)a;
    char *nombre = (char *)b;
    return strcmp(sa->nombre, nombre);
}

int cmpID(Status *a, char *b) {
    Status *sa = (Status *)a;
    int *id = (int *)b;
    return sa->id - *id;
}

int estadoEqual(Status *k1, Status *k2) {
    return k1 == k2;
}