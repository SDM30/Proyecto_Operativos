#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_TOPICO 256

void suscribirse(const char* pipe_SSC);

int main(int argc, char *argv[]) {
    // Verificación de argumentos
    if (argc != 3 || strcmp(argv[1], "-s") != 0) {
        fprintf(stderr, "Uso: %s -s <pipeSSC>\n", argv[0]);
        return 1;
    }

    // Nombre del pipe recibido como argumento
    const char* pipe_SSC = argv[2];
    
    // Iniciar el proceso de suscripción
    suscribirse(pipe_SSC);

    return 0;
}

void suscribirse(const char* pipe_SSC) {
    char topicos[MAX_TOPICO];
    int pipe_fd;

    // Solicitar tópicos al usuario
    printf("Ingrese los tópicos a los que desea suscribirse (separados por comas): ");
    fgets(topicos, MAX_TOPICO, stdin);
    topicos[strcspn(topicos, "\n")] = '\0';  // Remover el salto de línea

    // Abrir el pipe para escribir (suscribirse a los tópicos)
    pipe_fd = open(pipe_SSC, O_WRONLY);
    if (pipe_fd < 0) {
        perror("Error al abrir el pipe");
        exit(1);
    }

    // Enviar los tópicos al sistema de comunicación
    if (write(pipe_fd, topicos, strlen(topicos)) == -1) {
        perror("Error al enviar los tópicos");
        close(pipe_fd);
        exit(1);
    }
    close(pipe_fd);

    // Abrir el pipe para leer noticias
    pipe_fd = open(pipe_SSC, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Error al abrir el pipe para leer");
        exit(1);
    }

    printf("Esperando noticias de interés...\n");

    // Leer y mostrar noticias
    char buffer[1024];
    ssize_t bytes_leidos;
    while ((bytes_leidos = read(pipe_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_leidos] = '\0';
        printf("Noticia: %s\n", buffer);
    }

    if (bytes_leidos == 0) {
        printf("El sistema de comunicación ha finalizado la conexión.\n");
    } else if (bytes_leidos == -1) {
        perror("Error al leer del pipe");
    }

    close(pipe_fd);
}
