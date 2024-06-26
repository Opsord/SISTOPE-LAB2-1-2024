FLAGS = -Wall -lm

main: lab1.c lectura.o funciones.o resultados.o
	gcc $(FLAGS) -o main lab1.c lectura.o funciones.o resultados.o

lectura.o: lectura.h lectura.c estructuras.h
	gcc -c $(FLAGS) lectura.c

funciones.o: funciones.h funciones.c
	gcc -c $(FLAGS) funciones.c

resultados.o: resultados.h resultados.c
	gcc -c $(FLAGS) resultados.c

clear:
	rm -f main *.o