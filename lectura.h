#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include "tipoDato.h"
#include "tdas/multimapaItems.h"
#include "balance.h"
#include "miscelaneo.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

Map *leer_status();

List *leer_skills();

multiMapa *leer_items(List *listaItems, List *listaSkills);

List *leer_Enemies(List *listaItems, List *listaSkills);