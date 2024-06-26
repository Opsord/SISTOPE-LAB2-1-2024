#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "estructuras.h"

// Function prototypes
BMPImage* saturate_bmp(BMPImage* image, float factor);
BMPImage* grayscale_bmp(BMPImage* image);
BMPImage* binary_bmp(BMPImage* image, float threshold);

bool is_nearly_black(BMPImage* image, float threshold);

#endif