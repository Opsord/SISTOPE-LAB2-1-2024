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
            fprintf(stderr,"Image saturated correctly\n");
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

Worker call_worker(BMPImage image, int id_worker, int num_filters, float saturation_factor, float binarization_threshold) {
    Worker worker;
    worker.id = id_worker;
    worker.original = &image;
    fprintf(stderr, "Worker %d: Starting call\n", id_worker);

    for (int i = 0; i < num_filters; i++) {
        switch (i) {
            case 0:
                worker.saturated = saturate_bmp(worker.original, saturation_factor);
                if (worker.saturated == NULL) {
                    fprintf(stderr, "Error saturating the image\n");
                }
                break;
            case 1:
                worker.grayscale = grayscale_bmp(worker.original);
                if (worker.grayscale == NULL) {
                    fprintf(stderr, "Error converting to grayscale\n");
                }
                break;
            case 2:
                worker.binarized = binary_bmp(worker.original, binarization_threshold);
                if (worker.binarized == NULL) {
                    fprintf(stderr, "Error binarizing the image\n");
                }
                break;
            default:
                fprintf(stderr, "Invalid filter index: %d\n", i);
                break;
        }
    }

    return worker;
}