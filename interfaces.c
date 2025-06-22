#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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

void limpiarLineas(int lineas) {
    for (int i = 0; i < lineas; i++) {
        printf("\033[F"); // Cursor hacia arriba una línea
        printf("\033[2K"); // Borra la línea completa
    }
}

int main(){
    elGuerrero();
    printf("Bienvenido.");
    getchar();
    limpiarLineas(50);
    printf("Wawawawawa\n");
    printf("Probando");
    return 0;
}