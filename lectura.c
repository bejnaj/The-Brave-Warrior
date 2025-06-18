#include <tdas/extra.h>
#include <tdas/list.h>
#include <tdas/map.h>
#include <tipoDato.h>

void leer_status(List *listaStatus) {
  FILE *archivo = fopen("data/Status.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo");
    return;
  }

  // lee los nombres de cada columna
  char **campos;
  campos = leer_linea_csv(archivo, ','); 

  // lee la linea correspondiente a una sala
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // guarda espacio para los datos del Status actual
    Status *Actual = malloc(sizeof(Status));
    if (Actual == NULL) {
        perror("Error al asignar memoria para el escenario");
        continue;
    }
    // Asigna memoria para el ID del Status
    int *idEscenario = malloc(sizeof(int));
    Actual->id = atoi(campos[0]);
    // Que tipo de efecto posee
    char *efectoDeStatus = strdup(campos[1]);
    if (strcmp(efectoDeStatus, "Vida"))
        Actual->tipo = 0;
    else if (strcmp(efectoDeStatus, "daño"))
        Actual->tipo = 1;
    else if (strcmp(efectoDeStatus, "defensa"))
        Actual->tipo = 2;
    else if (strcmp(efectoDeStatus, "saltarTurno"))
        Actual->tipo = 3;
    else {
        break; // Si no es un tipo de status valido, salimos del bucle
    }
    Actual->costeTurnos = atoi(campos[3]);
    char *TipoOperacion = strdup(campos[4]);
    if (strcmp(campos[4], "suma") == 0) Actual->op = 0;
    else if (strcmp(campos[4], "mult") == 0) Actual->op = 1;
    else break; // Si no es un tipo de operacion valido, salimos del bucle
      Actual->cantidad = atof(campos[5]);
  }
  fclose(archivo); 
}

