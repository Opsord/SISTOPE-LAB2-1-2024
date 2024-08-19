#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include "../estructuras.h"

// Input: BMPImage structure with the image information and a saturation factor
// Output: BMPImage structure with the saturated image
// Functionality: increases the saturation of the image
BMPImage *saturate_bmp(BMPImage *image, float factor);

// Input: BMPImage structure with the image information
// Output: BMPImage structure with the grayscale image
// Functionality: converts the image to grayscale
BMPImage *grayscale_bmp(BMPImage *image);

// Input: BMPImage structure with the image information and a threshold
// Output: BMPImage structure with the binarized image
// Functionality: converts the image to binary
BMPImage *binary_bmp(BMPImage *image, float threshold);

// Input: BMPImage structure with the image information and a threshold
// Output: boolean indicating if the image is nearly black
// Functionality: checks if the image is nearly black
bool is_nearly_black(BMPImage *image, float threshold);

// Input: BMPImage structure, number of filters, saturation factor, binarization threshold
// Output: OutputImages structure with the processed images
// Functionality: applies the selected filters to the image
OutputImages *apply_filters(BMPImage *image, int filter_amount, float saturation_factor, float binarization_threshold);

#endif // FILTROS_H