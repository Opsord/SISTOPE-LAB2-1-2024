#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fworker.h"
#include "../estructuras.h"
#include "../filtros/filtros.h"

// Función para aplicar los filtros a la imagen

Worker pipeline(Worker *worker, int num_filters, int factor_saturacion, int umbral_binarizacion)
{
    // Split the image into parts
    BMPImage *original = worker->original;

    // Apply the filters to each part and write the results to files
    char output_filename[256];
    for (int i = 0; i < num_filters; i++)
    {
        if (i == 0)
        {
            // Saturación
            BMPImage *imagen_saturada = saturate_bmp(original, factor_saturacion);
            if (imagen_saturada == NULL)
            {
                printf("Error al saturar la imagen\n");
            }
            printf("Imagen saturada\n");
            worker->saturada = imagen_saturada;
        }
        else if (i == 1)
        {
            // Escala de grises
            BMPImage *imagen_gris = grayscale_bmp(original);
            if (imagen_gris == NULL)
            {
                printf("Error al convertir a escala de grises\n");
            }
            printf("Imagen en escala de grises\n");
            worker->grises = imagen_gris;
        }
        else if (i == 2)
        {
            // Binarización
            BMPImage *imagen_binaria = binary_bmp(original, umbral_binarizacion);
            if (imagen_binaria == NULL)
            {
                printf("Error al binarizar la imagen\n");
            }
            printf("Imagen binarizada\n");
            worker->binarizada = imagen_binaria;
        }
    }

    return *worker;
}