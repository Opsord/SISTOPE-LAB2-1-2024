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

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]) {
    printf("Broker: Starting\n");

    // ---------------------Argument verification-------------------------

    if (argc != 6) {
        fprintf(stderr, "Broker: Invalid arguments\n");
        fprintf(stderr, "Broker: Number of arguments received: %d\n", argc);
        return 1;
    }

    const char *prefix_name = argv[1];
    char *endptr;
    errno = 0;

    int num_filters = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting num_filters");
        return 1;
    }

    float saturation_factor = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting saturation_factor");
        return 1;
    }

    float binarization_threshold = strtof(argv[4], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting binarization_threshold");
        return 1;
    }

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
    // printf("Broker: Image width -> %d\n", image->width);
    // printf("Broker: Image height -> %d\n", image->height);

    // ---------------------Image splitting-------------------------

    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    if (workers == NULL) {
        perror("Broker: Error allocating memory for workers");
        free_bmp(image);
        return 1;
    }

    BMPImage *parts = split_columns_2(num_workers, image);
    if (parts == NULL) {
        fprintf(stderr, "Broker: Error splitting the image\n");
        free(workers);
        free_bmp(image);
        return 1;
    }

    // ---------------------Pipe and Worker management-------------------------

    int fd[num_workers][2];
    int fd2[num_workers][2];
    pid_t pids[num_workers];

    for (int i = 0; i < num_workers; i++) {
        pipe(fd[i]);
        pipe(fd2[i]);
    }

    for (int i = 0; i < num_workers; i++) {
        printf("Broker: Creating worker %d\n", (i+1));
        pid_t pid = fork();
        pids[i] = pid;

        if (pid == 0) {

            // Close the writing end of the child pipe
            close(fd[i][WRITE]);
            // Close the reading end of the parent pipe
            close(fd2[i][READ]);
            // Redirect the standard input to the reading end of the child pipe
            dup2(fd[i][READ], STDIN_FILENO);
            // Close the reading end of the child pipe
            close(fd[i][READ]);
            // Redirect the standard output to the writing end of the parent pipe
            dup2(fd2[i][WRITE], STDOUT_FILENO);
            // Close the writing end of the parent pipe
            close(fd2[i][WRITE]);

            char worker_id_str[12];
            snprintf(worker_id_str, sizeof(worker_id_str), "%d", (i+1));

            char *worker_argv[] = {
                    "./worker",
                    argv[2],
                    argv[3],
                    argv[4],
                    worker_id_str,
                    NULL
            };

            // Execute worker
            execv("./worker", worker_argv);
            perror("Error executing worker");
            exit(EXIT_FAILURE);

        } else if (pid > 0) {
            // Close the reading end of the child pipe
            close(fd[i][READ]);
            // Close the writing end of the parent pipe
            close(fd2[i][WRITE]);

            if (write(fd[i][WRITE], &parts[i], sizeof(BMPImage)) == -1) {
                perror("Error writing image part to worker");
                close(fd[i][WRITE]);
                close(fd2[i][READ]);
                free(parts);
                free(workers);
                free_bmp(image);
                return 1;
            }
            close(fd[i][WRITE]);

        } else {
            perror("Error creating child process");
            for (int j = 0; j <= i; j++) {
                // Close all pipes
                close(fd[j][0]);
                close(fd[j][1]);
                close(fd2[j][0]);
                close(fd2[j][1]);
            }
            free(parts);
            free(workers);
            free_bmp(image);
            return 1;
        }
    }

    for (int i = 0; i < num_workers; i++) {
        int status;
        waitpid(pids[i], &status, 0);
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

    printf("Broker process finished\n");
    return 0;
}