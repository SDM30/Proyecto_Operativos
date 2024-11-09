#include "Publicador.h"

int main( int argc, char *argv[] ) {

    int char_flag = 0;

    if (argc != 7) {
        printf("Uso: ./ejecutable -p <Nombre_FIFO> -f <Archivo_Noticias> -t <Tiempo_Segundos>");
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
        printf("Uso: ./ejecutable -p <Nombre_FIFO> -f <Archivo_Noticias> -t <Tiempo_Segundos>\n");
        return 1;
    }

    if (DEBUG) { printf("Todos los argumentos requeridos están presentes.\n"); }

    return 0;
}

void LeerNoticias(const char* nombre_archivo) {

}

void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo) {
    
}