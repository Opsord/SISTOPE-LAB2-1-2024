#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fbroker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {
    printf("Broker started\n");

    // ---------------------Argument verification-------------------------

    if (argc != 6) { // 5 arguments + null pointer
        fprintf(stderr, "Broker: Invalid arguments\n");
        fprintf(stderr, "Broker: Number of arguments received: %d\n", argc);
        return 1;
    }

    // Argument mapping
    const char *prefix_name = argv[1];
    char *endptr;
    errno = 0;

    // Convert num_filters
    int num_filters = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting num_filters");
        return 1;
    }

    // Convert saturation_factor
    float saturation_factor = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting saturation_factor");
        return 1;
    }

    // Convert binarization_threshold
    float binarization_threshold = strtof(argv[4], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting binarization_threshold");
        return 1;
    }

    // Convert num_workers
    int num_workers = strtol(argv[5], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting num_workers");
        return 1;
    }

    // ---------------------Image reading-------------------------

    BMPImage *image = read_bmp(prefix_name);
    if (image == NULL) {
        fprintf(stderr, "Broker: Error reading the image\n");
        return 1;
    }
    printf("Broker: Image read successfully\n");
    printf("Broker: Width: %d\n", image->width);
    printf("Broker: Height: %d\n", image->height);

    // ---------------------Pipe and worker management-------------------------

    // Create pipes for communication with workers
    int pipe_broker[2];
    if (pipe(pipe_broker) == -1) {
        perror("Broker: Error creating pipe");
        free_bmp(image);
        return 1;
    }

    // Close the writing end of the pipe
    close(pipe_broker[1]);

    // Allocate memory for the workers
    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    if (workers == NULL) {
        perror("Broker: Error allocating memory for workers");
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    // Split the image into parts
    BMPImage *parts = split_columns_2(num_workers, image);
    if (parts == NULL) {
        fprintf(stderr, "Broker: Error splitting the image\n");
        free(workers);
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    // Declaring pipes for communication with workers
    int fd[num_workers][2];  // Pipes for child processes
    int fd2[num_workers][2]; // Pipes for parent processes

    for (int i = 0; i < num_workers; i++) {
        if (pipe(fd[i]) == -1 || pipe(fd2[i]) == -1) {
            perror("Broker: Error creating pipes for workers");
            for (int j = 0; j < i; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
                close(fd2[j][0]);
                close(fd2[j][1]);
            }
            free(parts);
            free(workers);
            free_bmp(image);
            close(pipe_broker[0]);
            return 1;
        }
    }

    // ---------------------Worker creation-------------------------
    
    pid_t pid;
    for (int i = 0; i < num_workers; i++) {
        printf("Broker: Creating worker %d\n", i);
        pid = fork();

        if (pid == 0) { // Child process
            printf("Worker %d: Ancho de la parte: %d\n", i, parts[i].width);
            printf("Broker %d: Alto de la parte: %d\n", i, parts[i].height);
            write_bmp("parte_worker_.bmp", &parts[i]);
            close(fd[i][1]);
            close(fd2[i][0]);
            // Send the resulting Worker structure to stdout
            size_t pixel_data_size = parts[i].width * parts[i].height * sizeof(Pixel);
            ssize_t bytes_written = write(STDOUT_FILENO, &parts[i], sizeof(BMPImage));
            if (bytes_written != sizeof(BMPImage)) {
                perror("Error al escribir en stdout\n");
                exit(EXIT_FAILURE);
            }

            close(fd[i][0]);
            close(fd2[i][1]);

            // Prepare arguments for execv
            char worker_id_str[12];
            snprintf(worker_id_str, sizeof(worker_id_str), "%d", i);

            char *worker_argv[] = {
                    "./worker",
                    argv[2], // num_filters
                    argv[3], // saturation_factor
                    argv[4], // binarization_threshold
                    worker_id_str,
                    NULL
            };

            execv("./worker", worker_argv);

            perror("Error executing worker");
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // Parent process
            close(fd[i][0]);
            close(fd2[i][1]);

            // Wait for the workers to finish
            int status;
            waitpid(pid, &status, 0);

            // Send the image part to the worker
            if (write(fd[i][1], &parts[i], sizeof(BMPImage)) == -1) {
                perror("Error writing image part to worker");
                for (int j = 0; j <= i; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                    close(fd2[j][0]);
                    close(fd2[j][1]);
                }
                free(parts);
                free(workers);
                free_bmp(image);
                close(pipe_broker[0]);
                return 1;
            }
            close(fd[i][1]);
        } else {
            perror("Error creating child process");
            for (int j = 0; j <= i; j++) {
                close(fd[j][0]);
                close(fd[j][1]);
                close(fd2[j][0]);
                close(fd2[j][1]);
            }
            free(parts);
            free(workers);
            free_bmp(image);
            close(pipe_broker[0]);
            return 1;
        }
    }

    OutputImages *final_image = malloc(sizeof(OutputImages));
    if (final_image == NULL) {
        perror("Error allocating memory for final image");
        for (int i = 0; i < num_workers; i++) {
            close(fd[i][0]);
            close(fd[i][1]);
            close(fd2[i][0]);
            close(fd2[i][1]);
        }
        free(parts);
        free(workers);
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    final_image = merge_all(workers, num_workers);

    ssize_t bytes_written = write(STDOUT_FILENO, final_image, sizeof(OutputImages));
    if (bytes_written != sizeof(OutputImages)) {
        perror("Error writing to stdout");
        free(final_image);
        for (int i = 0; i < num_workers; i++) {
            close(fd[i][0]);
            close(fd[i][1]);
            close(fd2[i][0]);
            close(fd2[i][1]);
        }
        free(parts);
        free(workers);
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    free(final_image);
    for (int i = 0; i < num_workers; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
        close(fd2[i][0]);
        close(fd2[i][1]);
    }
    free(parts);
    free(workers);
    free_bmp(image);
    close(pipe_broker[0]);

    printf("Broker process finished\n");
    return 0;
}