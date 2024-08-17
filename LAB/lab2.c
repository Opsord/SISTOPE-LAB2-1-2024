#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "lectura/lectura.h"
#include "filtros/filtros.h"
#include "resultados/resultados.h"

int main(int argc, char *argv[]) {

    // ---------------------Definición de argumentos-------------------------

    // Variables para las opciones y sus valores por defecto
    char *prefix_name = NULL;
    int num_filters = 3;
    double saturation_factor = 1.3;
    double binarization_threshold = 0.5;
    double classification_threshold = 0.5;
    int num_workers = 1;
    char *folder_name = NULL;
    char *csv_file_name = NULL;

    int option;
    printf("Main process starting\n");

    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:w:")) != -1) {
        switch (option) {
            case 'N':
                prefix_name = optarg;
                break;
            case 'f':
                if (optarg != NULL) {
                    num_filters = atoi(optarg);
                    break;
                }
            case 'p':
                saturation_factor = atof(optarg);
                break;
            case 'u':
                binarization_threshold = atof(optarg);
                break;
            case 'v':
                classification_threshold = atof(optarg);
                break;
            case 'C':
                folder_name = optarg;
                break;
            case 'R':
                csv_file_name = optarg;
                break;
            case 'w':
                num_workers = atof(optarg);
                break;
            default:
                printf("Opción no válida: %c\n", option);
                return 1;
        }
    }

    // ---------------------Verificación de argumentos-------------------------

    if (prefix_name == NULL) {
        printf("El nombre del prefijo es obligatorio\n");
        return 1;
    }

    // Se agrega la extensión .bmp al nombre del archivo
    char bmp_file_name[256];
    snprintf(bmp_file_name, sizeof(bmp_file_name), "%s.bmp", prefix_name);

    printf("Argumentos:\n");
    printf("Nombre del archivo BMP: %s\n", bmp_file_name);
    printf("Número de filtros: %d\n", num_filters);
    printf("Factor de saturación: %f\n", saturation_factor);
    printf("Umbral de binarización: %f\n", binarization_threshold);
    printf("Umbral de clasificación: %f\n", classification_threshold);
    printf("Número de workers: %d\n", num_workers);
    printf("Nombre de la carpeta de resultados: %s\n", folder_name);
    printf("Nombre del archivo CSV: %s\n", csv_file_name);

    // ---------------------------Creación de pipes------------------------------

    // Manejo de pipe
    int pipe_broker[2];
    if (pipe(pipe_broker) == -1) {
        perror("ERROR EN PIPE");
        return 1;
    }

    // ---------------------Creación de proceso broker-------------------------

    printf("Main process: Creating child process\n");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo
        printf("Child process created\n");

        close(pipe_broker[0]);  // Cerrar el extremo de lectura en el hijo

        dup2(pipe_broker[1], STDOUT_FILENO);
        close(pipe_broker[1]);

        // Convertir los argumentos a cadenas de texto
        char num_filters_str[10];
        char saturation_factor_str[10];
        char binarization_threshold_str[10];
        char num_workers_str[10];

        snprintf(num_filters_str, sizeof(num_filters_str), "%d", num_filters);
        snprintf(saturation_factor_str, sizeof(saturation_factor_str), "%f", saturation_factor);
        snprintf(binarization_threshold_str, sizeof(binarization_threshold_str), "%f", binarization_threshold);
        snprintf(num_workers_str, sizeof(num_workers_str), "%d", num_workers);

        printf("Switching to broker process\n");
        execl("./broker", "broker", prefix_name, num_filters, saturation_factor, binarization_threshold, num_workers, NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    } else {  // Proceso padre

        printf("Main process: Handling pipes\n");

        close(pipe_broker[1]);  // Cerrar el extremo de escritura en el padre

        OutputImages output_images;

        // Leer la estructura OutputImages desde el pipe
        read(pipe_broker[0], &output_images, sizeof(OutputImages));

        close(pipe_broker[0]);  // Cerrar el descriptor de lectura

        // Aquí podrías procesar las imágenes recibidas
        write_bmp("output_saturated.bmp", output_images.saturated);
        write_bmp("output_grayscale.bmp", output_images.grayscale);
        write_bmp("output_binarized.bmp", output_images.binarized);

        // Liberar memoria de las imágenes recibidas
        free_bmp(output_images.saturated);
        free_bmp(output_images.grayscale);
        free_bmp(output_images.binarized);

        printf("Main process: Waiting for workers\n");
        // Esperar a que el proceso hijo termine
        wait(NULL);

        // ---------------------Clasificación final de la imagen-------------------------

        BMPImage *image = read_bmp(bmp_file_name);
        bool classification = is_nearly_black(image, classification_threshold);
        printf("Clasificación: %d\n", classification);

        if (create_csv(csv_file_name, bmp_file_name, classification) == 1) {
            printf("Error al crear el archivo CSV\n");
        }
        printf("FIN\n");
    }

    return 0;
}
