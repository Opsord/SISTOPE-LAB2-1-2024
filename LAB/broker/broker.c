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
    fprintf(stderr, "Broker: Starting\n");

    // ---------------------Argument verification-------------------------

    if (argc != 6) {
        fprintf(stderr, "Broker: Invalid arguments\n");
        fprintf(stderr, "Broker: Number of arguments received: %d\n", argc);
        return 1;
    }

    // ---------------------Argument conversion-------------------------

    const char *prefix_name = argv[1];
    char *endptr;
    errno = 0;

    // Convert the arguments to the correct type
    int num_filters = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting num_filters");
        return 1;
    }

    // Convert the arguments to the correct type
    float saturation_factor = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting saturation_factor");
        return 1;
    }

    // Convert the arguments to the correct type
    float binarization_threshold = strtof(argv[4], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting binarization_threshold");
        return 1;
    }

    // Convert the arguments to the correct type
    int num_workers = strtol(argv[5], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Broker: Error converting num_workers");
        return 1;
    }

    // ---------------------Pipe creation-------------------------

    int fd_in[num_workers][2];
    int fd_out[num_workers][2];

    for (int i = 0; i < num_workers; i++) {
        pipe(fd_in[i]);
        pipe(fd_out[i]);
    }

    // ---------------------Image reading-------------------------

    // Read the image
    BMPImage *image = read_bmp(prefix_name);
    if (image == NULL) {
        fprintf(stderr, "Broker: Error reading the image\n");
        return 1;
    }
    fprintf(stderr, "Broker: Image read successfully\n");
    // printf("Broker: Image width -> %d\n", image->width);
    // printf("Broker: Image height -> %d\n", image->height);

    // ---------------------Image splitting-------------------------

    // Allocate memory for the workers
    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    if (workers == NULL) {
        perror("Broker: Error allocating memory for workers");
        free_bmp(image);
        return 1;
    }

    // Split the image into parts
    BMPImage *parts = split_columns_2(num_workers, image);
    if (parts == NULL) {
        fprintf(stderr, "Broker: Error splitting the image\n");
        free(workers);
        free_bmp(image);
        return 1;
    }

    // ---------------------Worker management-------------------------

    // Array of pids for the workers
    pid_t pids[num_workers];

    // Create the workers
    for (int i = 0; i < num_workers; i++) {
        fprintf(stderr, "Broker: Creating worker %d\n", (i+1));
        pid_t pid = fork(); // Create a child process
        pids[i] = pid;

        if (pid == 0) { // Child process case

            // Close the writing end of the child pipe
            close(fd_in[i][WRITE]);
            // Close the reading end of the parent pipe
            close(fd_out[i][READ]);

            // Redirect the standard input to the reading end of the child pipe
            dup2(fd_in[i][READ], STDIN_FILENO);
            // Close the reading end of the child pipe
            close(fd_in[i][READ]);

            // Redirect the standard output to the writing end of the parent pipe
            dup2(fd_out[i][WRITE], STDOUT_FILENO);
            // Close the writing end of the parent pipe
            close(fd_out[i][WRITE]);

            // Convert the worker id to a string
            char worker_id_str[12];
            snprintf(worker_id_str, sizeof(worker_id_str), "%d", (i+1));

            // Prepare the arguments for the worker
            char *worker_argv[] = {
                    "WORKER",
                    argv[2],
                    argv[3],
                    argv[4],
                    worker_id_str,
                    NULL
            };

            // Debug print before execv
            //fprintf(stderr, "Broker: Executing worker with args: %s %s %s %s %s\n", worker_argv[0], worker_argv[1], worker_argv[2], worker_argv[3], worker_argv[4]);

            // Execute worker using execv
            execv("./WORKER", worker_argv);
            perror("Error executing worker");
            exit(EXIT_FAILURE);

        } else if (pid > 0) { // Parent process case

            // Close the reading end of the child pipe
            close(fd_in[i][READ]);
            // Close the writing end of the parent pipe
            close(fd_out[i][WRITE]);

        } else { // Error case
            perror("Error creating child process");
            for (int j = 0; j <= i; j++) {
                // Close all pipes
                close(fd_in[j][0]);
                close(fd_in[j][1]);
                close(fd_out[j][0]);
                close(fd_out[j][1]);
            }
            free(parts);
            free(workers);
            free_bmp(image);
            return 1;
        }
    }

    for (int i = 0; i < num_workers; i++) { // Parent process case
        int status;
        waitpid(pids[i], &status, 0);
    }

    // ---------------------Reading the workers' output-------------------------

    for (int i = 0; i < num_workers; i++) {
        // Read the worker's output
        ssize_t bytes_read = read(fd_out[i][READ], &workers[i], sizeof(Worker));
        if (bytes_read != sizeof(Worker)) {
            perror("Error reading worker's output");
            for (int j = 0; j < num_workers; j++) {
                close(fd_in[j][0]);
                close(fd_in[j][1]);
                close(fd_out[j][0]);
                close(fd_out[j][1]);
            }
            free(parts);
            free(workers);
            free_bmp(image);
            return 1;
        }
    }

    // ---------------------Output image processing-------------------------

    // Definition of the final image structure
    OutputImages *final_image = malloc(sizeof(OutputImages));

    if (final_image == NULL) {
        perror("Error allocating memory for final image");
        for (int i = 0; i < num_workers; i++) {
            close(fd_in[i][0]);
            close(fd_in[i][1]);
            close(fd_out[i][0]);
            close(fd_out[i][1]);
        }
        free(parts);
        free(workers);
        free_bmp(image);
        return 1;
    }

    // Debugging print
    fprintf(stderr, "Broker: Merging images\n");


    // Initialize the widths and heights of each merged image
    final_image = merge_all(workers, num_workers);

    // ---------------------Output image writing-------------------------

    // Write the final image to send it to the main process
    ssize_t bytes_written = write(STDOUT_FILENO, final_image, sizeof(OutputImages));

    // Free the memory
    free(final_image);
    for (int i = 0; i < num_workers; i++) {
        close(fd_in[i][0]);
        close(fd_in[i][1]);
        close(fd_out[i][0]);
        close(fd_out[i][1]);
    }
    free(parts);
    free(workers);
    free_bmp(image);

    printf("Broker process finished\n");

    // Return success
    return 0;
}