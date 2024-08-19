#ifndef SISTOPE_LAB2_1_2024_FWORKER_H
#define SISTOPE_LAB2_1_2024_FWORKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../estructuras.h"
#include "../filtros/filtros.h"

Worker workflow(Worker *worker, int num_filters, float saturation_factor, float binarization_threshold);

// Input: BMP Image, worker id, number of filters, saturation factor, binarization threshold
// Output: Worker with the modified image
// Functionality: Creates a worker with the corresponding id, assigns the image to be modified
// and calls the pipeline function that applies the filters to the image
Worker call_worker(BMPImage image, int id_worker, int num_filters, float saturation_factor, float binarization_threshold);

#endif // SISTOPE_LAB2_1_2024_FWORKER_H