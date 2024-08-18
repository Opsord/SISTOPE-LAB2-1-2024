#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "fworker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {

    printf("Worker started\n");

    if (argc != 5) {
        printf("Worker error %d\n", argc);
        fprintf(stderr, "Usage: %s <num_filters> <saturation_factor> <binarization_threshold> <id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *endptr;
    errno = 0;

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

    printf("Arguments -->  Filters:%d Sat-Factor:%f Bin-Threshold:%f ID:%d\n", num_filters, saturation_factor, binarization_threshold, id);

    BMPImage part;

    // Read the image part from stdin (file descriptor 0)
    ssize_t bytes_read = read(STDIN_FILENO, &part, sizeof(BMPImage));

    write_bmp("parte_worker.bmp", &part);

    Worker result = call_worker(part, id, num_filters, saturation_factor, binarization_threshold);
    printf("Worker: Ancho de la parte: %d\n", part.width);
    printf("Worker: Alto de la parte: %d\n", part.height);

    printf("Worker ID: %d\n", id);
    printf("Width: %d\n", result.original->width);
    printf("Height: %d\n", result.original->height);

    write_bmp("pruebaworker.bmp", result.original);

    ssize_t bytes_written = write(STDOUT_FILENO, &result, sizeof(Worker));
    if (bytes_written != sizeof(Worker)) {
        perror("Error writing to stdout");
        exit(EXIT_FAILURE);
    }

    return 0;
}