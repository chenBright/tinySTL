cmake_minimum_required(VERSION 3.13)
project(tinySTL)

set(CMAKE_CXX_STANDARD 11)

add_library(tinySTL tinySTL/alloc.h tinySTL/constrcut.h)