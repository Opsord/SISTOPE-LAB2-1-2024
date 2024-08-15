#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../estructuras.h"
#include "fworker.h"

int main(int argc, char *argv[]) {

    // --------------------------Verificación de argumentos--------------------------
    if (argc != 6) { // 5 argumentos + puntero null
        fprintf(stderr, "Uso: %s <num_filters> <saturation_factor> <binarization_threshold>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_filters = atoi(argv[1]);
    float saturation_factor = atof(argv[2]);
    float binarization_threshold = atof(argv[3]);
    int id = atoi(argv[4]);

    // --------------------------Configuración de Pipes--------------------------

    // Cerrar los extremos no necesarios del pipe (normalmente, el proceso padre configuraría estos)
    close(STDOUT_FILENO);  // Cerrar la salida estándar del hijo (stdout)
    close(STDIN_FILENO);   // Cerrar la entrada estándar del hijo (stdin)

    // Redirigir la entrada estándar (stdin) al extremo de lectura del pipe
    if (dup2(0, STDIN_FILENO) == -1) {
        perror("Error en dup2 para stdin");
        exit(EXIT_FAILURE);
    }

    // Redirigir la salida estándar (stdout) al extremo de escritura del pipe
    if (dup2(1, STDOUT_FILENO) == -1) {
        perror("Error en dup2 para stdout");
        exit(EXIT_FAILURE);
    }

    // --------------------------Procesamiento de la Imagen--------------------------

    BMPImage part;

    // Leer la parte de la imagen desde stdin
    ssize_t bytes_read = read(STDIN_FILENO, &part, sizeof(BMPImage));
    if (bytes_read != sizeof(BMPImage)) {
        perror("Error al leer desde stdin");
        exit(EXIT_FAILURE);
    }

    // Procesar la imagen con los filtros
    Worker worker;
    Worker result = call_worker(part, id, num_filters, saturation_factor, binarization_threshold);

    // Enviar la estructura Worker resultante a stdout
    ssize_t bytes_written = write(STDOUT_FILENO, &result, sizeof(Worker));
    if (bytes_written != sizeof(Worker)) {
        perror("Error al escribir en stdout");
        exit(EXIT_FAILURE);
    }

    return 0;
}
