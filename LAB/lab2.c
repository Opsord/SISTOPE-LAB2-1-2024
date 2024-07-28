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
    char *prefix_name = NULL;
    int num_filters = 3;
    float saturation_factor = 1.3;
    float binarization_threshold = 0.5;
    float classification_threshold = 0.5;
    float num_workers = 1;
    char *folder_name = NULL;
    char *csv_file_name = NULL;

    // Variable para getopt
    int option;
    printf("Main process started\n");

    // Procesar las opciones
    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:w:")) != -1)
    {
        switch (option)
        {
            // N es para el nombre antes del .bmp - es obligatorio
        case 'N':
            prefix_name = optarg;
            break;
            // f es para la cantidad de filtros
        case 'f':
            if (optarg != NULL)
            {
                num_filters = atoi(optarg);
                break;
            }
            // p es para el factor de saturación
        case 'p':
            saturation_factor = atof(optarg);
            break;
            // u es para el umbral de binarización
        case 'u':
            binarization_threshold = atof(optarg);
            break;
            // v es para el umbral de clasificación
        case 'v':
            classification_threshold = atof(optarg);
            break;
            // C es para el nombre de la carpeta
        case 'C':
            folder_name = optarg;
            break;
            // R es para el nombre del archivo CSV
        case 'R':
            csv_file_name = optarg;
            break;
            // w es para la cantidad de workers
        case 'w':
            num_workers = atof(optarg);
            break;
        default:
            printf("Opción no válida: %c\n", option);
            return 1;
        }
    }

    // Verificar que el nombre del prefijo no sea NULL
    if (prefix_name == NULL)
    {
        printf("El nombre del prefijo es obligatorio\n");
        return 1;
    }

    // Construir el nombre del archivo BMP
    char bmp_file_name[256];
    snprintf(bmp_file_name, sizeof(bmp_file_name), "%s.bmp", prefix_name);

    // ------------------- Lectura de la imagen -------------------

    // Llamada a la función de lectura de la imagen
    BMPImage *image = read_bmp(bmp_file_name);
    // Verificación de la lectura de la imagen
    if (image == NULL)
    {
        printf("Error al leer la imagen\n");
        return 1;
    }
    // Resultado de la lectura de la imagen
    printf("Imagen leída\n");
    // Imprimir la información de la imagen
    printf("Ancho: %d\n", image->width);
    printf("Alto: %d\n", image->height);

    // ------------------- Llamada al broker -------------------

    // Llamada al broker con la cantidad de workers, la imagen y la cantidad de filtros a aplicar
    pid_t pid = fork();

    // Tarea para el proceso hijo
    if (pid == 0)
    {
        printf("Inicio del proceso hijo\n");

        // Convertir el número de workers a cadena
        char num_workers_str[10];
        sprintf(num_workers_str, "%f", num_workers);
        // Convertir el número de filtros a cadena
        char num_filters_str[10];
        sprintf(num_filters_str, "%d", num_filters);
        // Convertir el factor de saturación a cadena
        char saturation_factor_str[10];
        sprintf(saturation_factor_str, "%f", saturation_factor);
        // Convertir el umbral de binarización a cadena
        char binarization_threshold_str[10];
        sprintf(binarization_threshold_str, "%f", binarization_threshold);

        printf("Cambiar la ejecución del proceso hijo al broker\n");
        // Cambiar la ejecución del proceso hijo al broker
        execl("./broker", "broker", num_workers_str, num_filters_str, saturation_factor_str, binarization_threshold_str, NULL);
        // Termina el proceso hijo y recibir la imagen procesada
        exit(1);
    }

    // ------------------- Clasificación -------------------

    bool classification = is_nearly_black(image, classification_threshold);
    printf("Clasificación: %d\n", classification);

    // ------------------- Creación del archivo CSV -------------------

    if (create_csv(csv_file_name, bmp_file_name, classification) == 1)
    {
        printf("Error al crear el archivo CSV\n");
    }
    printf("FIN\n");

    return 0;
}