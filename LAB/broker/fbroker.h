#ifndef SISTOPE_LAB2_1_2024_FBROKER_H
#define SISTOPE_LAB2_1_2024_FBROKER_H

#include "../estructuras.h"

// Input: worker A, worker B
// Output: ID of the worker with the lowest ID
// Functionality: Compares the IDs of the workers and returns the lowest ID
int compare_worker_id(const void *a, const void *b);

// Input: Array of workers, number of workers, function to get the image from a worker
// Output: BMP image with all parts merged
// Functionality: Merges all parts of the images from the workers into a single image
OutputImages *merge_all(Worker *workers, int num_workers);

// Input: Number of parts to split the image into, original image
// Output: Array of split images
// Functionality: Splits the image into parts of equal size (except the last one)
// and assigns the corresponding pixel data to each part
BMPImage **split_columns_2(int num_workers, BMPImage *image);

#endif // SISTOPE_LAB2_1_2024_FBROKER_H