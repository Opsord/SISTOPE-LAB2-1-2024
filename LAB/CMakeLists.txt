cmake_minimum_required(VERSION 3.28)
project(LAB2 C)

# Establecer el estándar de C
set(CMAKE_C_STANDARD 11)

# Incluir directorios
include_directories(.)
include_directories(filtros)
include_directories(lectura)
include_directories(resultados)
include_directories(broker)
include_directories(worker)

# Main
add_executable(${PROJECT_NAME}
    lab2.c
    filtros/filtros.c
    lectura/lectura.c
    lectura/lectura.h
    estructuras.h
    resultados/resultados.h
    resultados/resultados.c
)

# Broker
add_executable(broker
    broker/broker.c
    broker/fbroker.c
    broker/fbroker.h
    estructuras.h
)

# Worker
add_executable(worker
    worker/worker.c
    worker/fworker.c
    worker/fworker.h
    estructuras.h
    filtros/filtros.c
    filtros/filtros-aplicados.c
    filtros/filtros.h
)