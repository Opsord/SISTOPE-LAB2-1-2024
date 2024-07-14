#include <stdlib.h>
#include <unistd.h>

int main()
{
    // Fork a proceso-pruebas con argumento int 5
    int pid = fork();
    if (pid == 0)
    {
        execl("proceso-pruebas", "proceso-pruebas", "5", NULL);
    }
    return 0;
}