#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "lectura/lectura.h"
#include "filtros/filtros.h"
#include "resultados/resultados.h"

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]) {

    // ---------------------Argument definition-------------------------

    // Variables for options and their default values
    char *prefix_name = NULL;
    int num_filters = 3;
    double saturation_factor = 1.3;
    double binarization_threshold = 0.5;
    double classification_threshold = 0.5;
    int num_workers = 1;
    char *folder_name = NULL;
    char *csv_file_name = NULL;

    int option;
    printf("Main process: Starting\n");

    // Read the options
    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:w:")) != -1) {
        switch (option) {
            case 'N':
                prefix_name = optarg;
                break;
            case 'f':
                if (optarg != NULL) {
                    if (optarg>0){
                        num_filters = atoi(optarg);
                    }
                    else{
                        printf("Argumento inválido - ERROR\n");
                        exit(0);
                    }
                }
                break;
            case 'p':
                if(isdigit(atof(optarg)) || atof(optarg)>0){
                    saturation_factor = atof(optarg);
                }
                else{
                    printf("Argumento inválido - ERROR\n");
                    exit(0);
                }
                break;
            case 'u':
                if (isdigit(atof(optarg)) || atof(optarg)>0){
                    binarization_threshold = atof(optarg);
                }
                else{
                    printf("Argumento inválido - ERROR\n");
                }
                break;
            case 'v':
                if (isdigit(atof(optarg)) || atof(optarg)>0){
                    classification_threshold = atof(optarg);
                }
                else{
                    printf("Argumento inválido - ERROR\n");
                    exit(0);
                }
                break;
            case 'C':
                folder_name = optarg;
                break;
            case 'R':
                csv_file_name = optarg;
                break;
            case 'w':
                if (isdigit(atof(optarg)) || atof(optarg)>0){
                    num_workers = atoi(optarg);
                }
                else{
                    printf("Argumento inválido - ERROR\n");
                    exit(0);
                }
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

    // Add a .bmp extension to the file name and add "../" at the beginning
    char bmp_file_name[256];
    snprintf(bmp_file_name, sizeof(bmp_file_name), "../%s.bmp", prefix_name);

    // ---------------------------Pipe creation------------------------------

    int fd_in[2];
    int fd_out[2];
    if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // ---------------------Broker process creation-------------------------

    // Create a child process
    printf("Main process: Creating child process\n");
    pid_t pid = fork();

    // Check if the fork was successful
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Check if the process is the child or the parent
    if (pid == 0) {  // Child process
        printf("Child: Starting (future Broker)\n");

        // Define the arguments for the execv call
        char num_filters_str[10];
        char saturation_factor_str[10];
        char binarization_threshold_str[10];
        char num_workers_str[10];
        // Convert the arguments to strings
        snprintf(num_filters_str, sizeof(num_filters_str), "%d", num_filters);
        snprintf(saturation_factor_str, sizeof(saturation_factor_str), "%f", saturation_factor);
        snprintf(binarization_threshold_str, sizeof(binarization_threshold_str), "%f", binarization_threshold);
        snprintf(num_workers_str, sizeof(num_workers_str), "%d", num_workers);

        // Close the unused pipe ends
        close(fd_in[READ]);
        close(fd_out[WRITE]);

        // Debug print before pipe redirection
        printf("Child: Redirecting pipes\n");

        // Redirect the standard input and output to the pipe
        if (dup2(fd_in[WRITE], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd_in[WRITE]);

        if (dup2(fd_out[READ], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd_out[READ]);

        // Prepare arguments for execv
        char *args[] = {
                "BROKER",
                bmp_file_name,
                num_filters_str,
                saturation_factor_str,
                binarization_threshold_str,
                num_workers_str,
                NULL
        };

        // Debug print before execv
        // fprintf(stderr, "Child: Executing broker with args: %s %s %s %s %s %s\n", args[0], args[1], args[2], args[3], args[4], args[5]);

        // Call the broker process using execv
        execv("./BROKER", args);

        // If execv returns, an error occurred
        perror("Error executing broker");
        exit(EXIT_FAILURE);

    } else {  // Parent process

        close(fd_in[WRITE]);
        close(fd_out[READ]);

        //  ---------------------Receive images from the broker-------------------------

        OutputImages output_images;

        // Read the images from the pipe


        printf("Main process: Waiting for broker to finish\n");
        // Wait for the child process to finish
        int status;
        // Wait for the child process to finish
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        fprintf(stderr, "Main process: Broker finished\n");

        // ---------------------Output image processing-------------------------

        // Process the received images
        // Write the images to files
        if (output_images.saturated != NULL) {
            write_bmp("output_saturated.bmp", output_images.saturated);
            free_bmp(output_images.saturated);
        } else {
            // If the image is NULL, print an error message
            printf("Main process: Saturated image not received\n");
        }
        // Write the images to files
        if (output_images.grayscale != NULL) {
            write_bmp("output_grayscale.bmp", output_images.grayscale);
            free_bmp(output_images.grayscale);
        } else {
            // If the image is NULL, print an error message
            printf("Main process: Grayscale image not received\n");
        }
        // Write the images to files
        if (output_images.binarized != NULL) {
            write_bmp("output_binarized.bmp", output_images.binarized);
            free_bmp(output_images.binarized);
        } else {
            // If the image is NULL, print an error message
            printf("Main process: Binarized image not received\n");
        }

        // ---------------------Final image classification-------------------------

        // Read the final image for classification
        BMPImage *image = read_bmp(bmp_file_name);
        if (image == NULL) {
            printf("Main process: Error reading BMP image for final classification\n");
            return 1;
        }

        // Classify the image
        bool classification = is_nearly_black(image, classification_threshold);
        printf("Main process: Classification: %d\n", classification);

        // ---------------------CSV file creation-------------------------
        if (create_csv(csv_file_name, bmp_file_name, classification) == 1) {
            printf("Main process: Error creating CSV file\n");
        }

        // Free the memory used by the image
        free_bmp(image);
        printf("END\n");
    }

    // Return success
    return 0;
}