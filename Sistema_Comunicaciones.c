#include "Sistema_Comunicaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

// Variables globales
Suscriptor suscriptores[MAX_SUSCRIPTORES];
int num_suscriptores = 0;
int cola_mensajes_id;

// Semáforos
sem_t mutex_suscriptores;

// Pipes nominales
const char *pipePSC = NULL;
const char *pipeSSC = NULL;
int sistema_activo = 1; // 1: Activo, 0: Finalizar

void agregarSuscriptor(const char *mensaje) {
    char categorias[MAX_BUFFER];
    pid_t pid;

    // Parsear PID y categorías desde el mensaje recibido
    sscanf(mensaje, "%d:%s", &pid, categorias);

    sem_wait(&mutex_suscriptores);

    if (num_suscriptores < MAX_SUSCRIPTORES) {
        suscriptores[num_suscriptores].pid = pid;
        strcpy(suscriptores[num_suscriptores].categorias, categorias);
        num_suscriptores++;
        printf("Nuevo suscriptor registrado: PID=%d, Categorías=%s\n", pid, categorias);

        // Enviar noticias pendientes relevantes
        int pendientes = inicio_pendientes;
        while (pendientes != fin_pendientes) {
            if (strchr(categorias, cola_pendientes[pendientes].tipo)) {
                printf("Enviando noticia pendiente a PID=%d: %s\n", pid, cola_pendientes[pendientes].contenido);

                Mensaje mensaje;
                mensaje.mtype = pid;
                strncpy(mensaje.contenido, cola_pendientes[pendientes].contenido, MAX_BUFFER);

                if (msgsnd(cola_mensajes_id, &mensaje, sizeof(mensaje.contenido), 0) == -1) {
                    perror("Error al enviar noticia pendiente a la cola de mensajes");
                } else {
                    kill(pid, SIGUSR1);
                }
            }
            pendientes = (pendientes + 1) % MAX_NOTICIAS;
        }
    } else {
        printf("Número máximo de suscriptores alcanzado.\n");
    }

    sem_post(&mutex_suscriptores);
}


void enviarNoticia(char tipo, const char *contenido) {
    Mensaje mensaje;
    int suscriptores_encontrados = 0;

    sem_wait(&mutex_suscriptores);

    for (int i = 0; i < num_suscriptores; i++) {
        if (strchr(suscriptores[i].categorias, tipo)) {
            suscriptores_encontrados = 1;
            mensaje.mtype = suscriptores[i].pid; // Asignar el PID como tipo de mensaje
            strncpy(mensaje.contenido, contenido, MAX_BUFFER);

            // Escribir noticia en la cola de mensajes
            if (msgsnd(cola_mensajes_id, &mensaje, sizeof(mensaje.contenido), 0) == -1) {
                perror("Error al enviar noticia a la cola de mensajes");
            } else {
                printf("Noticia enviada a PID=%d: %s\n", suscriptores[i].pid, contenido);

                // Enviar señal al suscriptor
                kill(suscriptores[i].pid, SIGUSR1);
            }
        }
    }

    // Si no hay suscriptores, guardar en cola de pendientes
    if (!suscriptores_encontrados) {
        cola_pendientes[fin_pendientes].tipo = tipo;
        strncpy(cola_pendientes[fin_pendientes].contenido, contenido, MAX_BUFFER);
        fin_pendientes = (fin_pendientes + 1) % MAX_NOTICIAS;
        printf("Noticia almacenada en cola pendiente: %c: %s\n", tipo, contenido);
    }

    sem_post(&mutex_suscriptores);
}

void *gestionarPublicadores(void *arg) {
    int pipe_fd = open(pipePSC, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Error al abrir pipePSC");
        pthread_exit(NULL);
    }

    printf("Pipe para publicadores abierto correctamente.\n");

    char buffer[MAX_BUFFER];
    while (1) {
        ssize_t n = read(pipe_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Noticia recibida: %s\n", buffer);

            if (buffer[1] == ':') { // Validar formato 'A: contenido'
                enviarNoticia(buffer[0], buffer + 2);
            } else {
                printf("Formato de noticia inválido: %s\n", buffer);
            }
        } else if (n == 0) {
            printf("No hay más noticias.\n");
            break;
        } else {
            perror("Error al leer del pipePSC");
        }
    }

    close(pipe_fd);
    pthread_exit(NULL);
}

void *gestionarSuscripciones(void *arg) {
    int pipe_fd = open(pipeSSC, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Error al abrir pipeSSC");
        pthread_exit(NULL);
    }

    printf("Pipe para suscripciones abierto correctamente.\n");

    char buffer[MAX_BUFFER];
    while (sistema_activo) {
        ssize_t n = read(pipe_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Suscripción recibida: %s\n", buffer);
            agregarSuscriptor(buffer);
        } else if (n == -1) {
            perror("Error al leer del pipeSSC");
            break;
        }
        // Si n == 0 (EOF), esperar un momento y volver a intentar
        usleep(100000); // Esperar 100 ms
    }

    close(pipe_fd);
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Uso: %s -p pipePSC -s pipeSSC -t timeF\n", argv[0]);
        return EXIT_FAILURE;
    }

    sem_init(&mutex_suscriptores, 0, 1);

    key_t key = ftok("sistema_comunicaciones", 65);
    cola_mensajes_id = msgget(key, 0666 | IPC_CREAT);
    if (cola_mensajes_id == -1) {
        perror("Error al crear la cola de mensajes");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            pipePSC = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0) {
            pipeSSC = argv[++i];
        }
    }

    pthread_t hilo_suscripciones, hilo_publicadores;
    pthread_create(&hilo_suscripciones, NULL, gestionarSuscripciones, NULL);
    pthread_create(&hilo_publicadores, NULL, gestionarPublicadores, NULL);

    printf("Presiona ENTER para finalizar el sistema...\n");
    getchar(); // Esperar a que el usuario presione ENTER

    // Indicar que el sistema debe finalizar
    sistema_activo = 0;

    // Esperar a que los hilos terminen
    pthread_join(hilo_publicadores, NULL);
    pthread_join(hilo_suscripciones, NULL);

    // Finalizar suscriptores antes de eliminar recursos
    finalizarSuscriptores();

    // Eliminar la cola de mensajes
    msgctl(cola_mensajes_id, IPC_RMID, NULL);

    sem_destroy(&mutex_suscriptores);

    printf("Sistema de Comunicaciones finalizado.\n");
    return EXIT_SUCCESS;
}


void finalizarSuscriptores() {
    sem_wait(&mutex_suscriptores);

    for (int i = 0; i < num_suscriptores; i++) {
        printf("Enviando señal de terminación a PID=%d\n", suscriptores[i].pid);
        if (kill(suscriptores[i].pid, SIGTERM) == -1) {
            perror("Error al enviar señal de terminación");
        }
    }

    sem_post(&mutex_suscriptores);
}
