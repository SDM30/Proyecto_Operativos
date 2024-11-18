# ifndef PUBLICADOR_H
# define PUBLICADOR_H

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
void liberarNoticias(NoticiasT* noticias);
int validarFormatoNoticia(const char* linea);
void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo, NoticiasT noticias);

#endif