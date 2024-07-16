#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Verificar que se haya pasado la cantidad correcta de argumentos
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <numero>\n", argv[0]);
        return 1;
    }

    // Convertir el argumento a un número entero
    int numero = atoi(argv[1]);

    printf("Aqui inicia el proceso llamado por el proceso padre\n");
    printf("El número es: %d\n", numero);
    printf("Aqui termina el proceso llamado por el proceso padre\n");

    return 0;
}