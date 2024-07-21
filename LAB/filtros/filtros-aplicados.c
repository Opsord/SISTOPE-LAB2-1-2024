#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filtros.h"

// Entrada: estructura BMPImage, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: estructura BMPImage con la imagen procesada
// Funcionamiento: aplica los filtros seleccionados a la imagen
BMPImage *apply_filters(BMPImage *image, int filter_amount, float saturation_factor, float binarization_threshold)
{
    BMPImage *processed_image = image;

    for (int i = 0; i < filter_amount; i++)
    {
        switch (i)
        {
        case 0:
            processed_image = saturate_bmp(processed_image, saturation_factor);
            break;
        case 1:
            processed_image = grayscale_bmp(processed_image);
            break;
        case 2:
            processed_image = binary_bmp(processed_image, binarization_threshold);
            break;
        default:
            break;
        }
    }

    return processed_image;
}