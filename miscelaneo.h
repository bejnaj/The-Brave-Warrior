#ifndef MISCELANEO_H
#define MISCELANEO_H


// Limpia el STDIN para asegurar el correcto funcionamiento de funciones que dependan de este (por ejemplo, fgets)
void limpiarSTDIN(); 

// Para esperar a que el usuario realice una accion antes de seguir con el programa
void esperarAccion() ;

// Verifica que una opcion numerica este entre 1 y un limite especificado (siendo el maximo el 9)
void verificarOpcion(int *, int); 

// Verifica que una opcion numerica este entre 1 y un limite especificado (siendo el maximo el 9) y borra el numero elegido
void verificarOpcionConBorrado(int *, int); 

// Verifica que una opcion numerica este entre 1 y un limite o contenga palabra clave "SALIR"
int verificarOpcionConSalir(int *, int); 
// RETURN
// 0: No contiene la palabra clave "SALIR"
// 1: Contiene la palabra clave 

// Imprime texto animado con velocidad personalizada (en milisegundos por carácter)
void printcv(int vel, const char *fmt, ...);

// Imprime texto animado con velocidad fija (10 ms por carácter)
void printb(const char *fmt, ...);

// borra x cantidad de lineas de texto
void borrarLineas(int x);

#endif 