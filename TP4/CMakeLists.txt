cmake_minimum_required(VERSION 3.28)
project(TP4 C)

set(CMAKE_C_STANDARD 11)

add_executable(TP4
        client.c)

target_link_libraries(TP4
        PRIVATE ${CMAKE_SOURCE_DIR}/libseg.a)
