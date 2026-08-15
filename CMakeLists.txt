cmake_minimum_required(VERSION 4.0)
project(diploma)

set(CMAKE_CXX_STANDARD 20)

# Для всех сборок (если нужно)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O0 -fno-omit-frame-pointer")

add_executable(graph_parser main.cpp
        graph.h
        graph.cpp
        coloring_algotithms.h
        coloring_algotithms.cpp)
