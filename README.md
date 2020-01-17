# tinySTL

STL 的子集。

本项目为本人的练习项目。

## 已完成

- 空间配置器
- iterator
    - 普通迭代器
    - 反向迭代器
    - back_insert_iterator
    - back_inserter
    - front_insert_iterator
    - front_inserter
    - insert_iterator
    - inserter
    - move_iterator
    - make_move_iterator
    - begin
    - cbegin
    - end
    - cend
    - rbegin
    - crbegin
- vector
- list
- deque
- stack
- queue
- heap
- priority_queue
- rb_tree（实现来自[GitHub](https://github.com/liuyunbin/tiny-STL/blob/1dd6633ccd25f2a58465a0d528fb6c1bbfe9c412/src/stl_rb_tree.h)）
- set
- multiset
- map
- multimap
- hashtable（其他 hash 函数参考[here](https://zh.cppreference.com/w/cpp/utility/hash)）
- unordered_set
- unordered_map
- unordered_multiset
- unordered_multimap
- type traits
    - integral_constant
    - true_type
    - false_type
    - is_same
    - remove_const
    - remove_volatile
    - remove_cv
    - is_void
    - is_null_pointer
    - is_integral
    - is_floating_point
    - is_array
    - is_pointer
    - is_lvalue_reference
    - is_rvalue_reference
    - is_reference
    - is_const
    - is_volatile
    - remove_reference
    - add_lvalue_reference
    - add_rvalue_reference
    - remove_pointer
    - __type_traits
- algorithm
    - all_of
    - any_of
    - none_of
    - for_each
    - count
    - count_if
    - mismatch
    - find
    - find_if
    - find_if_not
    - find_end
    - find_first_of
    - adjacent_find
    - search
    - search_n
    - copy
    - copy_if
    - copy_n
    - copy_backward
    - move
    - move_backward
    - remove
    - remove_if
    - remove_copy
    - remove_copy_if
    - replace
    - replace_if
    - replace_copy
    - replace_copy_if
    - swap
    - swap_ranges
    - iter_swap
    - reverse
    - reverse_copy
    - rotate
    - rotate_copy
    - unique
    - unique_copy
    - is_partitioned
    - partition
    - partition_copy
    - stable_partition
    - partition_point
    - is_sorted
    - is_sorted_until
    - sort
    - partial_sort
    - partial_sort_copy
    - stable_sort
    - nth_element
    - lower_bound
    - upper_bound
    - binary_search
    - equal_range
    - merge
    - inplace_merge
    - includes
    - set_difference
    - set_intersection
    - set_symmetric_difference
    - set_union
    - max
    - max_element
    - min
    - min_element
    - equal
    - lexicographical_compare
    - is_permutation
    - next_permutation
    - prev_permutation
- functional
    - hash
    - plus
    - minus
    - multiplies
    - divides
    - modulus
    - negate
    - equal_to
    - not_equal_to
    - greater
    - less
    - greater_equal
    - less_equal
    - logical_and
    - logical_or
    - logical_not
    - bit_and
    - bit_or
    - bit_xor
    - identity
    - select1st
    - select2nd
- utility
    - initializer_list
    - forward
    - move
    - pair
    - make_pair
- numeric
    - iota
    - accumulate
    - inner_product
    - adjacent_difference
    - partial_sum
- memory（简单实现）
    - shared_ptr
    - weak_ptr
    - unique_ptr
    - addressof
    - uninitialized_copy
    - uninitialized_copy_n
    - uninitialized_fill
    - uninitialized_fill_n
- cstring    
    - strcpy
    - strncpy
    - strcat
    - strncat
    - strxfrm
    - strlen
    - strcmp
    - strncmp
    - strchr
    - strrchr
    - strspn
    - strcspn
    - strpbrk
    - strstr
    - strtok
    - memchr
    - memcpy
    - memset
    - memmove

## TODO

- - string
- dynamic_bitset
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
