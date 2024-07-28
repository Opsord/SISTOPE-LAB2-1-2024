#include <stdio.h>
#include <stdlib.h>

#include "fbroker.h"

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Uso: %s <num_workers> <num_filters> <saturation_factor> <binarization_threshold>\n", argv[0]);
        return 1;
    }

    int num_workers = atoi(argv[1]);
    int num_filters = atoi(argv[2]);
    float saturation_factor = atof(argv[3]);
    float binarization_threshold = atof(argv[4]);

    printf("Número de workers: %d\n", num_workers);
    printf("Número de filtros: %d\n", num_filters);
    printf("Factor de saturación: %f\n", saturation_factor);
    printf("Umbral de binarización: %f\n", binarization_threshold);

    // Procesar argumentos y preparar el trabajo
    // ...

    return 0;
}