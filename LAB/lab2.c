#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "lectura/lectura.h"
#include "filtros/filtros.h"
#include "resultados/resultados.h"

// Entradas: Argumentos de línea de comandos
// Salidas: Estado de finalización del programa (0 si es exitoso, 1 si hay un error)
// Descripción: Lee una imagen BMP, aplica varios filtros y escribe los resultados en archivos separados
int main(int argc, char *argv[])
{
    // Variables para las opciones y sus valores por defecto
    char *nombre_prefijo = NULL;
    int cantidad_filtros = 3;
    float factor_saturacion = 1.3;
    float umbral_binarizacion = 0.5;
    float umbral_clasificacion = 0.5;
    float cantidad_workers = 1;
    char *nombre_carpeta = NULL;
    char *nombre_archivo_csv = NULL;

    // Variable para getopt
    int option;
    printf("Proceso main iniciado\n");

    // Procesar las opciones
    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:w:")) != -1)
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
            // w es para la cantidad de workers
        case 'w':
            cantidad_workers = atof(optarg);
            break;
        default:
            printf("Opción desconocida: %c\n", option);
            return 1;
        }
    }

    // ------------------- Creación de variables -------------------

    // Creation de variable para el nombre del archivo
    char nombre_archivo[256];
    // Creación del nombre del archivo
    sprintf(nombre_archivo, "%s%s", nombre_prefijo, ".bmp");

    // Creación de la variable para el nombre del archivo de salida
    char output_filename[256];

    // Creación de la carpeta para los resultados
    if (create_directory(nombre_carpeta) == 1)
    {
        printf("Error al crear el directorio\n");
    }

    // ------------------- Lectura de la imagen -------------------

    // Llamada a la función de lectura de la imagen
    BMPImage *imagen = read_bmp(nombre_archivo);
    // Verificación de la lectura de la imagen
    if (imagen == NULL)
    {
        printf("Error al leer la imagen\n");
        return 1;
    }
    // Resultado de la lectura de la imagen
    printf("Imagen leída\n");
    // Impresión de la información de la imagen
    printf("Ancho: %d\n", imagen->width);
    printf("Alto: %d\n", imagen->height);

    // ------------------- Llamado al broker -------------------

    // Llamado al broker con la cantidad de workers, la imagen y los filtros a aplicar

    // ------------------- Aplicación de los filtros -------------------

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
        // Imprimir información del header

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

    // ------------------- Clasificación -------------------

    bool clasificacion = is_nearly_black(imagen, umbral_clasificacion);
    printf("Clasificación: %d\n", clasificacion);

    // ------------------- Creación del archivo CSV -------------------

    if (create_csv(nombre_archivo_csv, nombre_archivo, clasificacion) == 1)
    {
        printf("Error al crear el archivo CSV\n");
    }
    printf("FIN\n");

    return 0;
}