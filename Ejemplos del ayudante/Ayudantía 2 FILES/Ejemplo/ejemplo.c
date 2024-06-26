#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "funciones.h"

int main(int argc, char *argv[]){
    // Option para el getopt
    int option;
    // Variables int
    int a = 0, b = 1;
    // Variable string
    char string[100];
    // Variable float
    float f = 10.0;
    // Variables booleanas
    int c = 0, obligatorio = 0;
    while((option = getopt(argc, argv, "s:f:a:b:c")) != -1){
        switch(option){
            case 's':
                strcpy(string, optarg);
                obligatorio = 1;
                break;
            case 'f':
                f = atof(optarg);
                break;
            case 'a':
                a = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'c':
                c = 1;
                break;
        }
    }
    if(obligatorio == 0){
        printf("Se debe ingresar unos caracteres\n");
        return 1;
    }
    if(c){
        printf("La suma de a + b = %d\n", sum(a, b));
        printf("La resta de a - b = %d\n", resta(a, b));
    }
    printf("Los valores ingresados son:\n  string=%s\n  float=%f\n  a=%d b=%d\n", string, f, a, b);
    printf("Finalizando el programa\n");
    return 0;
}
