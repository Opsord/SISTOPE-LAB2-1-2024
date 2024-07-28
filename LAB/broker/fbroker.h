#ifndef SISTOPE_LAB2_1_2024_FBROKER_H
#define SISTOPE_LAB2_1_2024_FBROKER_H

#include "../estructuras.h"

// Entrada: Número de partes en las que se dividirá la imagen, imagen original
// Salida: Arreglo de imágenes divididas
// Funcionamiento: Divide la imagen en partes de igual tamaño y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns(int num, BMPImage *image);

// Entrada: Imagen BMP, id del worker, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: Worker con la imagen modificada
// Funcionamiento: Crea un worker con el id correspondiente, asigna la imagen a modificar
// y llama a la función pipeline que aplica los filtros a la imagen
Worker call_worker(BMPImage image, int id_worker, int num_filters, int factor_saturacion, int umbral_binarizacion);

// Entrada: Imagen BMP original, cantidad de workers
// Salida: Arreglo de workers con las imágenes modificadas
// Funcionamiento: Divide la imagen en partes y asigna cada parte a un worker
void divide_image_for_workers(BMPImage *original, int num_workers);

// Entrada: worker A, worker B
// Salida: Id del worker con el id más bajo
// Funcionamiento: Compara los id de los workers y retorna el id más bajo
int compare_worker_id(const void *a, const void *b);

// Entrada: Arreglo de workers, cantidad de workers, función para obtener la imagen de un worker
// Salida: Imagen BMP con todas las partes unidas
// Funcionamiento: Une todas las partes de las imágenes de los workers en una sola imagen
BMPImage *merge_all(Worker *workers, int num_workers, BMPImage *(*get_image)(Worker *));

// Entrada: Número de partes en las que se dividirá la imagen, imagen original
// Salida: Arreglo de imágenes divididas
// Funcionamiento: Divide la imagen en partes de igual tamaño (exepto el ultimo)
// y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns_2(int num_workers, BMPImage *image);

// No todas las funciones se usan, hay como 3 para dividir la imagen en partes
// (Borrar las que no se usen y estos comentarios)

#endif // SISTOPE_LAB2_1_2024_FBROKER_H