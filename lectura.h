#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "tipoDato.h"
#include "tdas/multimapaItems.h"
#include "balance.h"
#include "miscelaneo.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

HashMap *leer_status(char *str);

List *leer_skills(char *str, HashMap *mapaEstados);

multiMapa *leer_items(char *str, List *listaSkills);

List *leer_Enemies(char *str, List *listaSkills);

// Obtiene los enemigos jefes de una lista de enemigos
List *obtenerJefes(List *listaEnemigos);