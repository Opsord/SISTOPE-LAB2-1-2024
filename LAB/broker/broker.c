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
    Worker* workers = (Worker*)malloc(num_workers * sizeof(Worker));
    BMPImage* image = read_bmp("image.bmp");
    // Dividir la imagen en partes
    BMPImage* parts = split_columns_2(num_workers, image);
    for(int i = 0; i < num_workers; i++){
        printf("Worker %d\n", i);
        int pid = fork();
        if (pid == 0)
        {
            printf("Inicio del proceso hijo\n");
            // Crear un worker con la imagen correspondiente
            BMPImage* part = &parts[i];
            Worker hijo;
            worker.id = i;
            worker.original = part;
            // Aplicar los filtros a la imagen
            Worker worker = call_worker(hijo, i, num_filters, saturation_factor, binarization_threshold);
            // Agregar el worker al arreglo
            workers[i] = worker;
            // Termina el proceso hijo
        }
        else if (pid > 0)
        {
            // Espera a que todos los procesos hijos terminen
            printf("Esperando a que los workers terminen\n");
            int status;
            waitpid(pid, &status, 0);
            // Unir los resultados de los workers
            BMPImage *final_image = merge_all(workers, num_workers);
            write_bmp("final_image.bmp", final_image);
            // Liberar memoria
            free_bmp(final_image);
            free_bmp(image);
            free(parts);
            free(workers);
            printf("Fin del proceso padre\n");
        }
        else
        {
            // Error al crear el proceso hijo
            printf("Error al crear el proceso hijo\n");
            exit(1);
        }
    }


    return 0;
}