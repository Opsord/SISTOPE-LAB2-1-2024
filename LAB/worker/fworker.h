//
// Created by ivang on 27/06/2024.
//

#ifndef SISTOPE_LAB1_1_2024_FWORKER_H
#define SISTOPE_LAB1_1_2024_FWORKER_H
#include "estructuras.h"
#include "filtros.h"
typedef struct {
    int id;
    BMPImage* original;
    BMPImage* saturada;
    BMPImage* grises;
    BMPImage* binarizada;
} Worker;

Worker pipeline(Worker* worker, int num_filters, int factor_saturacion, int umbral_binarizacion);



#endif //SISTOPE_LAB1_1_2024_FWORKER_H
