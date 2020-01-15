cmake_minimum_required(VERSION 3.13)
project(tinySTL)

set(CMAKE_CXX_STANDARD 11)

add_library(tinySTL library.cpp library.h tinySTL/alloc.h tinySTL/constrcut.h tinySTL/iterator_base.h tinySTL/type_traits.h tinySTL/vector.h tinySTL/iterator_reverse_iterator.h tinySTL/list.h tinySTL/deque.h tinySTL/stack.h tinySTL/queue.h tinySTL/algorithm_heap.h tinySTL/priority_queue.h tinySTL/algorithm_base.h tinySTL/functional_base.h tinySTL/utility_pair.h tinySTL/initializer_list.h tinySTL/set.h tinySTL/rb_tree.h tinySTL/map.h tinySTL/multiset.h tinySTL/multimap.h tinySTL/hashtable.h tinySTL/functional_hash.h tinySTL/unordered_set.h tinySTL/unordered_map.h tinySTL/unordered_multiset.h tinySTL/unordered_multimap.h tinySTL/utility_move.h tinySTL/memory_ptr.h tinySTL/memory_function.h tinySTL/numeric.h tinySTL/cstring.h tinySTL/iterator_insert_iterator.h tinySTL/iterator.h)

