#ifndef SISTOPE_LAB2_1_2024_FBROKER_H
#define SISTOPE_LAB2_1_2024_FBROKER_H

#include "../estructuras.h"

// Entrada: worker A, worker B
// Salida: Id del worker con el id más bajo
// Funcionamiento: Compara los id de los workers y retorna el id más bajo
int compare_worker_id(const void *a, const void *b);

// Entrada: Arreglo de workers, cantidad de workers, función para obtener la imagen de un worker
// Salida: Imagen BMP con todas las partes unidas
// Funcionamiento: Une todas las partes de las imágenes de los workers en una sola imagen
OutputImages *merge_all(Worker *workers, int num_workers);

// Entrada: Número de partes en las que se dividirá la imagen, imagen original
// Salida: Arreglo de imágenes divididas
// Funcionamiento: Divide la imagen en partes de igual tamaño (exepto el ultimo)
// y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns_2(int num_workers, BMPImage *image);

#endif // SISTOPE_LAB2_1_2024_FBROKER_H