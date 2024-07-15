#include <stdio.h>
#include <unistd.h>

int main(int numeroCualquiera)
{
    printf("Aqui inicia el proceso llamado por el proceso hijo\n");
    printf("El número es: %d\n", numeroCualquiera);
    printf("Aqui termina el proceso llamado por el proceso hijo\n");
    return 0;
}