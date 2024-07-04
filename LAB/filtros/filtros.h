#ifndef FILTROS_H
#define FILTROS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../estructuras.h"

// Entrada: estructura BMPImage con la información de la imagen y un factor de saturación
// Salida: estructura BMPImage con la imagen saturada
// Funcionamiento: aumenta la saturación de la imagen
BMPImage* saturate_bmp(BMPImage* image, float factor);

// Entrada: estructura BMPImage con la información de la imagen
// Salida: estructura BMPImage con la imagen en escala de grises
// Funcionamiento: convierte la imagen a escala de grises
BMPImage* grayscale_bmp(BMPImage* image);

// Entrada: estructura BMPImage con la información de la imagen y un umbral
// Salida: estructura BMPImage con la imagen binarizada
// Funcionamiento: convierte la imagen a binario
BMPImage* binary_bmp(BMPImage* image, float threshold);

// Entrada: estructura BMPImage con la información de la imagen y un umbral
// Salida: booleano que indica si la imagen es casi negra
// Funcionamiento: verifica si la imagen es casi negra
bool is_nearly_black(BMPImage* image, float threshold);

#endif