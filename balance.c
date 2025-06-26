#include "tipoDato.h"

int powerIndex(Item *I) { // Obtiene el poder bruto de un arma
    if (I -> tipoEquip != noEquipable) {
        int poderBruto, vidaDefensa, ataque;
        vidaDefensa = I -> statBonus.DefensaBonus; 
        vidaDefensa += I -> statBonus.vidaBonus;
        ataque = I -> statBonus.AtaqueBonus;
        
        poderBruto = vidaDefensa + ataque * 2;
        return poderBruto;
    }
    else return 1;
}

int hash(int clave, int valorMAX) { // Obtiene la posicion de un elemento en un mapa, llendo de valores de 1 a valorMAX
    if(clave = 1) return 0;
    int valor = (clave * clave) / valorMAX;
    return (valor >= valorMAX  ?  valorMAX : valor);
} 