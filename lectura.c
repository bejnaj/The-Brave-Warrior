#include "lectura.h"

Map *leer_status() {
  FILE *archivo = fopen("data/Status.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }
  Map *mapaStatus = map_create(estadoEqual);
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
    if (strcmp(campos[1], "Vida") == 0)
        Actual->tipo = vida;
    else if (strcmp(campos[1], "daño") == 0)
        Actual->tipo = dano;
    else if (strcmp(campos[1], "defensa") == 0)
        Actual->tipo = defensa;
    else if (strcmp(campos[1], "saltarTurno") == 0)
        Actual->tipo = saltarTurno;
    Actual->costeTurnos = atoi(campos[3]);
    if (strcmp(campos[4], "suma") == 0) Actual->op = suma;
    else if (strcmp(campos[4], "mult") == 0) Actual->op = multiplicacion;
    Actual->cantidad = atof(campos[5]);
    // Agrega el Status al mapa
    int *key = malloc(sizeof(int));
    *key = Actual->id;
    map_insert(mapaStatus, key, Actual);
  }
  fclose(archivo); 
}

List *leer_skills(List *listaStatus) {
  FILE *archivo = fopen("data/Skills.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo");
    return;
  }
  List *listaSkills = list_create();

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
        Status *estado = (Status *)list_find(listaStatus, &idEstado, cmpID);
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

multiMapa *leer_items(List *listaItems, List *listaSkills) {
  FILE *archivo = fopen("data/Items.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }


  multiMapa *mapaItems = crearMultiMapa(200); // Crea el multimapa de items con capacidad para 200 elementos

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
      Skill *found = (Skill *)list_find(listaSkills, campos[5], cmpSkill);
      Actual->habilidadAprendida = found;
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
    // Calcula el poder del item
    Actual -> poder = powerIndex(Actual);

    // Agrega el item al mapa
    insertarMultiMapa(mapaItems, Actual -> poder, Actual);
  }
  fclose(archivo);
  return mapaItems;
}

List *leer_Enemies(List *listaItems, List *listaSkills) {
  FILE *archivo = fopen("data/Enemies.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return;
  }
  List *listaEnemigos = list_create(); // Crea la lista de enemigos

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
    for (int j = 0; j < 3; j++) Actual->habilidades[j] = NULL;

    // Asigna memoria para el nombre del enemigo
    Actual->nombre = strdup(campos[0]);
    sscanf(campos[1], "%d;%d;%d", &Actual->vida, &Actual->ataque, &Actual->defensa);
    //asignarLootAleatorio(Actual, listaItems);
    Actual->esJefe = (strcmp(campos[3], "True") == 0) ? true : false;
    // Procesa habilidades (puede haber hasta 3 separadas por ';')
    if (strcmp(campos[4], "NULL") != 0) {
      List* L = split_string(campos[4], ";");
      int i = 0;
      for (char *subcampoActual = list_first(L) ; subcampoActual != NULL && i < 3 ; subcampoActual = list_first(L)) { // parsea y copia cada habilidad
        Skill *aux = (Skill *)list_find(listaSkills, subcampoActual, cmpSkill);
        Actual -> habilidades[i] = copiaSkill(aux);
        free(list_popFront(L));
        i++;
      }
    }
    // Agrega el enemigo a la lista
    list_pushFront(listaEnemigos, Actual);
  }
  fclose(archivo);
  return listaEnemigos;
}
