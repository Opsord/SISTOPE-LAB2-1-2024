FLAGS = -Wall -lm

main: lab2.c lectura.o filtros.o resultados.o
	gcc $(FLAGS) -o main lab1.c lectura.o funciones.o resultados.o

lectura.o: lectura/lectura.h lectura/lectura.c estructuras.h
	gcc -c $(FLAGS) lectura.c

filtros.o: filtros filtros
	gcc -c $(FLAGS) funciones.c

resultados.o: resultados/resultados.h resultados/resultados.c
	gcc -c $(FLAGS) resultados.c

clear:
	rm -f main *.o