#ifndef SISTOPE_LAB2_1_2024_FWORKER_H
#define SISTOPE_LAB2_1_2024_FWORKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../estructuras.h"
#include "../filtros/filtros.h"

Worker workflow(Worker *worker, int num_filters, float saturation_factor, float binarization_threshold);

// Entrada: Imagen BMP, id del worker, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: Worker con la imagen modificada
// Funcionamiento: Crea un worker con el id correspondiente, asigna la imagen a modificar
// y llama a la función pipeline que aplica los filtros a la imagen
Worker call_worker(BMPImage image, int id_worker, int num_filters, float saturation_factor, float binarization_threshold);

#endif // SISTOPE_LAB2_1_2024_FWORKER_H