#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include "LAB/estructuras.h"

// Entrada: estructura BMPImage con la información de la imagen y un factor de saturación
// Salida: estructura BMPImage con la imagen saturada
// Funcionamiento: aumenta la saturación de la imagen
BMPImage *saturate_bmp(BMPImage *image, float factor);

// Entrada: estructura BMPImage con la información de la imagen
// Salida: estructura BMPImage con la imagen en escala de grises
// Funcionamiento: convierte la imagen a escala de grises
BMPImage *grayscale_bmp(BMPImage *image);

// Entrada: estructura BMPImage con la información de la imagen y un umbral
// Salida: estructura BMPImage con la imagen binarizada
// Funcionamiento: convierte la imagen a binario
BMPImage *binary_bmp(BMPImage *image, float threshold);

// Entrada: estructura BMPImage con la información de la imagen y un umbral
// Salida: booleano que indica si la imagen es casi negra
// Funcionamiento: verifica si la imagen es casi negra
bool is_nearly_black(BMPImage *image, float threshold);

// Entrada: estructura BMPImage, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: estructura BMPImage con la imagen procesada
// Funcionamiento: aplica los filtros seleccionados a la imagen
BMPImage *apply_filters(BMPImage *image, int filter_amount, float saturation_factor, float binarization_threshold);

#endif // FILTROS_H