#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "fbroker.h"
#include "../estructuras.h"
#include "../lectura/lectura.h"

int main(int argc, char *argv[]) {

    printf("Broker started\n");

    // ---------------------Verificación de argumentos-------------------------

    // Verificar que los argumentos sean 5
    if (argc != 6) { // 5 argumentos + puntero null
        printf("Error: argumentos inválidos\n");
        // Mostrar cantidad de argumentos recibidos
        printf("Cantidad de argumentos recibidos: %d\n", argc);
    }

    // Recordatorio de los argumentos
    // prefix_name               argv[1]
    // num_filters               argv[2]
    // saturation_factor         argv[3]
    // binarization_threshold    argv[4]
    // num_workers               argv[5]

    const char *prefix_name = argv[1];

    char *endptr;
    errno = 0;

    // Convertir num_filters
    int num_filters = strtol(argv[2], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error al convertir num_filters");
        return 1;
    }

    // Convertir saturation_factor
    float saturation_factor = strtof(argv[3], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error al convertir saturation_factor");
        return 1;
    }

    // Convertir binarization_threshold
    float binarization_threshold = strtof(argv[4], &endptr);
    if (errno != 0 || *endptr != '\0') {
        perror("Error al convertir binarization_threshold");
        return 1;
    }

    // Convertir num_workers
    int num_workers = strtol(argv[5], &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        perror("Error al convertir num_workers");
        return 1;
    }

    /*
    // Mostrar los argumentos recibidos
    printf("Prefix name: %s\n", prefix_name);
    printf("Number of filters: %d\n", num_filters);
    printf("Saturation factor: %f\n", saturation_factor);
    printf("Binarization threshold: %f\n", binarization_threshold);
    printf("Num workers: %d\n", num_workers);
    */

    // ---------------------Lectura de la imagen-------------------------

    // Llamada a la función de lectura de la imagen
    BMPImage *image = read_bmp(prefix_name);
    // Verificación de la lectura de la imagen
    if (image == NULL)
    {
        printf("Error al leer la imagen\n");
        return 1;
    }
    // Resultado de la lectura de la imagen
    printf("Imagen leída\n");
    // Imprimir la información de la imagen
    printf("Ancho: %d\n", image->width);
    printf("Alto: %d\n", image->height);

    // ---------------------Manejo de pipes y workers-------------------------

    // Creación de pipe para el broker
    int pipe_broker[2];

    // Cerrar el extremo de escritura del pipe
    close(pipe_broker[1]);

    // Alojar memoria para los workers
    Worker *workers = (Worker *)malloc(num_workers * sizeof(Worker));
    // Dividir la imagen en partes para cada worker
    BMPImage *parts = split_columns_2(num_workers, image);

    // Crear pipe para padres e hijos
    int fd[num_workers][2]; // pipe hijos
    int fd2[num_workers][2]; // pipe padre

    // Asignar pipes para cada worker
    for (int i = 0; i < num_workers; i++) {
        pipe(fd[i]);
        pipe(fd2[i]);
    }

    // ---------------------Creación de workers-------------------------

    // Definir pid
    pid_t pid;

    // Llamado a workers con la lista de partes de la imagen
    for (int i = 0; i < num_workers; i++) {
        printf("Worker %d\n", i);
        pid = fork();

        // Trabajo de los workers
        if (pid == 0) {
            close(fd[i][1]);  // Cerrar el extremo de escritura del pipe fd[i]
            close(fd2[i][0]); // Cerrar el extremo de lectura del pipe fd2[i]

            // Redirigir la entrada estándar (stdin) al pipe fd[i]
            dup2(fd[i][0], STDIN_FILENO);
            // Redirigir la salida estándar (stdout) al pipe fd2[i]
            dup2(fd2[i][1], STDOUT_FILENO);

            // Cerrar los descriptores originales después de la duplicación
            close(fd[i][0]);
            close(fd2[i][1]);

            // Ejecutar el worker
            execl("./worker", "./worker", argv[2], argv[3], argv[4], i, NULL);

            // Si `execl` falla
            perror("Error al ejecutar execl en el worker");
            exit(EXIT_FAILURE);

        } else if (pid > 0) { // Trabajo del proceso padre

            close(fd[i][0]);  // Cerrar el extremo de lectura del pipe fd[i]
            close(fd2[i][1]); // Cerrar el extremo de escritura del pipe fd2[i]

            // Enviar la parte de la imagen al proceso hijo
            write(fd[i][1], &parts[i], sizeof(BMPImage));
            close(fd[i][1]);  // Cerrar el extremo de escritura después de enviar

            // Leer la estructura Worker del proceso hijo
            Worker worker_result;
            read(fd2[i][0], &worker_result, sizeof(Worker));
            close(fd2[i][0]); // Cerrar el extremo de lectura después de leer

            // Se guarda el worker resultante en el arreglo
            workers[i] = worker_result;

            printf("Worker %d terminó su trabajo\n", i);

        } else {
            // Error al crear el proceso hijo
            printf("Error al crear el proceso hijo\n");
            exit(1);
        }
    }

    OutputImages *final_image = malloc(sizeof(OutputImages));
    if (final_image == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Unir los resultados de los workers
    final_image = merge_all(workers, num_workers);

    // Enviar la estructura Worker resultante a stdout
    ssize_t bytes_written = write(STDOUT_FILENO, &final_image, sizeof(OutputImages));
    if (bytes_written != sizeof(OutputImages)) {
        perror("Error al escribir en stdout");
        exit(EXIT_FAILURE);
    }


    // Liberar memoria

    free_bmp(image);
    free(parts);
    free(workers);

    printf("Broker process finished\n");
    return 0;

}


/*


    // Leer la imagen
    BMPImage *image = read_bmp("image.bmp");
    if (!image) {
        fprintf(stderr, "Error al leer la imagen\n");
        return 1;
    }

    // Dividir la imagen en partes
    BMPImage *parts = split_columns_2(num_workers, image);

    // Crear pipes para cada worker
    int fd[num_workers][2];
    for (int i = 0; i < num_workers; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos hijos (workers)
    for (int i = 0; i < num_workers; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Proceso hijo
            close(fd[i][1]); // Cerrar el extremo de escritura
            dup2(fd[i][0], STDIN_FILENO); // Redirigir lectura del pipe a STDIN
            close(fd[i][0]); // Cerrar el extremo de lectura

            // Ejecutar el programa worker
            execl("./worker", "worker", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            // Proceso padre
            close(fd[i][0]); // Cerrar el extremo de lectura
            // Escribir la parte de la imagen en el pipe
            write(fd[i][1], &parts[i], sizeof(BMPImage));
            close(fd[i][1]); // Cerrar el extremo de escritura
        }
    }

    // Esperar a que los workers terminen
    for (int i = 0; i < num_workers; i++) {
        wait(NULL);
    }

    // Liberar memoria
    free_bmp(image);
    free(parts);

 */


/*
 * //--
            printf("Esperando a que los workers terminen\n");
            int status;
            waitpid(pid, &status, 0);
            // Unir los resultados de los workers
            BMPImage *final_image = merge_all(workers, num_workers);
            // Debemos enviar la imagen final al proceso padre para que este la guarde

            // Liberar memoria
            free_bmp(final_image);
            free_bmp(image);
            free(parts);
            free(workers);
            printf("Fin del proceso padre\n");
 */