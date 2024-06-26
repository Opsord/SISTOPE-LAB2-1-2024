cmake_minimum_required(VERSION 3.28)
project(SISTOPE_LAB1_1_2024 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)



add_executable(SISTOPE_LAB1_1_2024
        LAB/lab1.c
        LAB/funciones.c
        LAB/lectura.c
        LAB/lectura.h
        LAB/funciones.h
        LAB/estructuras.h
        LAB/resultados.h
        LAB/resultados.c)
