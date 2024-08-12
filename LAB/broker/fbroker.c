#include <stdlib.h>
#include <string.h>

#include "../estructuras.h"
#include "../worker/fworker.h"

// Entrada: Número de partes en las que se dividirá la imagen, imagen original
// Salida: Arreglo de imágenes divididas
// Funcionamiento: Divide la imagen en partes de igual tamaño y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns(int num, BMPImage *image)
{
    // Calcula el ancho de cada parte
    uint32_t part_width = image->width / num;
    // Crea un arreglo de imágenes para almacenar las partes
    BMPImage *parts = malloc(num * sizeof(BMPImage)); // Se puede optimizar asignando memoria para el largo x ancho de la imagen nueva, en vez de solo el largo
    // Asigna los datos de píxeles correspondientes a cada parte
    for (int i = 0; i < num; i++)
    {
        parts[i].width = part_width;
        parts[i].height = image->height;
        // Calculate the byte offset for each part considering each pixel might be more than 1 byte
        size_t pixelSize = sizeof(Pixel); // Assuming Pixel is a defined structure for pixel data
        parts[i].data = (Pixel *)((uint8_t *)image->data + i * part_width * pixelSize * image->height);
    }
    return parts;
}

// Entrada: Imagen BMP, id del worker, cantidad de filtros, factor de saturación, umbral de binarización
// Salida: Worker con la imagen modificada
// Funcionamiento: Crea un worker con el id correspondiente, asigna la imagen a modificar
// y llama a la función pipeline que aplica los filtros a la imagen
Worker call_worker(BMPImage image, int id_worker, int num_filters, int factor_saturacion, int umbral_binarizacion)
{

    // Primero se crea un worker con el id correspondiente
    Worker worker;
    worker.id = id_worker;
    // Asignamos la imagen a modificar
    worker.original = &image;

    // Luego se llama a la función pipeline que aplica los filtros a la imagen
    workflow(&worker, num_filters, factor_saturacion, umbral_binarizacion);

    // Finalmente se retorna el worker con la imagen modificada
    return worker;
}

// Entrada: Imagen BMP original, cantidad de workers
// Salida: Arreglo de workers con las imágenes modificadas
// Funcionamiento: Divide la imagen en partes y asigna cada parte a un worker
void divide_image_for_workers(BMPImage *original, int num_workers)
{
    int total_columns = original->width;
    int columns_per_worker = total_columns / num_workers;
    int extra_columns = total_columns % num_workers;

    for (int i = 0; i < num_workers; i++)
    {
        int worker_columns = columns_per_worker + (i == num_workers - 1 ? extra_columns : 0);
        BMPImage worker_image;
        worker_image.width = worker_columns;
        worker_image.height = original->height;
        worker_image.data = malloc(worker_columns * original->height * sizeof(Pixel));

        for (int y = 0; y < original->height; y++)
        {
            for (int x = 0; x < worker_columns; x++)
            {
                int originalX = x + (i * columns_per_worker);
                if (i == num_workers - 1 && x >= columns_per_worker)
                {
                    originalX += extra_columns - columns_per_worker;
                }
                int originalIndex = y * original->width + originalX;
                int workerIndex = y * worker_columns + x;
                worker_image.data[workerIndex] = original->data[originalIndex];
            }
        }

        // Aquí deberías guardar o procesar la sub-imagen worker_image como sea necesario
        // Por ejemplo, podrías escribir la sub-imagen a un archivo o enviarla a un worker para su procesamiento
        // No olvides liberar la memoria de worker_image.data cuando ya no sea necesaria
    }
}

// Entrada: worker A, worker B
// Salida: Id del worker con el id más bajo
// Funcionamiento: Compara los id de los workers y retorna el id más bajo
int compare_worker_id(const void *a, const void *b)
{
    Worker *workerA = (Worker *)a;
    Worker *workerB = (Worker *)b;
    if (workerA->id < workerB->id)
    {
        return workerA->id;
    }
    else
    {
        return workerB->id;
    }
}

