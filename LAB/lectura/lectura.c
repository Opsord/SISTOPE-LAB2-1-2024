#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lectura.h"

// Open the file in binary read mode
BMPImage *read_bmp(const char *filepath) {
    // File verification
    FILE *file = fopen(filepath, "rb"); // rb = read binary
    if (!file) {
        fprintf(stderr, "Error: Could not open the file at path %s.\n", filepath);
        return NULL;
    }

    // Read the BMP file header
    BMPHeader header;
    if (fread(&header, sizeof(BMPHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Could not read the file header.\n");
        fclose(file);
        return NULL;
    }

    // Validate the BMP file header
    if (header.type != 0x4D42) {
        fprintf(stderr, "Error: The file is not a valid BMP.\n");
        fclose(file);
        return NULL;
    }

    // Read the BMP image information
    BMPInfoHeader info_header;
    if (fread(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Could not read the image information.\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for the BMPImage structure
    BMPImage *image = (BMPImage *)malloc(sizeof(BMPImage));
    if (!image) {
        fprintf(stderr, "Error: Could not allocate memory for the image.\n");
        fclose(file);
        return NULL;
    }
    image->width = info_header.width;
    image->height = info_header.height;
    image->data = (Pixel *)malloc(sizeof(Pixel) * image->width * image->height);
    if (!image->data) {
        fprintf(stderr, "Error: Could not allocate memory for the image data.\n");
        free(image);
        fclose(file);
        return NULL;
    }

    // SEEK_SET = inicio del archivo,
    // SEEK_CUR = posición actual del archivo
    // SEEK_END = final del archivo
    // fseek(archivo, desplazamiento, origen desde donde se desplaza)

    // Move the file pointer to the beginning of the pixel data
    fseek(file, header.offset, SEEK_SET);

    // Calculate padding
    long padding = (4 - (info_header.width * sizeof(Pixel)) % 4) % 4;

    // Read the pixels of the image
    for (int y = info_header.height - 1; y >= 0; y--) {
        for (int x = 0; x < info_header.width; x++) {
            if (fread(&image->data[y * info_header.width + x], sizeof(Pixel), 1, file) != 1) {
                fprintf(stderr, "Error: Could not read pixel data.\n");
                free(image->data);
                free(image);
                fclose(file);
                return NULL;
            }
        }
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return image;
}

// Free the memory of the BMP image
void free_bmp(BMPImage *image) {
    if (image) {
        if (image->data) {
            free(image->data);
        }
        free(image);
    }
}

// Save the image to a file
void write_bmp(const char *filename, BMPImage *image) {
    // Open the file in binary write mode
    FILE *file = fopen(filename, "wb"); // wb = write binary
    if (!file) {
        fprintf(stderr, "Error: Could not open the file for writing.\n");
        return;
    }

    // Prepare the BMP file header
    BMPHeader header;
    header.type = 0x4D42;
    header.size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image->width * image->height * sizeof(Pixel);
    header.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);
    header.reserved1 = 0;
    header.reserved2 = 0;

    // Prepare the BMP info header
    BMPInfoHeader info_header;
    info_header.size_info = sizeof(BMPInfoHeader);
    info_header.width = image->width;
    info_header.height = image->height;
    info_header.planes = 1;
    info_header.bit_count = 24; // fixed at 24 in this example but can be 1, 4, 8, 16, 24, or 32
    info_header.size_image = image->width * image->height * sizeof(Pixel);

    // Write the headers to the file
    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);

    // Calculate padding
    int padding = (4 - (image->width * sizeof(Pixel)) % 4) % 4;

    // Write each pixel to the file
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            fwrite(&image->data[y * image->width + x], sizeof(Pixel), 1, file);
        }
        for (int p = 0; p < padding; p++) {
            fputc(0, file);
        }
    }

    // Close the file after use
    fclose(file);
}