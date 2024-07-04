#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "resultados.h"

// Entradas: dir - una cadena de caracteres que representa el nombre del directorio a crear
// Salidas: 0 si el directorio se creó con éxito, 1 si hubo un error
// Descripción: Esta función intenta crear un nuevo directorio con el nombre especificado.
int create_directory(const char *dir) {
    if (mkdir(dir, 0777) == -1) {
        fprintf(stderr, "Error: No se pudo crear el directorio %s.\n", dir);
        return 1;
    }
    return 0;
}

// Entradas: filename - una cadena de caracteres que representa el nombre del archivo CSV a crear o modificar
//           image_name - una cadena de caracteres que representa el nombre de la imagen
//           classification - un booleano que representa la clasificación de la imagen
// Salidas: 0 si el archivo CSV se creó o modificó con éxito, 1 si hubo un error
// Descripción: Esta función intenta abrir el archivo CSV especificado y añadir una nueva línea con el nombre de la imagen y su clasificación.
int create_csv(const char *filename, const char *image_name, bool classification) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s.\n", filename);
        return 1;
    }

    fprintf(file, "%s,%d\n", image_name, classification ? 1 : 0);

    fclose(file);
    return 0;
}