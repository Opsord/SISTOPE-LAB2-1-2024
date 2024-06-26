#ifndef LECTURA_H
#define LECTURA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "estructuras.h"

// Function prototypes
BMPImage* read_bmp(const char* filename);
void free_bmp(BMPImage* image);
void write_bmp(const char* filename, BMPImage* image);

#endif //LECTURA_H