#ifndef RESULTADOS_H
#define RESULTADOS_H

#include <stdbool.h>

// Function to create a directory
int create_directory(const char *dir);

// Function to create a CSV file with classification results
int create_csv(const char *filename, const char *image_name, bool classification);

#endif // RESULTADOS_H