// Entrada: Arreglo de workers, cantidad de workers, función para obtener la imagen de un worker
// Salida: Imagen BMP con todas las partes unidas
// Funcionamiento: Une todas las partes de las imágenes de los workers en una sola imagen
OutputImages *merge_all(Worker *workers, int num_workers) {
    OutputImages *result = malloc(sizeof(OutputImages));

    // Inicializar los anchos y altos de cada imagen fusionada
    result->saturated = malloc(sizeof(BMPImage));
    result->grayscale = malloc(sizeof(BMPImage));
    result->binarized = malloc(sizeof(BMPImage));

    result->saturated->width = 0;
    result->grayscale->width = 0;
    result->binarized->width = 0;

    result->saturated->height = workers[0].saturated->height;
    result->grayscale->height = workers[0].grayscale->height;
    result->binarized->height = workers[0].binarized->height;

    // Calcular el ancho total de cada imagen fusionada
    for (int i = 0; i < num_workers; i++) {
        result->saturated->width += workers[i].saturated->width;
        result->grayscale->width += workers[i].grayscale->width;
        result->binarized->width += workers[i].binarized->width;
    }

    // Asignar memoria para los datos de cada imagen fusionada
    result->saturated->data = malloc(result->saturated->width * result->saturated->height * sizeof(Pixel));
    result->grayscale->data = malloc(result->grayscale->width * result->grayscale->height * sizeof(Pixel));
    result->binarized->data = malloc(result->binarized->width * result->binarized->height * sizeof(Pixel));

    // Ordenar los workers por id
    qsort(workers, num_workers, sizeof(Worker), compare_worker_id);

    // Fusión de las imágenes saturadas
    uint32_t current_offset_saturated = 0;
    uint32_t current_offset_grayscale = 0;
    uint32_t current_offset_binarized = 0;

    for (int i = 0; i < num_workers; i++) {
        BMPImage *part_saturated = workers[i].saturated;
        BMPImage *part_grayscale = workers[i].grayscale;
        BMPImage *part_binarized = workers[i].binarized;

        for (uint32_t y = 0; y < part_saturated->height; y++) {
            memcpy(result->saturated->data + y * result->saturated->width + current_offset_saturated,
                   part_saturated->data + y * part_saturated->width,
                   part_saturated->width * sizeof(Pixel));

            memcpy(result->grayscale->data + y * result->grayscale->width + current_offset_grayscale,
                   part_grayscale->data + y * part_grayscale->width,
                   part_grayscale->width * sizeof(Pixel));

            memcpy(result->binarized->data + y * result->binarized->width + current_offset_binarized,
                   part_binarized->data + y * part_binarized->width,
                   part_binarized->width * sizeof(Pixel));
        }

        current_offset_saturated += part_saturated->width;
        current_offset_grayscale += part_grayscale->width;
        current_offset_binarized += part_binarized->width;
    }

    return result;
}




// Entrada: Número de partes en las que se dividirá la imagen, imagen original
// Salida: Arreglo de imágenes divididas
// Funcionamiento: Divide la imagen en partes de igual tamaño (exepto el ultimo)
// y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns_2(int num_workers, BMPImage *image)
{
    uint32_t total_width = image->width;
    uint32_t base_part_width = total_width / num_workers; // Ancho base para cada parte
    int extra_columns = total_width % num_workers;        // Columnas extras que no se distribuyen equitativamente

    BMPImage *parts = (BMPImage *)malloc(num_workers * sizeof(BMPImage));
    uint32_t current_offset = 0; // Desplazamiento actual para calcular el inicio de cada parte

    for (int i = 0; i < num_workers; i++)
    {
        parts[i].height = image->height;
        // Si es la última parte y hay columnas extras, se añaden a esta parte
        if (i == num_workers - 1 && extra_columns > 0)
        {
            parts[i].width = base_part_width + extra_columns;
        }
        else
        {
            parts[i].width = base_part_width;
        }

        size_t pixelSize = sizeof(Pixel); // Tamaño de cada píxel
        parts[i].data = (Pixel *)malloc(parts[i].width * parts[i].height * pixelSize);

        // Copiar los datos de píxeles correspondientes a cada parte
        for (uint32_t y = 0; y < parts[i].height; y++)
        {
            for (uint32_t x = 0; x < parts[i].width; x++)
            {
                uint32_t originalIndex = y * total_width + (current_offset + x);
                parts[i].data[y * parts[i].width + x] = image->data[originalIndex];
            }
        }

        current_offset += parts[i].width; // Actualizar el desplazamiento para la siguiente parte
    }

    return parts;
}
