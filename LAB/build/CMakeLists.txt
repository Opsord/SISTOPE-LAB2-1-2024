cmake_minimum_required(VERSION 3.28)
project(SISTOPE_LAB2_1_2024 C)

# Establecer el estándar de C
set(CMAKE_C_STANDARD 11)

# Incluir directorios
include_directories(.)
include_directories(LAB)
include_directories(LAB/filtros)
include_directories(LAB/lectura)
include_directories(LAB/resultados)
include_directories(LAB/broker)
include_directories(LAB/worker)

# Agregar ejecutables
add_executable(${PROJECT_NAME}
    LAB/lab2.c
    LAB/filtros/filtros.c
    LAB/lectura/lectura.c
    LAB/lectura/lectura.h
    LAB/filtros/filtros.h
    LAB/estructuras.h
    LAB/resultados/resultados.h
    LAB/resultados/resultados.c
    LAB/broker/broker.c
    LAB/worker/worker.c
    LAB/worker/fworker.c
    LAB/broker/fbroker.c
    LAB/broker/fbroker.h
    LAB/worker/fworker.h
    LAB/filtros/filtros-aplicados.c
)

