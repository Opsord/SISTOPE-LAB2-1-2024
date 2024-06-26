#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "lectura.h"
#include "funciones.h"
#include "resultados.h"

// Entradas: Argumentos de línea de comandos
// Salidas: Estado de finalización del programa (0 si es exitoso, 1 si hay un error)
// Descripción: Lee una imagen BMP, aplica varios filtros y escribe los resultados en archivos separados
int main(int argc, char *argv[])
{
    // Variables para las opciones
    char *nombre_prefijo = NULL;
    int cantidad_filtros = 3;
    float factor_saturacion = 1.3;
    float umbral_binarizacion = 0.5;
    float umbral_clasificacion = 0.5;
    char *nombre_carpeta = NULL;
    char *nombre_archivo_csv = NULL;

    // Variable para getopt
    int option;
    printf("FUNCIONA\n");

    // Procesar las opciones
    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:")) != -1)
    {
        switch (option)
        {
            // N es para el nombre antes del .bmp - es obligatorio
        case 'N':
            nombre_prefijo = optarg;
            break;
            // f es para la cantidad de filtros
        case 'f':
            if (optarg != NULL)
            {
                cantidad_filtros = atoi(optarg);
                break;
            }
            // p es para el factor de saturación
        case 'p':
            factor_saturacion = atof(optarg);
            break;
            // u es para el umbral de binarización
        case 'u':
            umbral_binarizacion = atof(optarg);
            break;
            // v es para el umbral de clasificación
        case 'v':
            umbral_clasificacion = atof(optarg);
            break;
            // C es para el nombre de la carpeta - es obligatorio
        case 'C':
            nombre_carpeta = optarg;
            break;
            // R es para el nombre del archivo CSV - es obligatorio
        case 'R':
            nombre_archivo_csv = optarg;
            break;
        default:
            printf("Opción desconocida: %c\n", option);
            return 1;
        }
    }

    // Primero va la lectura de la imagen
    char nombre_archivo[256];
    sprintf(nombre_archivo, "%s%s", nombre_prefijo, ".bmp");
    // Ahora se llama a la lectura
    BMPImage *imagen = read_bmp(nombre_archivo);
    if (imagen == NULL)
    {
        printf("Error al leer la imagen\n");
        return 1;
    }
    printf("Imagen leída\n");
    // Imprimir la información de la imagen
    printf("Ancho: %d\n", imagen->width);
    printf("Alto: %d\n", imagen->height);

    // Ahora se aplican los filtros
    char output_filename[256];

    // Ahora crear el directorio con los resultados
    if (create_directory(nombre_carpeta) == 1)
    {
        printf("Error al crear el directorio\n");
    }

    // Los filtros se utilizan en orden
    // Si se llama 1, se llama el primero no más
    if (cantidad_filtros == 1)
    {
        // Saturación
        sprintf(output_filename, "%s/%s", nombre_carpeta, "saturada.bmp");
        BMPImage *imagen_saturada = saturate_bmp(imagen, factor_saturacion);
        if (imagen_saturada == NULL)
        {
            printf("Error al saturar la imagen\n");
            return 1;
        }
        // Imprimir la información de la imagen
        printf("Ancho: %d\n", imagen_saturada->width);
        printf("Alto: %d\n", imagen_saturada->height);
        // Imprimir informacion del header

        printf("Imagen saturada\n");
        write_bmp(output_filename, imagen_saturada);
    }
    // Si se llaman 2, se llaman los dos primeros
    if (cantidad_filtros == 2)
    {
        // Saturación
        sprintf(output_filename, "%s/%s", nombre_carpeta, "saturada.bmp");
        BMPImage *imagen_saturada = saturate_bmp(imagen, factor_saturacion);
        if (imagen_saturada == NULL)
        {
            printf("Error al saturar la imagen\n");
            return 1;
        }
        printf("Imagen saturada\n");
        write_bmp(output_filename, imagen_saturada);

        // Escala de grises
        sprintf(output_filename, "%s/%s", nombre_carpeta, "gris.bmp");
        BMPImage *imagen_gris = grayscale_bmp(imagen);
        if (imagen_gris == NULL)
        {
            printf("Error al convertir a escala de grises\n");
            return 1;
        }
        printf("Imagen en escala de grises\n");
        write_bmp(output_filename, imagen_gris);
    }

    // Si se llaman 3, se llaman todos
    if (cantidad_filtros == 3)
    {
        // Saturación
        sprintf(output_filename, "%s/%s", nombre_carpeta, "saturada.bmp");
        BMPImage *imagen_saturada = saturate_bmp(imagen, factor_saturacion);
        if (imagen_saturada == NULL)
        {
            printf("Error al saturar la imagen\n");
            return 1;
        }
        printf("Imagen saturada\n");
        write_bmp(output_filename, imagen_saturada);

        // Escala de grises
        sprintf(output_filename, "%s/%s", nombre_carpeta, "gris.bmp");
        BMPImage *imagen_gris = grayscale_bmp(imagen);
        if (imagen_gris == NULL)
        {
            printf("Error al convertir a escala de grises\n");
            return 1;
        }
        printf("Imagen en escala de grises\n");
        write_bmp(output_filename, imagen_gris);

        // Binarización
        sprintf(output_filename, "%s/%s", nombre_carpeta, "binaria.bmp");
        BMPImage *imagen_binaria = binary_bmp(imagen, umbral_binarizacion);
        if (imagen_binaria == NULL)
        {
            printf("Error al binarizar la imagen\n");
            return 1;
        }
        printf("Imagen binarizada\n");
        write_bmp(output_filename, imagen_binaria);
    }

    // Ahora clasificación - Se aplica siempre
    bool clasificacion = is_nearly_black(imagen, umbral_clasificacion);
    printf("Clasificación: %d\n", clasificacion);

    // Ahora crear el archivo CSV
    if (create_csv(nombre_archivo_csv, nombre_archivo, clasificacion) == 1)
    {
        printf("Error al crear el archivo CSV\n");
    }
    printf("FIN\n");

    return 0;
}