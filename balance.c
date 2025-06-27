#include "tipoDato.h"
#include <math.h>

int powerIndexItems(Item *I) {
    if (I -> tipoEquip != noEquipable) {
        int poderBruto, vidaDefensa, ataque;
        vidaDefensa = I -> statBonus.defensa; 
        vidaDefensa += I -> statBonus.vida;
        ataque = I -> statBonus.ataque;
        
        poderBruto = vidaDefensa + ataque * 2;
        return poderBruto;
    }
    else return 1;
}

 int powerIndexPlayer(Jugador *P) { 
    return (P -> nivel) * 5.63;
}

int multiHash(int clave, int indiceMAX) {
    if (clave <= 0) return 0;

    double norm = (double)clave / 90.0;          
    double escala = pow(norm,2);             // Escalado cuadrático
    int valor = (int)(escala * (indiceMAX - 1));  // Llevar al rango [0, indiceMAX-1]
    if (valor > 90) valor = 90;
    return valor;
}

int unhash(int hashValor, int indiceMAX) {
    if (hashValor <= 0) return 1;

    double propor = (double)hashValor / (indiceMAX - 1);
    int claveAprox = (int)(sqrt(propor) * 90.0);  
    
    return claveAprox;
}