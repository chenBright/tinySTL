cmake_minimum_required(VERSION 3.13)
project(tinySTL)

set(CMAKE_CXX_STANDARD 11)

add_library(tinySTL library.cpp library.h tinySTL/alloc.h tinySTL/constrcut.h tinySTL/iterator_base.h tinySTL/type_traits.h tinySTL/vector.h tinySTL/iterator_reverse_iterator.h tinySTL/list.h tinySTL/deque.h tinySTL/stack.h tinySTL/queue.h)
