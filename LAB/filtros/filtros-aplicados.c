#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filtros.h"
#include "estructuras.h"

// Input: BMPImage structure, number of filters, saturation factor, binarization threshold
// Output: Result structure with the processed image
// Functionality: applies the selected filters to the image
OutputImages *apply_filters(BMPImage *image, int filter_amount, float saturation_factor, float binarization_threshold)
{
    OutputImages *output = (OutputImages *)malloc(sizeof(OutputImages));

    for (int i = 0; i < filter_amount; i++)
    {
        switch (i)
        {
            case 0:
                output->saturated = saturate_bmp(image, saturation_factor);
                break;
            case 1:
                output->grayscale = grayscale_bmp(image);
                break;
            case 2:
                output->binarized = binary_bmp(image, binarization_threshold);
                break;
            default:
                break;
        }
    }
    return output;
}