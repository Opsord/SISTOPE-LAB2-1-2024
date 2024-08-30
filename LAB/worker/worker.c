#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "fworker.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {

    // ------------------------Argument validation------------------------

    fprintf(stderr, "Worker %s: Starting\n", argv[4]);

    // Check the number of arguments
    if (argc != 5) {
        printf("Worker error %d\n", argc);
        fprintf(stderr, "Usage: %s <num_filters> <saturation_factor> <binarization_threshold> <id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *endptr;
    errno = 0;

    // Convert the arguments to the correct type
    int num_filters = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting num_filters");
        exit(EXIT_FAILURE);
    }

    float saturation_factor = strtof(argv[2], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting saturation_factor");
        exit(EXIT_FAILURE);
    }

    float binarization_threshold = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting binarization_threshold");
        exit(EXIT_FAILURE);
    }

    int id = strtol(argv[4], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting id");
        exit(EXIT_FAILURE);
    }

    // Debug print
    //printf("Worker %d -->  Filters:%d Sat-Factor:%f Bin-Threshold:%f\n", id, num_filters, saturation_factor, binarization_threshold);

    // -------------------------Image processing-------------------------

    // Read the image part from stdin
    BMPImage part;

    fprintf(stderr, "Worker %d: Before reading STD\n", id);

    // Read the image part from stdin (file descriptor 0)
    ssize_t bytes_read = read(STDIN_FILENO, &part, sizeof(BMPImage));
    if (bytes_read != sizeof(BMPImage)) {
        perror("Error reading image part from stdin");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Worker %d: After reading STD\n", id);

    // Apply the filters to the image part
    Worker result = call_worker(part, id, num_filters, saturation_factor, binarization_threshold);
    fprintf(stderr,"Worker: Part width: %d\n", part.width);
    fprintf(stderr,"Worker: Part height: %d\n", part.height);

    fprintf(stderr,"Worker ID: %d\n", id);
    fprintf(stderr,"Width: %d\n", result.original->width);
    fprintf(stderr,"Height: %d\n", result.original->height);

    // Write the modified image to a file
    write_bmp("testworker.bmp", result.original);

    // Write the modified image to stdout
    ssize_t bytes_written = write(STDOUT_FILENO, &result, sizeof(Worker));
    if (bytes_written != sizeof(Worker)) {
        perror("Error writing to stdout");
        exit(EXIT_FAILURE);
    }

    // Free the memory
    return 0;
}