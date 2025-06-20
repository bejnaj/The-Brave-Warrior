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
    Actual->id = atoi(campos[0]);
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
    Skill *Actual = malloc(sizeof(Skill));
    if (Actual == NULL) {
        perror("Error al asignar memoria para la habilidad");
        continue;
    }
    // Asigna memoria para el nombre de la habilidad
    Actual->nombre = strdup(campos[0]);
    // Duracion de Cooldown para la habilidad
    Actual->cooldown = atoi(campos[1]);
    // Duracion de la habilidad
    Actual->duracion = atoi(campos[2]);
    if (strcmp(campos[3], "curacion") == 0) Actual->tipo = curacion;
    else if (strcmp(campos[3], "estado") == 0) Actual->tipo = estado;
    Actual->vidaCurada = atoi(campos[4]);
    Actual->estado = NULL; // Puedes implementar la carga de estados si tienes la estructura
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
    Item *Actual = malloc(sizeof(Item));
    if (Actual == NULL) {
        perror("Error al asignar memoria para la habilidad");
        continue;
    }
    // Asigna memoria para el nombre del item
    Actual->nombre = strdup(campos[0]);
    // Como se usara el item
    if (strcmp(campos[1], "noConsumible") == 0) Actual->tipoCons = noConsumible;
    else if (strcmp(campos[1], "libroDeHabilidad") == 0) Actual->tipoCons = libroDeHabilidad;
    else if (strcmp(campos[1], "tipoPocion") == 0) Actual->tipoCons = tipoPocion;
    // Donde se equipara el item
    if (strcmp(campos[2], "noEquipable") == 0) Actual->tipoEquip = noEquipable;
    else if (strcmp(campos[2], "arma") == 0) Actual->tipoEquip = arma;
    else if (strcmp(campos[2], "armadura") == 0) Actual->tipoEquip = armadura;
    // Asigna los bonus del item (separado por ';')
    int vida = 0, ataque = 0, defensa = 0;
    sscanf(campos[3], "%d;%d;%d", &vida, &ataque, &defensa);
    Actual->statBonus.vidaBonus = vida;
    Actual->statBonus.AtaqueBonus = ataque;
    Actual->statBonus.DefensaBonus = defensa;
    // Asigna la vida recuperada por el item
    Actual->vidaRecuperada = atoi(campos[4]);
    // Asigna la descripcion del item
    char *comillaInicio = strchr(campos[6], '"');
    char *comillaFin = NULL;
    if (comillaInicio) {
        comillaInicio++;
        comillaFin = strchr(comillaInicio, '"');
    }
    if (comillaInicio && comillaFin) {
        size_t len = comillaFin - comillaInicio;
        Actual->descripcion = malloc(len + 1);
        strncpy(Actual->descripcion, comillaInicio, len);
        Actual->descripcion[len] = '\0';
    } else {
        Actual->descripcion = strdup(campos[6]);
    }
    // Agrega el item a la lista
    list_pushFront(listaItems, Actual);
  }
  fclose(archivo); 
}

void leer_Enemies(List *listaEnemigos) {
  FILE *archivo = fopen("data/Enemies.csv", "r");
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
    Enemy *Actual = malloc(sizeof(Enemy));
    if (Actual == NULL) {
        perror("Error al asignar memoria para el enemigo");
        continue;
    }
    // Asigna memoria para el nombre del enemigo
    Actual->nombre = strdup(campos[0]);
    sscanf(campos[1], "%d;%d;%d", &Actual->vida, &Actual->ataque, &Actual->defensa);
    Actual->loot = NULL; // Puedes implementar la carga de loot si tienes la estructura
    Actual->esJefe = (strcmp(campos[3], "True") == 0) ? true : false;
    Actual->habilidades = strdup(campos[4]); 
    // Agrega el enemigo a la lista
    list_pushFront(listaEnemigos, Actual);
  }
  fclose(archivo); 
}
