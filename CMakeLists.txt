cmake_minimum_required(VERSION 3.30)
project(path C)

set(CMAKE_C_STANDARD 11)

add_library(path STATIC library.c)
