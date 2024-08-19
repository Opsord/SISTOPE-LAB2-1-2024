#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fworker.h"
#include "../estructuras.h"
#include "../filtros/filtros.h"

// Function to apply filters to the image
Worker workflow(Worker *worker, int num_filters, float saturation_factor, float binarization_threshold)
{
    // Split the image into parts
    BMPImage *original = worker->original;

    // Apply the filters to each part and write the results to files
    char output_filename[256];
    for (int i = 0; i < num_filters; i++)
    {
        if (i == 0)
        {
            // Saturation
            BMPImage *saturated_image = saturate_bmp(original, saturation_factor);
            if (saturated_image == NULL)
            {
                printf("Error saturating the image\n");
            }
            printf("Saturated image\n");
            worker->saturated = saturated_image;
        }
        else if (i == 1)
        {
            // Grayscale
            BMPImage *grayscale_image = grayscale_bmp(original);
            if (grayscale_image == NULL)
            {
                printf("Error converting to grayscale\n");
            }
            printf("Grayscale image\n");
            worker->grayscale = grayscale_image;
        }
        else if (i == 2)
        {
            // Binarization
            BMPImage *binary_image = binary_bmp(original, binarization_threshold);
            if (binary_image == NULL)
            {
                printf("Error binarizing the image\n");
            }
            printf("Binary image\n");
            worker->binarized = binary_image;
        }
    }

    return *worker;
}

Worker call_worker(BMPImage image, int id_worker, int num_filters, float saturation_factor, float binarization_threshold)
{
    // First, create a worker with the corresponding id
    Worker worker;
    worker.id = id_worker;
    // Assign the image to be modified
    worker.original = &image;

    // Then call the pipeline function that applies the filters to the image
    workflow(&worker, num_filters, saturation_factor, binarization_threshold);

    // Finally, return the worker with the modified image
    return worker;
}