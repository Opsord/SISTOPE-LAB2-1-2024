#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    printf("Inicio del proceso prueba 1\n");

    // Fork a proceso prueba 2
    pid_t pid = fork();

    // Tarea para el proceso hijo
    if (pid == 0)
    {
        printf("Inicio del proceso hijo\n");
        execl("./proceso-pruebas-2", "proceso-pruebas-2", "123", NULL);
        // Termina el proceso prueba
        exit(1);
    }
    // Tarea para el proceso padre
    else if (pid > 0)
    {
        // Espera a que el proceso hijo termine
        int status;
        waitpid(pid, &status, 0);
        printf("Fin del proceso padre\n");
    }
    // Si no hay pid entonces hubo un error
    else
    {
        // Error al crear el proceso hijo
        printf("Error al crear el proceso hijo\n");
        exit(1);
    }

    return 0;
}