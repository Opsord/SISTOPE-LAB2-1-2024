#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    printf("Inicio del proceso prueba 1\n");

    // Fork a proceso prueba 2
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Inicio del proceso prueba 2\n");
        execl("./proceso-pruebas-2", "proceso-pruebas-2", "123", NULL);
        perror("Error al ejecutar proceso prueba 2");
    }
    else
    {
        printf("Fin del proceso prueba 1\n");
    }
}