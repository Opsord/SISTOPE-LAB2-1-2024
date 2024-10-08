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
            fprintf(stderr, "Image saturated correctly\n");
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

#include "fworker.h"
#include "../estructuras.h"
#include "../filtros/filtros.h"

Worker *call_worker(BMPImage *image, int id_worker, int num_filters, float saturation_factor, float binarization_threshold)
{
    // Asignar memoria para un Worker completo
    Worker *worker = (Worker *)malloc(sizeof(Worker));
    if (worker == NULL)
    {
        fprintf(stderr, "Error allocating memory for worker\n");
        return NULL;
    }

    worker->id = id_worker;
    worker->original = image;
    worker->saturated = NULL;
    worker->grayscale = NULL;
    worker->binarized = NULL;

    fprintf(stderr, "Worker %d: Starting call\n", id_worker);

    for (int i = 0; i < num_filters; i++)
    {
        switch (i)
        {
        case 0:
            fprintf(stderr, "Image width: %d\n", worker->original->width);
            fprintf(stderr, "Image height: %d\n", worker->original->height);

            worker->saturated = saturate_bmp(worker->original, saturation_factor);
            fprintf(stderr, "Saturating the image\n");

            if (worker->saturated == NULL)
            {
                fprintf(stderr, "Error saturating the image\n");
            }
            break;
        case 1:
            worker->grayscale = grayscale_bmp(worker->original);
            fprintf(stderr, "Grayscaling the image\n");

            if (worker->grayscale == NULL)
            {
                fprintf(stderr, "Error converting to grayscale\n");
            }
            break;
        case 2:
            worker->binarized = binary_bmp(worker->original, binarization_threshold);
            fprintf(stderr, "Binarizing the image\n");

            if (worker->binarized == NULL)
            {
                fprintf(stderr, "Error binarizing the image\n");
            }
            break;
        default:
            fprintf(stderr, "Invalid filter index: %d\n", i);
            break;
        }
    }
    fprintf(stderr, "Filters applied to the image\n");

    return worker;
}