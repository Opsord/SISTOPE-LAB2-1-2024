#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../estructuras.h"
#include "fworker.h"

int main(int argc, char *argv[]) {

    // --------------------------Argument verification--------------------------

    if (argc != 6) { // 5 arguments + null pointer
        fprintf(stderr, "Usage: %s <num_filters> <saturation_factor> <binarization_threshold> <id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *endptr;
    errno = 0;

    // Convert num_filters
    int num_filters = strtol(argv[1], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting num_filters");
        exit(EXIT_FAILURE);
    }

    // Convert saturation_factor
    float saturation_factor = strtof(argv[2], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting saturation_factor");
        exit(EXIT_FAILURE);
    }

    // Convert binarization_threshold
    float binarization_threshold = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting binarization_threshold");
        exit(EXIT_FAILURE);
    }

    // Convert id
    int id = strtol(argv[4], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error converting id");
        exit(EXIT_FAILURE);
    }

    // --------------------------Pipe configuration--------------------------

    // Close unnecessary pipe ends (normally, the parent process would set these ups)
    close(STDOUT_FILENO);  // Close the child's stdout
    close(STDIN_FILENO);   // Close the child's stdin

    // Redirect stdin to the read end of the pipe
    if (dup2(0, STDIN_FILENO) == -1) {
        perror("Error in dup2 for stdin");
        exit(EXIT_FAILURE);
    }

    // Redirect stdout to the writing end of the pipe
    if (dup2(1, STDOUT_FILENO) == -1) {
        perror("Error in dup2 for stdout");
        exit(EXIT_FAILURE);
    }

    // --------------------------Image processing--------------------------

    BMPImage part;

    // Read the image part from stdin
    ssize_t bytes_read = read(STDIN_FILENO, &part, sizeof(BMPImage));
    if (bytes_read != sizeof(BMPImage)) {
        perror("Error reading from stdin");
        exit(EXIT_FAILURE);
    }

    // Process the image with the filters
    Worker worker;
    Worker result = call_worker(part, id, num_filters, saturation_factor, binarization_threshold);

    // Send the resulting Worker structure to stdout
    ssize_t bytes_written = write(STDOUT_FILENO, &result, sizeof(Worker));
    if (bytes_written != sizeof(Worker)) {
        perror("Error writing to stdout");
        exit(EXIT_FAILURE);
    }

    return 0;
}