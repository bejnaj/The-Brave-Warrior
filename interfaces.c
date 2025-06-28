#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <windows.h>
#include "miscelaneo.h"
#include "tipoDato.h"
#include "tdas/extra.h"

void elGuerrero(){
    printf("                                                                 .::..                 \n");
    printf("                                            .~777!.              .:?Y::.               \n");
    printf("                                            :JJJJJ:              .::J5^::              \n");
    printf("       :JG?:......::^.                      :JJJJ?.               .::7P^::.            \n");
    printf("      :?@@@G7~^~~~~~~^                      :JJJJ7                 .::!P!::.           \n");
    printf("     :5BPB&@&P7~^^^^^^                      :JJJJ7                   .:~P!.:.          \n");
    printf("    .!B&&#PJ5PPP?5YJ?7!~:.                  :JJJJ^                    .:^P7.:.   :^:   \n");
    printf("   :G&#PJ5Y?~:YJP#&@@@@@&G!^.               :JJJ!                     ^^:^5?:..^^?Y5J7^\n");
    printf("   ^?GGJ~Y!?JJ5?&&&&##GGJ!~^.              .:7??~:.                    ^:.:Y5J55YY5J?J!\n");
    printf("  ^~^!7??5!~7GYPGB#&#5~:..             ..:^~~~~~~~~^^:.               ..^?Y5&@&BY:.    \n");
    printf(".^~~~~^~!?J75JG####G?^.              .^~~~~~~~~~~~~~~~^.            ~PJY5P5P#BYY5J^    \n");
    printf("^~~~~~~~~^~7?~~~~~~^.               .^~~YGPJ!~~~?Y5Y~~~^.           ~BGBJ.~YYPP5JYP?   \n");
    printf("~~~~~~~~~~~^^~~~~!^                :~~!PBGGGJ^~?5PPGJ^~~:            !BP^^YYJJYGG5YPY. \n");
    printf("^~~~~~^^^^!77?77?J?:               ~~~5GP5Y5J^~Y55Y5P7~~~:            ..!5YJJJYYPY!!~. \n");
    printf(" .....    ?GP5J???JY~              ~~~Y5P555?^!JY555Y!~~~:            .?5YJJJJY5Y:     \n");
    printf("          :PP5J????J5!             ~~~YJ??JY7^7Y?77??~~~:            ^Y5JJJ?JJYY^      \n");
    printf("           ?P5Y??????J^            77YPYJJY57^J5J???7~~.           .!YJJJ???JJY^       \n");
    printf("           ~P5YJ??777?P!           ~5BG5YYPPYJYY5YJJYJ!           ^?J???????JYY.       \n");
    printf("           .JP5YJ?77YB&#Y.          ^?GGP5Y555YYYJJY5?.         .!JJ???????JJY7        \n");
    printf("             JPYJ?JP#&&&&Y            JBBG5YYJJJJY5P7          :Y#BY???JJJJJYY:        \n");
    printf("             ?B5Y5#&&&&&&&G:          JBB###BBBBBGG?          !P&&&#GYJJJJJYY!         \n");
    printf("           .5#&BB&&&&&&&&&#P~.       :YG5PGBGGGGP557!J7~^^^~75#&&&&&&#PJJJJ5G~         \n");
    printf("           .P&&&&&&&&&&&G?~?&GGP?^^JG#&BGGPPB##BBBBB###BGGGBJ~7?B@&&&&&BPPPB&Y         \n");
    printf("            .?#&&&&&&&&Y~^~BP7!777?????7!!~~GG7~!!!!!!!~~~~!YY~^~5&&&&&&&&&&5.         \n");
    printf("              !B&&&&&&J^~^YB~^~~~~^^^^^^~~~^J?^~~~~~~~~~~~~~~PG~^^75B&&&&&@G.          \n");
    printf("               ^P&&&#?^~~!#7^~~~~~~~~~~~~~~^YJ^~~~~~~~~~~~~~~~BP~~~^~?P#&&B:           \n");
    printf("                 7#B7^~~^?#~^~~~~~~~~~~~~~~^PY^~~~~~~~~~~~~~~^J&?^~~~^^~YB~            \n");
    printf("                  !7^~~~^7#Y~^^^^^^^^^~~~~~~GP~~~~^^^^^^^^^^^^~#5^~~~~~~~~             \n");
    printf("                  :~~~~~~~7PBPYJ?????7!!~~!5#G!~!!7JJJJJJYJYY5GY~~~~~~~~~              \n");
    printf("                  :~~~~~~~^^!77777PG7!!~~~~~!~~~!!7JGG?Y#J77777~~~~~~~~~:              \n");
    printf("                 .~~~~~~~~~~~^^^^^?#5!~~~~^~~~~~~~~7P?^^P!^^^^~~~~~~~~~~.              \n");
    printf("                 ~!!!!!~~~~~~~~~~~~G5!?JJ??PJ~~^~~~?Y^^!G!^~~~~~~~~~~~~~.              \n");
    printf("                 5#####BBBBBBBGGGPYGGJ77!!J#5?J5PPGBJ!?BB!!~~~~~~^^^~~~~.              \n");
    printf("                 5&&&&&&&&&&&P:::::::::::::::::::::^~!?JJ?JGGGGGG55GGGGJ               \n");
    printf("                 5&&&&&&&&&&#^                             Y@&&&&&&&&&#^               \n");
    printf("                 7JJJJY5PPP5^                              :JYYYYYYYJJ!                \n");
    printf("                 :^^~~^~~^^.                                .^^~^^^^:.                 \n");
    printf("                  ..::::..                                    ......                   \n");
}

