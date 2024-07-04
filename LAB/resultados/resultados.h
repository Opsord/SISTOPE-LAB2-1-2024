#ifndef RESULTADOS_H
#define RESULTADOS_H

#include <stdbool.h>

// Entrada: nombre de un directorio
// Salida: 0 si el directorio se creó con éxito, 1 si hubo un error
// Descripción:
// Esta función intenta crear un nuevo directorio con el nombre especificado.
int create_directory(const char *dir);

// Entrada: nombre de un archivo CSV y una cadena de caracteres que representa el nombre de la imagen
// Salida: 0 si el archivo CSV se creó o modificó con éxito, 1 si hubo un error
// Descripción:
// Esta función intenta abrir el archivo CSV especificado y
// añadir una nueva línea con el nombre de la imagen y su clasificación.
int create_csv(const char *filename, const char *image_name, bool classification);

#endif