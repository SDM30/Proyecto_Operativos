#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>    
#include <fcntl.h>      
#include <sys/types.h>  
#include <sys/stat.h>   
#include <semaphore.h>  
#include <errno.h> 
#include "Sistema_Comunicaciones.h"

int main (int argc, char *argv[]) {
    // Validación de argumentos
    if (argc != 7) {
        fprintf(stderr, "Uso: %s -p pipePSC -s pipeSSC -t timeF\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* pipePSC = NULL;
    const char* pipeSSC = NULL;
    int timeF = 0;

    // Procesar argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            pipePSC = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0) {
            pipeSSC = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0) {
            timeF = atoi(argv[++i]);
        } else {
            fprintf(stderr, "Argumento desconocido: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // Validar argumentos obligatorios
    if (!pipePSC || !pipeSSC || timeF <= 0) {
        fprintf(stderr, "Argumentos inválidos. Uso: %s -p pipePSC -s pipeSSC -t timeF\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("PipePSC: %s, PipeSSC: %s, TimeF: %d\n", pipePSC, pipeSSC, timeF);

    // Crear pipes nominales si no existen
    // if (mkfifo(pipePSC, 0666) == -1 && errno != EEXIST) {
    //     perror("Error al crear pipePSC");
    //     return EXIT_FAILURE;
    // }
    // if (mkfifo(pipeSSC, 0666) == -1 && errno != EEXIST) {
    //     perror("Error al crear pipeSSC");
    //     return EXIT_FAILURE;
    // }

    // Leer noticias del pipePSC (ejemplo de funcionalidad)
    printf("Esperando noticias en %s...\n", pipePSC);
    LeerNoticias(pipePSC);
}

void LeerNoticias(const char* nombre_fifo) {
    int fifo_fd = open(nombre_fifo, O_RDONLY);  // Abrir FIFO
    if (fifo_fd < 0) {
        perror("Error al abrir el FIFO");
        return;
    }

    char buffer[256];
    while (1) {
        int n = read(fifo_fd, buffer, sizeof(buffer) - 1);  // Leer del FIFO
        if (n > 0) {
            buffer[n] = '\0';
            printf("Recibido: %s\n", buffer);  // Procesar noticia
        }
    }

    close(fifo_fd);  
}