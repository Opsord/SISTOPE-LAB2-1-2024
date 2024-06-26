#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    // Definición de la estructura BMP Header (Informacion del archivo)
    uint16_t type;              // Tipo de dato, indica si el archivo es BMP a través de las siglas BM
    uint32_t size;              // Tamaño del archivo
    uint16_t reserved1;         // Reservado
    uint16_t reserved2;         // Reservado
    uint32_t offset;            // Offset, indica la posición en bytes donde comienza la información de la imagen
} BMPHeader;

// Definición de la estructura BMP Info Header (Informacion de la imagen contenida en el archivo)
typedef struct {
    uint32_t size_info;         // Tamaño de la información de la imagen (info header)
    int32_t width;              // Ancho de la imagen
    int32_t height;             // Alto de la imagen
    uint16_t planes;            // Planos, indica la cantidad de planos de color, debe valer 1
    uint16_t bit_count;         // Bits por píxel
    uint32_t compression;       // Compresión, puede ser 0 (sin compresión), 1 (RLE 8-bit/píxel), 2 (RLE 4-bit/píxel), 3 (bitfields), 4 (JPEG), 5 (PNG)
    uint32_t size_image;        // Tamaño de la imagen
    int32_t x_pixels_per_meter; // Píxeles por metro en el eje X
    int32_t y_pixels_per_meter; // Píxeles por metro en el eje Y
    uint32_t colors_used;       // Colores usados
    uint32_t colors_important;  // Colores importantes
} BMPInfoHeader;
#pragma pack(pop)

// Definición de la estructura Pixel (Pixel RGB/BIT)
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

// Definición de la estructura BMPImage (Imagen BMP)
typedef struct {
    uint32_t width;
    uint32_t height;
    Pixel *data; // Puntero a los píxeles de la imagen
} BMPImage;

#endif // ESTRUCTURAS_H