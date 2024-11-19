#ifndef SUSCRIPCION_H
#define SUSCRIPCION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Tamaño máximo para los tópicos
#define MAX_TOPICO 256
void suscribirse(const char* pipe_SSC);

#endif // SUSCRIPCION_H
