# ifndef PUBLICADOR_H
# define PUBLICADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>

#define DEBUG 1

struct Publicador {
    /* data */
};
typedef struct Publicador PublicadorT; 

struct Noticias {
    char** datos;     // Arreglo de strings
    int tam_actual;   // Numero de strings
};
typedef struct Noticias NoticiasT;

int contarLineas(const char* nombreArchivo);
void LeerNoticias(const char* nombre_archivo, NoticiasT* noticias);
void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo);
void liberarNoticias(struct Noticias* noticias);
int validarFormatoNoticia(const char* linea);

#endif