#ifndef LECTURA_H
#define LECTURA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../estructuras.h"

// Input: name of a BMP file
// Output: BMPImage structure with the image information
// Functionality: opens the BMP file, reads the header and the image information
BMPImage *read_bmp(const char *filename);

// Input: BMPImage structure with the image information
// Output: void
// Functionality: frees the memory allocated to the BMPImage structure
void free_bmp(BMPImage *image);

// Input: name of a BMP file and a BMPImage structure with the image information
// Output: void
// Functionality: writes the BMPImage structure to a BMP file
void write_bmp(const char *filename, BMPImage *image);

#endif // LECTURA_H