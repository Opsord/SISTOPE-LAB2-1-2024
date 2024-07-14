cmake_minimum_required(VERSION 3.28)
project(LAB_PRUEBAS C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(${PROJECT_NAME}
        proceso-pruebas.c
        proceso-pruebas-2.c
        )
