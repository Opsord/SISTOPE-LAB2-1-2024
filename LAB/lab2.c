#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "lectura/lectura.h"
#include "filtros/filtros.h"
#include "resultados/resultados.h"

int main(int argc, char *argv[]) {

    // ---------------------Argument definition-------------------------

    // Variables for options and their default values
    char *prefix_name = NULL;
    int num_filters = 3;
    float saturation_factor = 1.3;
    float binarization_threshold = 0.5;
    float classification_threshold = 0.5;
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
                printf("Invalid option: %c\n", option);
                return 1;
        }
    }

    // ---------------------Argument verification-------------------------

    if (prefix_name == NULL) {
        printf("Prefix name is mandatory\n");
        return 1;
    }

    // Add a .bmp extension to the file name
    char bmp_file_name[256];
    snprintf(bmp_file_name, sizeof(bmp_file_name), "%s.bmp", prefix_name);

    /*
    printf("Arguments:\n");
    printf("BMP file name: %s\n", bmp_file_name);
    printf("Number of filters: %d\n", num_filters);
    printf("Saturation factor: %f\n", saturation_factor);
    printf("Binarization threshold: %f\n", binarization_threshold);
    printf("Classification threshold: %f\n", classification_threshold);
    printf("Number of workers: %d\n", num_workers);
    printf("Folder name: %s\n", folder_name);
    printf("CSV file name: %s\n", csv_file_name);
     */

    // ---------------------------Pipe creation------------------------------

    // Pipe handling
    int pipe_broker[2];
    if (pipe(pipe_broker) == -1) {
        perror("PIPE ERROR");
        return 1;
    }

    // ---------------------Broker process creation-------------------------

    printf("Main process: Creating child process\n");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        printf("Child process created\n");

        // Close the read end in the child
        close(pipe_broker[0]);

        // dup2(pipe_broker[1], STDOUT_FILENO);
        close(pipe_broker[1]);

        printf("After dup\n");

        // Convert arguments to strings
        char num_filters_str[10];
        char saturation_factor_str[10];
        char binarization_threshold_str[10];
        char num_workers_str[10];

        snprintf(num_filters_str, sizeof(num_filters_str), "%d", num_filters);
        snprintf(saturation_factor_str, sizeof(saturation_factor_str), "%f", saturation_factor);
        snprintf(binarization_threshold_str, sizeof(binarization_threshold_str), "%f", binarization_threshold);
        snprintf(num_workers_str, sizeof(num_workers_str), "%d", num_workers);

        printf("Switching to broker process\n");
        // Prepare arguments for execv
        char *args[] = {
                "./broker",
                bmp_file_name,
                num_filters_str,
                saturation_factor_str,
                binarization_threshold_str,
                num_workers_str,
                NULL
        };

        printf("Starting broker process\n");
        execv("./broker", args);

        perror("execl");
        exit(EXIT_FAILURE);
    } else {  // Parent process

        printf("Main process: Handling pipes\n");

        close(pipe_broker[1]);  // Close the writing end in the parent

        OutputImages output_images;

        // Read the OutputImages structure from the pipe
        if (read(pipe_broker[0], &output_images, sizeof(OutputImages)) == -1) {
            perror("read");
            close(pipe_broker[0]);
            return 1;
        }

        close(pipe_broker[0]);  // Close the read descriptor

        // Process the received images
        if (output_images.saturated != NULL) {
            write_bmp("output_saturated.bmp", output_images.saturated);
            free_bmp(output_images.saturated);
        } else {
            printf("Saturated image not received\n");
        }
        if (output_images.grayscale != NULL) {
            write_bmp("output_grayscale.bmp", output_images.grayscale);
            free_bmp(output_images.grayscale);
        } else {
            printf("Grayscale image not received\n");
        }
        if (output_images.binarized != NULL) {
            write_bmp("output_binarized.bmp", output_images.binarized);
            free_bmp(output_images.binarized);
        } else {
            printf("Binarized image not received\n");
        }

        printf("Main process: Waiting for workers\n");
        // Wait for the child process to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        // ---------------------Final image classification-------------------------

        BMPImage *image = read_bmp(bmp_file_name);
        if (image == NULL) {
            printf("Error reading BMP image for final classification\n");
            return 1;
        }

        bool classification = is_nearly_black(image, classification_threshold);
        printf("Classification: %d\n", classification);

        if (create_csv(csv_file_name, bmp_file_name, classification) == 1) {
            printf("Error creating CSV file\n");
        }

        free_bmp(image);
        printf("END\n");
    }

    return 0;
}