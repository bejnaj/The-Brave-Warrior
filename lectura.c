#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include "tipoDato.h"

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
        Actual->tipo = vida;
    else if (strcmp(efectoDeStatus, "daño"))
        Actual->tipo = daño;
    else if (strcmp(efectoDeStatus, "defensa"))
        Actual->tipo = defensa;
    else if (strcmp(efectoDeStatus, "saltarTurno"))
        Actual->tipo = saltarTurno;
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

int cmpId(void *a, void *b) {
    Status *sa = (Status *)a;
    int *id = (int *)b;
    return sa->id - *id;
}

void leer_skills(List *listaSkills, List *listaStatus) {
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
    Status *estado = NULL;
    if (strcmp(campos[5], "NULL") != 0) {
        int idEstado = atoi(campos[5]);
        Status *estado = (Status *)list_find(listaStatus, &idEstado, cmpId);
        Actual->estado = estado;
    } else {
        Actual->estado = NULL;
    }
    Actual->hacia = atoi(campos[6]);
    // Agrega la habilidad a la lista
    list_pushFront(listaSkills, Actual);
  }
  fclose(archivo); 
}

int cmpSkill(void *a, void *b) {
    Skill *sa = (Skill *)a;
    char *nombre = (char *)b;
    return strcmp(sa->nombre, nombre);
}

void leer_items(List *listaItems, List *listaSkills) {
  FILE *archivo = fopen("data/Items.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }

  // lee los nombres de cada columna
  char **campos;
  campos = leer_linea_csv(archivo, ','); 

  // lee la linea correspondiente a un item
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Item *Actual = malloc(sizeof(Item));
    if (Actual == NULL) {
        perror("Error al asignar memoria para el item");
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
    else if (strcmp(campos[2], "arma") == 0) Actual->tipoEquip = ARMA;
    else if (strcmp(campos[2], "armadura") == 0) Actual->tipoEquip = ARMADURA;
    
    // Asigna los bonus del item (separado por ';')
    int vida = 0, ataque = 0, defensa = 0;
    sscanf(campos[3], "%d;%d;%d", &vida, &ataque, &defensa);
    Actual->statBonus.vidaBonus = vida;
    Actual->statBonus.AtaqueBonus = ataque;
    Actual->statBonus.DefensaBonus = defensa;
    // Asigna la vida recuperada por el item
    Actual->vidaRecuperada = atoi(campos[4]);
    // Enlaza la habilidad aprendida si corresponde
    if (strcmp(campos[5], "NULL") == 0) {
        Actual->habilidadAprendida = NULL;
    } else {
        // Buscar la skill por nombre en listaSkills
        Skill *skill = NULL;
        if (strcmp(campos[5], "NULL") == 0) {
            Actual->habilidadAprendida = NULL;
        } else {
            Skill *found = (Skill *)list_find(listaSkills, campos[5], cmpSkill);
            Actual->habilidadAprendida = found;
        }
    }
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

void leer_Enemies(List *listaEnemigos, List *listaItems, List *listaSkills) {
  FILE *archivo = fopen("data/Enemies.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }

  // lee los nombres de cada columna
  char **campos;
  campos = leer_linea_csv(archivo, ',');

  // lee la linea correspondiente a un enemigo
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Enemy *Actual = malloc(sizeof(Enemy));
    if (Actual == NULL) {
        perror("Error al asignar memoria para el enemigo");
        continue;
    }
    // Asigna memoria para el nombre del enemigo
    Actual->nombre = strdup(campos[0]);
    sscanf(campos[1], "%d;%d;%d", &Actual->vida, &Actual->ataque, &Actual->defensa);
    asignarLootAleatorio(Actual, listaItems);
    Actual->esJefe = (strcmp(campos[3], "True") == 0) ? true : false;
    // Procesa habilidades (puede haber varias separadas por ';')
    Actual->habilidades = list_create();
    if (strcmp(campos[4], "NULL") != 0) {
        char *habilidadesStr = strdup(campos[4]);
        char *token = strtok(habilidadesStr, ";");
        while (token != NULL) {
            Skill *habilidadActual = (Skill *)list_find(listaSkills, token, cmpSkill);
            if (habilidadActual) list_pushBack(Actual->habilidades, habilidadActual);
            token = strtok(NULL, ";");
        }
        free(habilidadesStr);
    }
    // Agrega el enemigo a la lista
    list_pushFront(listaEnemigos, Actual);
  }
  fclose(archivo); 
}
