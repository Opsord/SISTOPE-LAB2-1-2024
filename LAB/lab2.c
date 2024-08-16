#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "lectura/lectura.h"
#include "filtros/filtros.h"
#include "resultados/resultados.h"

int main(int argc, char *argv[]) {
    // Variables para las opciones y sus valores por defecto
    char *prefix_name = NULL;
    int num_filters = 3;
    float saturation_factor = 1.3;
    float binarization_threshold = 0.5;
    float classification_threshold = 0.5;
    int num_workers = 1;
    char *folder_name = NULL;
    char *csv_file_name = NULL;

    int option;
    printf("Main process started\n");

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

    if (prefix_name == NULL) {
        printf("El nombre del prefijo es obligatorio\n");
        return 1;
    }

    char bmp_file_name[256];
    snprintf(bmp_file_name, sizeof(bmp_file_name), "%s.bmp", prefix_name);

    // Manejo de pipe
    int pipe_broker[2];
    if (pipe(pipe_broker) == -1) {
        perror("ERROR EN PIPE");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo
        close(pipe_broker[0]);  // Cerrar el extremo de lectura en el hijo

        dup2(pipe_broker[1], STDOUT_FILENO);
        close(pipe_broker[1]);

        execl("./broker", "broker", prefix_name, argv[2], argv[3], argv[4], argv[5], NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    } else {  // Proceso padre
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

        // Esperar a que el proceso hijo termine
        wait(NULL);

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
