#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "fbroker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {

    // Recordatorio de los argumentos
    // 1. prefix_name  argv[1]
    // 2. num_filters  argv[2]
    // 3. saturation_factor argv[3]
    // 4. binarization_threshold argv[4]
    // 5. num_workers  argv[5]

    const char *prefix_name = argv[1];
    int num_filters = atoi(argv[2]);
    float saturation_factor = atof(argv[3]);
    int binarization_threshold = atoi(argv[4]);
    int num_workers = atoi(argv[5]);

    // Verificar que los argumentos sean 5
    if (argc != 5) {
        printf("Error: argumentos inválidos\n");
        return 1;
    }

    close(pipe_broker[1]); // Cerrar el extremo de escritura del pipe

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
    for (int i = 0; i < num_workers; i++) {
        printf("Worker %d\n", i);
        int pid = fork();
        if (pid == 0) {
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
        } else if (pid > 0) {
            // Padre
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
        } else {
            // Error al crear el proceso hijo
            printf("Error al crear el proceso hijo\n");
            exit(1);
        }

    }
    return 0;

}