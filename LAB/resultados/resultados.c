#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "resultados.h"

// Inputs: dir - a string representing the name of the directory to create
// Outputs: 0 if the directory was successfully created, 1 if there was an error
// Description: This function attempts to create a new directory with the specified name.
int create_directory(const char *dir) {
    if (mkdir(dir, 0777) == -1) {
        fprintf(stderr, "Error: Could not create directory %s.\n", dir);
        return 1;
    }
    return 0;
}

// Inputs: filename - a string representing the name of the CSV file to create or modify
//         image_name - a string representing the name of the image
//         classification - a boolean representing the classification of the image
// Outputs: 0 if the CSV file was successfully created or modified, 1 if there was an error
// Description: This function attempts to open the specified CSV file and add a new line with the image name and its classification.
int create_csv(const char *filename, const char *image_name, bool classification) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s.\n", filename);
        return 1;
    }

    fprintf(file, "%s,%d\n", image_name, classification ? 1 : 0);

    fclose(file);
    return 0;
}