void interfazComienzo(char* nombre){
    limpiarPantalla();
    printb("Sientes una luz en el rostro...\n");
    Sleep(1000);
    printb("Dice tu nombre y luego pierdes su calidez...");
    Sleep(1500);
    printcv(200,"%s...",nombre);
    Sleep(2000);
}

void muestrorra(){
    limpiarPantalla();
    printf("\n\n\n\n");
}

void comoJugar(){
    muestrorra();
    printcv(30,"¿Cómo jugar The Brave Warrior?\n");
    Sleep(500);
    printb("\nTe preguntarás.");
    Sleep(1000);
    muestrorra();
    printb("Primero lo primero:");Sleep(500);printb("Un poco de contexto sobre el juego para familiarizarte, ");Sleep(200);printb("No?");
    muestrorra();printcv(40,"Eres un guerrero perdido, en un mundo bizarro y desconocido, donde el único recuerdo que tienes de tu vida es tu propio nombre.\n");
    Sleep(200);printcv(40,"Pero de repente escuchas una voz, alguien que te llama en la lejanía.\n");Sleep(200);
    printcv(40,"Y lo único que pasa por tu mente es la forma de poder alcanzarla.\n\n");Sleep(200);printcv(40,"La Valentía de tu ser quema tu interior.");Sleep(250); printcv(40,"Así nace The Brave Warrior.");
    Sleep(2000);printb("\nCachaste?");Sleep(200);printb(" medio lore enigmático pa un juego de cmd si.");
    Sleep(100);printb("Pero es para darle emoción po,");Sleep(200);printb(" al menos el juego es livianito y podi jugarlo cuando quieras.");
    Sleep(1500);printb("Voy a suponer que leiste todo ya");Sleep(1500);
    muestrorra();printb("Ok, cómo jugar:");Sleep(1000);printcv(30,"\n\nTe encontrarás en una mazmorra llena de enemigos y monstruos monstruosos que querrán comerte vivo.\n");
    printcv(30,"Tu objetivo?");Sleep(200);printcv(30," llegar lo más profundo posible, dentro de la mazmorra.");Sleep(300);printcv(30,"Porque, ");
    printcv(45,"como te imaginarás, ");printcv(30,"la trampa del juego es que no tiene final.");Sleep(100);printcv(30," Perdón por el spoiler, pero es que qué otra cosa divertida podrías hacer en un juego para CMD, huh?");Sleep(1000);
    printb("\n\nBueno, continuamos..");Sleep(1000);
    muestrorra();printcv(30,"El sistema de combate consta de 2 partes, tu daño y tu defensa.");Sleep(200);printcv(30,"\nObviamente incluyendo la vida del personaje.\n");
    Sleep(200);printcv(30,"Durante el combate, habrá algo llamado ¨Vida Teória¨ del personaje.");Sleep(100);printcv(30," Que basicamente es la suma de la Vida del personaje y su Defensa.");Sleep(100);printcv(30,"\nY digo personaje, porque este sistema aplica tanto para el jugador como al enemigo.");
    Sleep(100);printcv(30,"Sabi, mucha palabrería, mejor te muestro todo rápido de una y te haces una idea de como funciona.");Sleep(1000);
    muestrorra();printcv(30,"El daño que un personaje recibe será el %% de daño que recibiría la ¨Vida Teórica¨ del mismo.\n\nEjemplo:");Sleep(200);
    printb("Tienes 10 de vida, pero 90 de defensa. Eso es 100 de ¨Vida Teórica¨.\n\nSí recibes 20 puntos de vida, ese valor se restará a tu ¨Vida Teórica¨ y se calculará cuando %% se le quitó.\n\n");Sleep(200);
    printb("En este ejemplo, se le quitó el 20%% a la Vida Teórica.");
    printb("Así que ese 20%% es lo que se le restará a la Vida REAL de tu personaje.");Sleep(200);printb("Dando de resultado que pierdas 2 puntos de vida, ya que conforma el 20%% de tu Vida REAL");
    Sleep(2000); muestrorra(); printb("Entonces como consejo busca más defensa que vida.");Sleep(200);printb(" Ahora:\n\n");
    Sleep(200);printb("Te vas a estar movilizando alrededor de una mazmorra de 5X5 salas, consiguiendo objetos, encontrando enemigos, y finalmente enfrentandote al Jefe del piso. Tras vencer a este pasarás al siguiente piso.");Sleep(500);printb(" Ah y también puedes subir de nivel según una cantidad de enemigos derrotados");Sleep(100);
    printb("Lo que te beneficiará en tus estadísticas de vida, daño y defensa de forma progresiva.");Sleep(200);printb("Por ende, no intentes escaparte todo el rato, o perderás experiencia.");Sleep(2000);
    Sleep(100);muestrorra();printb("Y eso sería basicamente toda la guía del juego, ");Sleep(100);printb("Es bastante instintivo y facil de seguir, siempre y cuando lo ejecutes en cmd para poder ver todas las características del juego sin ningun problema de carácteres");Sleep(2000);
    printb("Eso es todo! Buena suerte Guerrero Bravo!"); Sleep(1000);
}