#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "funciones.h"

// Entradas: Una imagen BMP y un factor de saturación
// Salidas: Una nueva imagen BMP saturada
// Descripción: Satura una imagen BMP multiplicando los componentes RGB de cada píxel por un factor
BMPImage* saturate_bmp(BMPImage* image, float factor) {
    // Asignar memoria para la nueva imagen
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    if (!new_image) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la nueva imagen.\n");
        return NULL;
    }
    // Copiar el ancho y alto de la imagen original
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel*)malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data) {
        fprintf(stderr, "Error: No se pudo asignar memoria para los datos de la nueva imagen.\n");
        free(new_image);
        return NULL;
    }
    // Saturar los colores de la imagen original
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Pixel pixel = image->data[y * image->width + x];
            // Multiplicar los componentes RGB de cada píxel por el factor y limitar el valor a 255
            // (si el resultado es mayor a 255, se asigna 255)
            // (se convierte el resultado a unsigned char para que el valor esté en el rango [0, 255])
            pixel.r = (pixel.r * factor > 255) ? 255 : (unsigned char)(pixel.r * factor);
            pixel.g = (pixel.g * factor > 255) ? 255 : (unsigned char)(pixel.g * factor);
            pixel.b = (pixel.b * factor > 255) ? 255 : (unsigned char)(pixel.b * factor);
            new_image->data[y * image->width + x] = pixel;
        }
    }

    return new_image;
}

// Entradas: Una imagen BMP
// Salidas: Una nueva imagen BMP en escala de grises
// Descripción: Convierte una imagen BMP a escala de grises
BMPImage* grayscale_bmp(BMPImage* image) {
    // Asignar memoria para la nueva imagen
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    if (!new_image) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la nueva imagen.\n");
        return NULL;
    }
    // Copiar el ancho y alto de la imagen original
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel*)malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data) {
        fprintf(stderr, "Error: No se pudo asignar memoria para los datos de la nueva imagen.\n");
        free(new_image);
        return NULL;
    }
    // Convertir los colores de la imagen original a escala de grises
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Pixel pixel = image->data[y * image->width + x];
            // Se altera el valor de cada componente RGB aplicandole un factor
            // Pixel = (R*0.3) + (G*0.59) + (B*0.11)
            // (se convierte el resultado a unsigned char para que el valor esté en el rango [0, 255])
            unsigned char gray = (unsigned char)(pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11);
            pixel.r = gray;
            pixel.g = gray;
            pixel.b = gray;
            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

// Entradas: Una imagen BMP y un umbral
// Salidas: Una nueva imagen BMP binarizada
// Descripción: Binariza una imagen BMP convirtiéndola a blanco y negro según un umbral
BMPImage* binary_bmp(BMPImage* image, float threshold) {
    // Asignar memoria para la nueva imagen
    BMPImage *new_image = (BMPImage *) malloc(sizeof(BMPImage));
    if (!new_image) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la nueva imagen.\n");
        return NULL;
    }
    // Copiar el ancho y alto de la imagen original
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (Pixel *) malloc(sizeof(Pixel) * image->width * image->height);
    if (!new_image->data) {
        fprintf(stderr, "Error: No se pudo asignar memoria para los datos de la nueva imagen.\n");
        free(new_image);
        return NULL;
    }
    // Binarizar los pixeles de la imagen
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Pixel pixel = image->data[y * image->width + x];
            // Calcular la intensidad del píxel
            float intensity = pixel.r + pixel.g + pixel.b;
            float umbral = threshold * 255 * 3;

            // Si la intensidad del píxel es mayor que el umbral, se convierte en blanco (255), de lo contrario, se convierte en negro (0)
            if (intensity > umbral) {
                pixel.r = 255;
                pixel.g = 255;
                pixel.b = 255;
            } else {
                pixel.r = 0;
                pixel.g = 0;
                pixel.b = 0;
            }

            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
    }

    // Entradas: Una imagen BMP y un umbral
    // Salidas: Un booleano que indica si la imagen es casi negra o no
    // Descripción: Clasifica una imagen BMP como casi negra o no, contando los píxeles negros y comparándolos con un umbral
    bool is_nearly_black(BMPImage* image, float threshold) {
        int black_pixels = 0; // Contador de pixeles negros
        int total_pixels = image->width * image->height; // Contador de pixeles en total

        // Recorrer todos los píxeles de la imagen
        for (int y = 0; y < image->height; y++) { // Para cada posición de los pixeles
            for (int x = 0; x < image->width; x++) {
                Pixel pixel = image->data[y * image->width + x]; // Tomaremos dicho pixel
                // Verificar si el píxel es negro
                if (pixel.r == 0  && pixel.g == 0 && pixel.b == 0) {
                    black_pixels++; // En caso de ser un pixel negro, lo contamos
                }
            }
        }

        // Calcular el porcentaje de píxeles negros
        float black_percentage = (float)black_pixels / total_pixels; // Queremos el porcentaje de pixeles negros con respecto al total
        printf("el porcentaje es %f\n", black_percentage);

        // Comparar el porcentaje con el umbral
        return black_percentage >= threshold;
    }