#ifndef LECTURA_H
#define LECTURA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "LAB/estructuras.h"

// Entrada: nombre de un archivo BMP
// Salida: estructura BMPImage con la información de la imagen
// Funcionamiento: abre el archivo BMP, lee la cabecera y la información de la imagen
BMPImage* read_bmp(const char* filename);

// Entrada: estructura BMPImage con la información de la imagen
// Salida: void
// Funcionamiento: libera la memoria asignada a la estructura BMPImage
void free_bmp(BMPImage* image);

// Entrada: nombre de un archivo BMP y una estructura BMPImage con la información de la imagen
// Salida: void
// Funcionamiento: escribe la estructura BMPImage en un archivo BMP
void write_bmp(const char* filename, BMPImage* image);

#endif //LECTURA_H