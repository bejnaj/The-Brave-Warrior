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
    Actual->costeTurnos = atoi(campos[3]);
    char *TipoOperacion = strdup(campos[4]);
    if (strcmp(campos[4], "suma") == 0) Actual->op = 0;
    else if (strcmp(campos[4], "mult") == 0) Actual->op = 1;
    Actual->cantidad = atof(campos[5]);
    // Agrega el Status a la lista
    list_pushFront(listaStatus, Actual);

  }
  fclose(archivo); 
}

void leer_skills(List *listaSkills) {
  FILE *archivo = fopen("data/Skills.csv", "r");
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
    Skill *Actual = malloc(sizeof(Skill));
    if (Actual == NULL) {
        perror("Error al asignar memoria para la habilidad");
        continue;
    }
    // Asigna memoria para el nombre de la habilidad
    Actual->nombre = atoi(campos[0]);
    // Duracion de Cooldown para la habilidad
    Actual->duracion = strdup(campos[1]);
    // Duracion de la habilidad
    Actual->duracion = strdup(campos[2]);
    tipoSkill tipoHabilidad = strdup(campos[3]);
    if (strcmp(tipoHabilidad, "curacion") == 0) Actual->tipo = 0;
    else if (strcmp(tipoHabilidad, "estado") == 0) Actual->tipo = 1;
    Actual->vidaCurada = atoi(campos[4]);
    Actual->estado = atoi(campos[5]);
    Actual->hacia = atoi(campos[6]);
    // Agrega la habilidad a la lista
    list_pushFront(listaSkills, Actual);
  }
  fclose(archivo); 
}

void leer_items(List *listaItems) {
  FILE *archivo = fopen("data/Items.csv", "r");
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
    Item *Actual = malloc(sizeof(Item));
    if (Actual == NULL) {
        perror("Error al asignar memoria para la habilidad");
        continue;
    }
    // Asigna memoria para el nombre de la habilidad
    Actual->nombre = atoi(campos[0]);
    // Como se usara el item
    tipoConsumible tipoConsumibleItem = strdup(campos[1]);
    if (strcmp(tipoConsumibleItem, "consumible") == 0) Actual->tipoCons = 0;
    else if (strcmp(tipoConsumibleItem, "libroHabilidad") == 0) Actual->tipoCons = 1;
    else if (strcmp(tipoConsumibleItem, "pocion") == 0) Actual->tipoCons = 2;
    // Donde se equipara el item
    tipoEquipable tipoEquipableItem = strdup(campos[2]);
    if (strcmp(tipoEquipableItem, "noEquipable") == 0) Actual->tipoEquip = 0;
    else if (strcmp(tipoEquipableItem, "arma") == 0) Actual->tipoEquip = 1;
    else if (strcmp(tipoEquipableItem, "armadura") == 0) Actual->tipoEquip = 2;
    // Asigna los bonus del item
    Bonus *statBonus = malloc(sizeof(Bonus));
    if (statBonus == NULL) {
        perror("Error al asignar memoria para los bonus del item");
        continue;
    }
    statBonus->vidaBonus = atoi(campos[3]);
    statBonus->AtaqueBonus = atoi(campos[4]);
    statBonus->DefensaBonus = atoi(campos[5]);
    Actual->statBonus = *statBonus;
    // Asigna la vida recuperada por el item
    Actual->vidaRecuperada = atoi(campos[6]);
    // Asigna la descripcion del item
    char *comillaInicio = strchr(campos[7], '"');
    char *comillaFin = NULL;
    // Lee el final de la comilla
    if (comillaInicio) {
        comillaInicio++;
        comillaFin = strchr(comillaInicio, '"');
    }
    // saca el tamaño de la descripcion y asigna memoria para esta, guardandola
    if (comillaInicio && comillaFin) {
        size_t len = comillaFin - comillaInicio;
        Actual->descripcion = malloc(len + 1);
        strncpy(Actual->descripcion, comillaInicio, len);
        Actual->descripcion[len] = '\0';
    }
    // Agrega el item a la lista
    list_pushFront(listaItems, Actual);
    
  }
  fclose(archivo); 
}

