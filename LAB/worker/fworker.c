#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fworker.h"
#include "../estructuras.h"
#include "../filtros/filtros.h"

// Función para aplicar los filtros a la imagen

Worker workflow(Worker *worker, int num_filters, int factor_saturacion, int umbral_binarizacion)
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
            worker->saturated = imagen_saturada;
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
            worker->grayscale = imagen_gris;
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
            worker->binarized = imagen_binaria;
        }
    }

    return *worker;
}

Worker call_worker(BMPImage image, int id_worker, int num_filters, int factor_saturacion, int umbral_binarizacion)
{

    // Primero se crea un worker con el id correspondiente
    Worker worker;
    worker.id = id_worker;
    // Asignamos la imagen a modificar
    worker.original = &image;

    // Luego se llama a la función pipeline que aplica los filtros a la imagen
    workflow(&worker, num_filters, factor_saturacion, umbral_binarizacion);

    // Finalmente se retorna el worker con la imagen modificada
    return worker;
}