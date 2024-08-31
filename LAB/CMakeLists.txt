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
add_executable(BROKER
        broker/broker.c
        broker/fbroker.c
        broker/fbroker.h
        estructuras.h
        lectura/lectura.c
)

# Worker
add_executable(WORKER
        worker/worker.c
        worker/fworker.c
        worker/fworker.h
        estructuras.h
        filtros/filtros.c
        filtros/filtros-aplicados.c
        filtros/filtros.h
        lectura/lectura.c
)

# Custom target to compile with debugging information
add_custom_target(debug
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
        COMMENT "Building with debugging information"
)

# Custom target to run gdb
add_custom_target(gdb
        COMMAND gdb --args ./LAB2 -N "image" -p 2 -p 1.4 -u 0.6 -v 0.6 -C "supercarpeta" -R "clasificacion" -w 4
        DEPENDS LAB2
        COMMENT "Running gdb on broker"
)