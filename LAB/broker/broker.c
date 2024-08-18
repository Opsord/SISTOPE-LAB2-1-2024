#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "fbroker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {
    printf("Broker started\n");

    // ---------------------Argument verification-------------------------

    if (argc != 6) { // 5 arguments + null pointer
        fprintf(stderr, "Error: Invalid arguments\n");
        fprintf(stderr, "Number of arguments received: %d\n", argc);
        return 1;
    }

    // Recordatorio de los argumentos
    // prefix_name               argv[1]
    // num_filters               argv[2]
    // saturation_factor         argv[3]
    // binarization_threshold    argv[4]
    // num_workers               argv[5]

    // Argument mapping
    const char *prefix_name = argv[1];
    char *endptr;
    errno = 0;

    // Convert num_filters
    int num_filters = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting num_filters");
        return 1;
    }

    // Convert saturation_factor
    float saturation_factor = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting saturation_factor");
        return 1;
    }

    // Convert binarization_threshold
    float binarization_threshold = strtof(argv[4], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting binarization_threshold");
        return 1;
    }

    // Convert num_workers
    int num_workers = strtol(argv[5], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting num_workers");
        return 1;
    }

    // ---------------------Image reading-------------------------

    BMPImage *image = read_bmp(prefix_name);
    if (image == NULL) {
        fprintf(stderr, "Error reading the image\n");
        return 1;
    }
    printf("Image read successfully\n");
    printf("Width: %d\n", image->width);
    printf("Height: %d\n", image->height);

    // ---------------------Pipe and worker management-------------------------

    int pipe_broker[2];
    if (pipe(pipe_broker) == -1) {
        perror("Error creating pipe");
        free_bmp(image);
        return 1;
    }

    close(pipe_broker[1]); // Close the writing end of the pipe

    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    if (workers == NULL) {
        perror("Error allocating memory for workers");
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    BMPImage *parts = split_columns_2(num_workers, image);
    if (parts == NULL) {
        fprintf(stderr, "Error splitting the image\n");
        free(workers);
        free_bmp(image);
        close(pipe_broker[0]);
        return 1;
    }

    int fd[num_workers][2];  // Pipes for child processes
    int fd2[num_workers][2]; // Pipes for parent processes

    for (int i = 0; i < num_workers; i++) {
        if (pipe(fd[i]) == -1 || pipe(fd2[i]) == -1) {
            perror("Error creating pipes for workers");
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

    pid_t pid;
    for (int i = 0; i < num_workers; i++) {
        printf("Creating worker %d\n", i);
        pid = fork();

        if (pid == 0) { // Child process
            close(fd[i][1]);
            close(fd2[i][0]);

            dup2(fd[i][0], STDIN_FILENO);
            dup2(fd2[i][1], STDOUT_FILENO);

            close(fd[i][0]);
            close(fd2[i][1]);

            execl("./worker", "./worker", argv[2], argv[3], argv[4], i, NULL);

            perror("Error executing worker");
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // Parent process
            close(fd[i][0]);
            close(fd2[i][1]);
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

    ssize_t bytes_written = write(STDOUT_FILENO, &final_image, sizeof(OutputImages));
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