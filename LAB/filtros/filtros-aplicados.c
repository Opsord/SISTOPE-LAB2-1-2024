#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filtros.h"
#include "LAB/estructuras.h"
#include "LAB/lectura/lectura.h"

// Entrada: estructura BMPImage, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: estructura BMPImage con la imagen procesada
// Funcionamiento: aplica los filtros seleccionados a la imagen

BMPImage *aplicar_filtros(BMPImage *imagen, int cantidad_filtros, float factor_saturacion, float umbral_binarizacion)
{
    BMPImage *imagen_procesada = imagen;

    for (int i = 0; i < cantidad_filtros; i++)
    {
        switch (i)
        {
        case 0:
            imagen_procesada = saturate_bmp(imagen_procesada, factor_saturacion);
            break;
        case 1:
            imagen_procesada = grayscale_bmp(imagen_procesada);
            break;
        case 2:
            imagen_procesada = binary_bmp(imagen_procesada, umbral_binarizacion);
            break;
        default:
            break;
        }
    }

    return imagen_procesada;
}