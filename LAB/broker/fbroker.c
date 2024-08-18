#include <stdlib.h>
#include <string.h>
#include "../estructuras.h"
#include "../worker/fworker.h"

// Entrada: worker A, worker B
// Salida: Id del worker con el id más bajo
// Funcionamiento: Compara los id de los workers y retorna el id más bajo
int compare_worker_id(const void *a, const void *b) {
    Worker *workerA = (Worker *)a;
    Worker *workerB = (Worker *)b;
    return (workerA->id < workerB->id) ? workerA->id : workerB->id;
}

// Entrada: Arreglo de workers, cantidad de workers, función para obtener la imagen de un worker
// Salida: Imagen BMP con todas las partes unidas
// Funcionamiento: Une todas las partes de las imágenes de los workers en una sola imagen para cada filtro
OutputImages *merge_all(Worker *workers, int num_workers) {
    OutputImages *result = (OutputImages *)malloc(sizeof(OutputImages));
    if (!result) {
        fprintf(stderr, "Error: Could not allocate memory for OutputImages.\n");
        return NULL;
    }

    // Inicializar los anchos y altos de cada imagen fusionada
    result->saturated = (BMPImage *)malloc(sizeof(BMPImage));
    result->grayscale = (BMPImage *)malloc(sizeof(BMPImage));
    result->binarized = (BMPImage *)malloc(sizeof(BMPImage));

    if (!result->saturated || !result->grayscale || !result->binarized) {
        fprintf(stderr, "Error: Could not allocate memory for BMPImage.\n");
        free(result->saturated);
        free(result->grayscale);
        free(result->binarized);
        free(result);
        return NULL;
    }

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
    result->saturated->data = (Pixel *)malloc(result->saturated->width * result->saturated->height * sizeof(Pixel));
    result->grayscale->data = (Pixel *)malloc(result->grayscale->width * result->grayscale->height * sizeof(Pixel));
    result->binarized->data = (Pixel *)malloc(result->binarized->width * result->binarized->height * sizeof(Pixel));

    if (!result->saturated->data || !result->grayscale->data || !result->binarized->data) {
        fprintf(stderr, "Error: Could not allocate memory for image data.\n");
        free(result->saturated->data);
        free(result->grayscale->data);
        free(result->binarized->data);
        free(result->saturated);
        free(result->grayscale);
        free(result->binarized);
        free(result);
        return NULL;
    }

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
// Funcionamiento: Divide la imagen en partes de igual tamaño (excepto el último)
// y asigna los datos de píxeles correspondientes a cada parte
BMPImage *split_columns_2(int num_workers, BMPImage *image) {

    // Save the total width of the image
    uint32_t total_width = image->width;
    // Calculate the base width for each part
    uint32_t base_part_width = total_width / num_workers;
    // Calculate the extra columns for the last part
    uint32_t extra_columns = total_width % num_workers;
    // Allocate memory for the parts
    BMPImage *parts = (BMPImage *)malloc(num_workers * sizeof(BMPImage));
    if (!parts) {
        fprintf(stderr, "Error: Could not allocate memory for parts.\n");
        return NULL;
    }

    uint32_t current_offset = 0; // Desplazamiento actual para calcular el inicio de cada parte

    for (int i = 0; i < num_workers; i++) {
        parts[i].height = image->height;
        // Si es la última parte y hay columnas extras, se añaden a esta parte
        if (i == num_workers - 1 && extra_columns > 0) {
            parts[i].width = base_part_width + extra_columns;
        } else {
            parts[i].width = base_part_width;
        }
        size_t pixelSize = sizeof(Pixel); // Tamaño de cada píxel
        parts[i].data = (Pixel *)malloc(parts[i].width * parts[i].height * pixelSize);
        if (!parts[i].data) {
            fprintf(stderr, "Error: Could not allocate memory for part data.\n");
            for (int j = 0; j < i; j++) {
                free(parts[j].data);
            }
            free(parts);
            return NULL;
        }

        // Copiar los datos de píxeles correspondientes a cada parte
        for (int y = 0; y < parts[i].height; y++) {
            memcpy(parts[i].data + y * parts[i].width,
                   image->data + y * total_width + current_offset,
                   parts[i].width * pixelSize);
        }

        current_offset += parts[i].width; // Actualizar el desplazamiento para la siguiente parte
    }

    return parts;
}