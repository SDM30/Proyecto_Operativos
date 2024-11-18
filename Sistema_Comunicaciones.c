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