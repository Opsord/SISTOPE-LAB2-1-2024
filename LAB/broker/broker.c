#include <stdio.h>
#include <stdlib.h>

#include "fbroker.h"

int main(int argc, char *argv[]) {

    // Procesar argumentos y preparar el trabajo

    // Dividir la imagen en imágenes más pequeñas: (Enunciado)
    // Para la aplicación de los filtros por parte de los workers, el broker dividirá usando el módulo (%)
    // la imagen de tamaño NXM, tomando solo la columna, en la cantidad de workers creados, es decir,
    // M%cantidad de workers. Donde el resto de la división se le deberá enviar al último worker.

    // Crear workers con fork() y asignarles una sub-imagen

    // Esperar a que los workers completen

    // Reconstruir la imagen original

    // Mandar la imagen al proceso main

    // Liberar memoria y terminar

    return 0;
}