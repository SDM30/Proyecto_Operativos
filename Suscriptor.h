#ifndef SUSCRIPCION_H
#define SUSCRIPCION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Tamaño máximo para los tópicos
#define MAX_TOPICO 128
#define MAX_NOTICIA 256
void suscribirse(const char* pipe_SSC);
void manejarTerminacion(int signo);
void manejarNoticia(int signo);
void manejarNoticia(int signo);

typedef struct {
    long mtype;                // Tipo de mensaje (PID del suscriptor)
    char contenido[MAX_NOTICIA]; // Contenido de la noticia
} Mensaje;


#endif // SUSCRIPCION_H
