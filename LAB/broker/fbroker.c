//
// Created by ivang on 27/06/2024.
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estructuras.h"
#include "fworker.h"

// Function to split image and apply filter
BMPImage* split(int num, BMPImage *image) {
    uint32_t part_height = image->height / num;
    BMPImage* parts = malloc(num * sizeof(BMPImage));
    for (int i = 0; i < num; i++) {
        parts[i].width = image->width;
        parts[i].height = part_height;
        parts[i].data = image->data + i * part_height * image->width;
    }
    return parts;
}

// Function to compare worker ids for qsort
int compare_worker_id(const void *a, const void *b) {
    Worker *workerA = (Worker *)a;
    Worker *workerB = (Worker *)b;
    return (workerA->id - workerB->id);
}

// Function to merge image parts
// Function to merge image parts from all workers
BMPImage* merge_all(Worker* workers, int num_workers, BMPImage* (*get_image)(Worker*)) {
    // Calculate total number of parts
    int total_parts = num_workers;

    // Allocate memory for the merged image
    BMPImage *merged = malloc(sizeof(BMPImage));
    merged->width = get_image(&workers[0])->width;
    merged->height = get_image(&workers[0])->height * total_parts;
    merged->data = malloc(merged->width * merged->height * sizeof(Pixel));

    // Sort workers by id
    qsort(workers, num_workers, sizeof(Worker), compare_worker_id);

    // Merge all parts from all workers
    for (int i = 0; i < num_workers; i++) {
        BMPImage *part = get_image(&workers[i]);
        memcpy(merged->data + i * part->width * part->height, part->data,
               part->width * part->height * sizeof(Pixel));
    }

    return merged;
}


