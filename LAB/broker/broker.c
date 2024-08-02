#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fbroker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main() {

    close(pipe_broker[1]); // Cerrar el extremo de escritura del pipe

    char num_workers_str[10], num_filters_str[10], saturation_factor_str[10], binarization_threshold_str[10];
    read(pipe_broker[0], num_workers_str, sizeof(num_workers_str));
    read(pipe_broker[0], num_filters_str, sizeof(num_filters_str));
    read(pipe_broker[0], saturation_factor_str, sizeof(saturation_factor_str));
    read(pipe_broker[0], binarization_threshold_str, sizeof(binarization_threshold_str));
    close(pipe_broker[0]); // Cerrar el extremo de lectura del pipe


    printf("Número de workers: %d\n", num_workers);
    printf("Número de filtros: %d\n", num_filters);
    printf("Factor de saturación: %f\n", saturation_factor);
    printf("Umbral de binarización: %f\n", binarization_threshold);



    // Procesar argumentos y preparar el trabajo
    // ...
    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    BMPImage *image = read_bmp("image.bmp");
    // Dividir la imagen en partes
    BMPImage *parts = split_columns_2(num_workers, image);

    // crear pipe para padres e hijos
    int fd[num_workers][2]; // pipe hijos
    int fd2[num_workers][2]; // pipe padre
    for (int i = 0; i < num_workers; i++) {
        // Llamado a pipes
        pipe(fd[i]);
        pipe(fd2[i]);
    }


    // Llamado a workers con la lista de partes de la imagen
    for (int i = 0; i < num_workers; i++)
    {
        printf("Worker %d\n", i);
        int pid = fork();
        if (pid == 0) {
            
        }
            // EXCL para cambio de ejecucion a los workers

            printf("Inicio del proceso hijo\n");
            // Crear un worker con la imagen correspondiente
            BMPImage *part = &parts[i];
            Worker hijo;
            hijo.id = i;
            hijo.original = part;
            // Aplicar los filtros a la imagen
            Worker worker = call_worker(*part, i, num_filters, saturation_factor, binarization_threshold);
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
            BMPImage *final_image = merge_all(workers, num_workers, ());
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