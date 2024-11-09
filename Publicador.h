# ifndef PUBLICADOR.H
# define PUBLICADOR.H

#include <stdio.h>

#define DEBUG 1

struct Publicador {
    /* data */
};
typedef struct Publicador PublicadorT; 

struct Noticias {
    char** datos;     // Arreglo de strings
    int tam_actual;   // Numero de strings
    int capacidad;    // Capacidad maxima de strings
};
typedef struct Noticias NoticiasT;

void LeerNoticias(const char* nombre_archivo);
void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo);

#endif