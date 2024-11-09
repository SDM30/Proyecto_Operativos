#include "Publicador.h"

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

    return 0;
}

int contarLineas(const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "r");
    if (!archivo) return -1;  // Error al abrir el archivo

    int lineas = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), archivo)) {
        lineas++;
    }
    fclose(archivo);
    return lineas;
}

void LeerNoticias(const char* nombre_archivo, NoticiasT* noticias) {
    FILE *arch_ptr;
    //buffer de caracter
    char carac;
    arch_ptr = fopen(nombre_archivo, "r");

    if (NULL == arch_ptr) {
        perror("No fue posible abrir el archivo");
        return;
    }

    noticias->datos = malloc(noticias->tam_actual * sizeof(char*));
    
    if (noticias->datos == NULL) {
        perror("Error al reservar memoria para noticias");
        fclose(arch_ptr);
        return;
    }

    char buffer[256];
    int i = 0;

    while(fgets(buffer, sizeof(buffer), arch_ptr)){
        if (DEBUG) { printf("%s", buffer); }
        buffer[strcspn(buffer, "\n")] = '\0';  // Eliminar el salto de línea
        noticias->datos[i] = malloc(strlen(buffer) + 1);  // Reservar espacio para cada string
        strcpy(noticias->datos[i], buffer);  // Copiar la línea al arreglo
        if (!validarFormatoNoticia(buffer)) {
            liberarNoticias(noticias);
            break;
        }
        i++;
    }

    fclose (arch_ptr);
}

void PublicarNoticias(unsigned int tiempo_n, const char* nombre_fifo) {
    //Abrir fifo
    //Escribir en el fifo hasta que se terminen las noticias
}

void liberarNoticias(struct Noticias* noticias) {
    for (int i = 0; i < noticias->tam_actual; i++) {
        free(noticias->datos[i]); // Liberar cada string
    }
    free(noticias->datos); // Liberar el arreglo de punteros
    noticias->tam_actual = 0;
}

int validarFormatoNoticia(const char* linea) {
    // Verificar si la línea comienza con una categoría válida seguida de ": "
    if ((linea[0] == 'A' || linea[0] == 'E' || linea[0] == 'C' || 
         linea[0] == 'P' || linea[0] == 'S') && 
        linea[1] == ':' && linea[2] == ' ') {
        
        // Verificar que después de ": " hay contenido
        if (strlen(linea) > 3) {
            return 1; 
        } else {
            printf("Archivo con formato invalido");
            return 0;
        }
    }
    return 0; // Formato inválido
}