#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>    
#include <fcntl.h>      
#include <sys/types.h>  
#include <sys/stat.h>   
#include <semaphore.h>  
#include <errno.h>  
#include "Publicador.h"

sem_t *mutex;

int main( int argc, char *argv[] ) {

    int char_flag = 0;

    if (argc != 7) {
        printf("Uso: ./ejecutable -p <Nombre_FIFO> -f <Archivo_Noticias> -t <Tiempo_Segundos> \n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            char_flag += 1;  // Mark that -p was found
            i++;         // Skip the following argument (Nombre_FIFO)
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            char_flag += 1;  // Mark that -f was found
            i++;         // Skip the following argument (Archivo_Noticias)
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            char_flag += 1;  // Mark that -t was found
            i++;         // Skip the following argument (Tiempo_Segundos)
        } else {
            printf("Argumento desconocido o faltante: %s\n", argv[i]);
            return 1;
        }
    }

    if (char_flag != 3 ) {
        printf("Faltan argumentos obligatorios.\n");
        printf("Uso: ./ejecutable -p <Nombre_FIFO> -f <Archivo_Noticias> -t <Tiempo_Segundos> \n");
        return 1;
    }

    if (DEBUG) { printf("Todos los argumentos requeridos están presentes.\n"); }

    const char* nombre_fifo = argv[2];
    const char* nombre_archivo = argv[4];
    int seg = atoi(argv[6]);

    NoticiasT noticias;
    noticias.tam_actual = contarLineas(argv[4]);

    if (noticias.tam_actual <= 0) {
        printf("No se encontraron noticias o error en el archivo.\n");
        return 1;
    }

    LeerNoticias(nombre_archivo, &noticias);
    for (int i = 0; i < noticias.tam_actual; i++) {
        printf("%s\n", noticias.datos[i]);
    }

    PublicarNoticias(seg, nombre_fifo, noticias);

    return 0;
}

int contarLineas(const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "r");
    if (!archivo) return -1;

    int lineas = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), archivo)) {
        lineas++;
    }
    fclose(archivo);
    return lineas;
}

void LeerNoticias(const char* nombre_archivo, NoticiasT* noticias) {
    FILE *arch_ptr = fopen(nombre_archivo, "r");

    if (!arch_ptr) {
        perror("No fue posible abrir el archivo");
        return;
    }

    noticias->datos = malloc(noticias->tam_actual * sizeof(char*));
    if (!noticias->datos) {
        perror("Error al reservar memoria para noticias");
        fclose(arch_ptr);
        return;
    }

    for (int i = 0; i < noticias->tam_actual; i++) {
        noticias->datos[i] = NULL;  // Inicializar cada puntero a NULL
    }

    char buffer[256];
    int i = 0;

    while (fgets(buffer, sizeof(buffer), arch_ptr) && i < noticias->tam_actual) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Eliminar el salto de línea

        noticias->datos[i] = malloc(strlen(buffer) + 1);
        if (!noticias->datos[i]) {
            perror("Error al reservar memoria para una noticia");
            liberarNoticias(noticias);
            fclose(arch_ptr);
            return;
        }
        
        strcpy(noticias->datos[i], buffer);

        if (!validarFormatoNoticia(buffer)) {
            printf("Archivo con formato inválido en la línea %d: %s\n", i + 1, buffer);
            liberarNoticias(noticias);
            fclose(arch_ptr);
            return;
        }
        i++;
    }

    fclose(arch_ptr);
}

void liberarNoticias(NoticiasT* noticias) {
    if (noticias->datos != NULL) {
        for (int i = 0; i < noticias->tam_actual; i++) {
            if (noticias->datos[i] != NULL) {
                free(noticias->datos[i]);
            }
        }
        free(noticias->datos);
        noticias->datos = NULL;
    }
    noticias->tam_actual = 0;
}

int validarFormatoNoticia(const char* linea) {
    if ((linea[0] == 'A' || linea[0] == 'E' || linea[0] == 'C' || 
         linea[0] == 'P' || linea[0] == 'S') && 
        linea[1] == ':' && linea[2] == ' ') {
        
        if (strlen(linea) > 3) {
            return 1; 
        }
    }
    return 0;
}

void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo, NoticiasT noticias) {
    mutex = sem_open("/semaforo_publicador", O_CREAT, 1);
    
    if (mutex == SEM_FAILED) {
        perror("Error al crear el semáforo");
        return;
    }

    int fifo_fd = open(nombre_fifo, O_WRONLY);  // Abrir FIFO
    if (fifo_fd < 0) {
        perror("Error al abrir el FIFO");
        sem_close(mutex);
        return;
    }

    for(int i = 0; i < noticias.tam_actual; i++) {
        //Seccion Critica
        sem_wait(mutex);
        // Escribir en el FIFO
        printf("Publicando Noticia = %s \n", noticias.datos[i]);
        write(fifo_fd, noticias.datos[i], strlen(noticias.datos[i]) + 1);
        sem_post(mutex);

        sleep(tiempo_n);  // Esperar antes de publicar otra noticia
    }

    close(fifo_fd);
    sem_close(mutex);
}