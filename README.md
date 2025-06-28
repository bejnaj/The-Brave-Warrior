# The Brave Warrior
The Brave Warrior es un juego de rol tipo dungeon crawler por turnos, donde el jugador explora una mazmorra llena de enemigos, jefes, objetos y habilidades. El objetivo es sobrevivir, mejorar al personaje y avanzar por los distintos pisos, enfrentando desafíos cada vez mayores. Todos los datos del juego (enemigos, objetos, habilidades, estados) se cargan dinámicamente desde archivos CSV, permitiendo una fácil personalización y expansión.

# TDA's
El juego utiliza estructuras de datos propias (listas, hashmaps, multimapas) para gestionar inventarios, enemigos, habilidades y más. El sistema de combate es por turnos, con efectos de estado, loot aleatorio y progresión de personaje.

# Importante
El juego funciona en terminales que soportan ANSI escape codes (Linux, macOS, y algunas consolas modernas de Windows 10/11). Si es la consola clásica de Windows (cmd.exe antiguo), puede que no funcione correctamente a menos que esté habilitado el soporte ANSI. 

En Windows 10/11: la terminal de VS Code, PowerShell y el nuevo CMD suelen soportar ANSI por defecto.
En Windows antiguos: el CMD clásico puede que no soporte estos códigos.
En Linux/macOS: funciona sin problemas.

En el sistema de combate, existe la pequeña probabilidad de que haya un problema con el ingreso de opciones. Por ejemplo que se seleccione automaticamente 1) en el primer menú y que automáticamente te mande al menú de elegir una opción de ataque.

Para compilar escribir en terminal "gcc *.c tdas/*.c -o bravewarrior.exe -lm
./bravewarrior.exe" en Windows o "gcc *.c tdas/*.c -o bravewarrior -lm
./bravewarrior" en Linux/MacOS.

# Funcionalidades

## Principales
-Exploración de mazmorras: Avanza por pisos generados con enemigos, objetos y eventos.
-Combate por turnos: Sistema de batalla con habilidades, efectos de estado y loot.
-Inventario y equipo: Usa, equipa y aprende habilidades de los objetos encontrados.
-Progresión: Sube de nivel, mejora tus estadísticas y enfréntate a jefes.
-Carga dinámica de datos: Todo el contenido se lee desde archivos CSV en la carpeta data.

## Interacción del usuario
-Personalizar nombre y equipamiento de tu personaje.
-El usuario navega por menús y toma decisiones en combate y exploración.
-Se debe administrar bien los recursos limitados que tiene la mazmorra piso a piso.
-Decidir si enfrentarse o no a poderosos enemigos para avanzar al siguiente nivel.

# Interfaces

## Menu Inicial
-Iniciar partida (realiza carga de datos y generacion de la mazmorra).
-Como jugar (reglas del juego y explicacion de sus funcionalidades).
-Salir (cierra la aplicacion).

## Menu de Navegacion (movimiento en la mazmorra)
-Arriba.
-Abajo.
-Izquierda.
-Derecha.
-Revisar Inventario (abre menu de inventario).

## Menu de combate 
-Ataque (basico o habilidad)
-Inventario (solo lista de items para usar)
-Huir

## Menu de inventario (Fuera de combate)
-Ver todo el inventario
-Ver objetos usables en combate
-Ver habilidades aprendidas
-Aprender habilidades
-Equipar algun objeto
-Volver al movimiento en la mazmorra


### Bastian Contreras:
- Hizo la gran mayoria del sistema de combate.
- Diseño inicial de la interfaz de usuario.
- Diseño y redacción del README.
- Creación de archivos csv junto a sus datos.
- Documentación y comentarios en el código.
- **Evaluacion grupal**: 10 (Aporte excelente).

### Sergio Codoceo:
- Diseñó la lectura y parseo de los archivo CSV para que sean correctamente leidos.
- Apoyo en otros sectores (combate y debugging).
- Documentación y comentarios en el código.
- Diseño y redacción del README.
- Creación de archivos csv junto a sus datos.
- **Evaluacion grupal**: 8 (Aporte bueno).

### Benjamin Torres:
- Implementó funciones de lista (list_get , list_find y list_size).
- Diseño la primera version de generacion de loot, ademas de algunos ajustes pequeños en algunas versiones.
- **Evaluacion grupal**: 3 (Aporte pésimo).

### Andres Cardenas:
- Aporte en la gestión de inventario dentro del sistema de combate.
- Implementó y modificó los tdas para que fueran utilizables en la aplicacion.
- Implementó funciones relacionadas fuera del combate , incluida main.
- Documentación y comentarios en el código.
- Implementación de la gestión de inventario para el Jugador.
- **Evaluacion grupal**: 10 (Aporte excelente).