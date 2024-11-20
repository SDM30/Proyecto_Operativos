#include "Suscriptor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int cola_mensajes_id;
pid_t mi_pid;

void suscribirse(const char* pipe_SSC) {
    char topicos[MAX_TOPICO];
    int pipe_fd;

    // Solicitar tópicos al usuario
    printf("Ingrese los tópicos a los que desea suscribirse (separados por comas): ");
    fgets(topicos, MAX_TOPICO, stdin);
    topicos[strcspn(topicos, "\n")] = '\0';  // Remover el salto de línea

    // Obtener PID del proceso actual
    mi_pid = getpid();

    // Abrir el pipe para escribir (suscribirse a los tópicos)
    pipe_fd = open(pipe_SSC, O_WRONLY);
    if (pipe_fd < 0) {
        perror("Error al abrir el pipe");
        exit(1);
    }

    // Formar mensaje con PID y tópicos
    char mensaje[MAX_TOPICO + 32];
    snprintf(mensaje, sizeof(mensaje), "%d:%s", mi_pid, topicos);

    // Enviar el mensaje al sistema
    if (write(pipe_fd, mensaje, strlen(mensaje)) == -1) {
        perror("Error al enviar los tópicos");
        close(pipe_fd);
        exit(1);
    }
    close(pipe_fd);

    // Configurar el manejador de señales
    signal(SIGUSR1, manejarNoticia);
    signal(SIGTERM, manejarTerminacion);

    printf("Suscriptor registrado con PID: %d. Esperando noticias...\n", mi_pid);

    // Esperar noticias indefinidamente
    while (1) {
        pause();  // Esperar señales
    }
}

void manejarNoticia(int signo) {
    Mensaje mensaje;

    // Leer noticia desde la cola de mensajes, filtrando por el PID del suscriptor
    if (msgrcv(cola_mensajes_id, &mensaje, sizeof(mensaje.contenido), mi_pid, IPC_NOWAIT) == -1) {
        perror("Error al leer de la cola de mensajes");
    } else {
        printf("Nueva noticia recibida: %s\n", mensaje.contenido);
    }
}

void manejarTerminacion(int signo) {
    printf("Recibida señal de terminación (%d). Finalizando suscriptor...\n", signo);
    exit(0); // Finalizar el proceso
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-s") != 0) {
        fprintf(stderr, "Uso: %s -s <pipeSSC>\n", argv[0]);
        return 1;
    }

    // Inicializar la cola de mensajes
    key_t key = ftok("sistema_comunicaciones", 65);
    cola_mensajes_id = msgget(key, 0666 | IPC_CREAT);
    if (cola_mensajes_id == -1) {
        perror("Error al obtener la cola de mensajes");
        exit(1);
    }

    // Iniciar el proceso de suscripción
    suscribirse(argv[2]);

    return 0;
}
