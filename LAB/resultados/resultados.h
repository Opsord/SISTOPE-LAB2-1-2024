#ifndef RESULTADOS_H
#define RESULTADOS_H

#include <stdbool.h>

// Input: name of a directory
// Output: 0 if the directory was successfully created, 1 if there was an error
// Description: This function attempts to create a new directory with the specified name.
int create_directory(const char *dir);

// Input: name of a CSV file and a string representing the name of the image
// Output: 0 if the CSV file was successfully created or modified, 1 if there was an error
// Description: This function attempts to open the specified CSV file and add a new line with the image name and its classification.
int create_csv(const char *filename, const char *image_name, bool classification);

#endif