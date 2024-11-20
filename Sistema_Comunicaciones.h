#ifndef SISTEMA_COMUNICACIONES_H
#define SISTEMA_COMUNICACIONES_H

#include <semaphore.h>

// Constantes
#define MAX_SUSCRIPTORES 100
#define MAX_NOTICIAS 100
#define MAX_BUFFER 256

// Estructuras
#define MAX_BUFFER 256
typedef struct {
    pid_t pid;                // PID del suscriptor
    char categorias[MAX_BUFFER]; // Categorías de interés
} Suscriptor;

typedef struct {
    long mtype;                // Tipo de mensaje (PID del suscriptor)
    char contenido[MAX_BUFFER]; // Contenido de la noticia
} Mensaje;

typedef struct {
    char tipo;               // Tipo de noticia ('A', 'E', 'C', 'P', 'S')
    char contenido[80];      // Contenido de la noticia
} Noticia;

// Variables globales
extern Suscriptor suscriptores[MAX_SUSCRIPTORES];
extern int num_suscriptores;
extern Noticia noticias_pendientes[MAX_NOTICIAS];
extern int num_noticias;

// Semáforos
extern sem_t mutex_suscriptores;
extern sem_t mutex_noticias;

// Funciones
void agregarSuscriptor(const char *mensaje);
void agregarNoticia(char tipo, const char *contenido);
void procesarNoticias();
void *gestionarSuscripciones(void *arg);
void *gestionarPublicadores(void *arg);

#endif // SISTEMA_COMUNICACIONES_H