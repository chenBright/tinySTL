# tinySTL

STL 的子集。

本项目为本人的练习项目，所以暂时未提供测试，不能保证完全正确。请多多包涵。

## 已完成

- [x] 空间配置器
- [x] 普通迭代器
- [x] 反向迭代器
- [x] vector
- [x] list
- [x] deque
- [x] stack
- [x] queue
- [x] heap
- [x] priority_queue
- [x] rb_tree（实现来自[GitHub](https://github.com/liuyunbin/tiny-STL/blob/1dd6633ccd25f2a58465a0d528fb6c1bbfe9c412/src/stl_rb_tree.h)）
- [x] set
- [x] multiset
- [x] map
- [x] multimap
- [x] hashtable（其他 hash 函数参考[here](https://zh.cppreference.com/w/cpp/utility/hash)）
- [x] unordered_set
- [x] unordered_map
- [x] unordered_multiset
- [x] unordered_multimap
- [x] type traits
    - [x] integral_constant
    - [x] true_type
    - [x] false_type
    - [x] is_same
    - [x] remove_const
    - [x] remove_volatile
    - [x] remove_cv
    - [x] is_void
    - [x] is_null_pointer
    - [x] is_integral
    - [x] is_floating_point
    - [x] is_array
    - [x] is_pointer
    - [x] is_lvalue_reference
    - [x] is_rvalue_reference
    - [x] is_reference
    - [x] is_const
    - [x] is_volatile
    - [x] remove_reference
    - [x] add_lvalue_reference
    - [x] add_rvalue_reference
    - [x] remove_pointer
    - [x] __type_traits
- [x] algorithm
    - [x] all_of
    - [x] any_of
    - [x] none_of
    - [x] for_each
    - [x] count
    - [x] count_if
    - [x] mismatch
    - [x] find
    - [x] find_if
    - [x] find_if_not
    - [x] find_end
    - [x] find_first_of
    - [x] adjacent_find
    - [x] search
    - [x] search_n
    - [x] copy
    - [x] copy_if
    - [x] copy_n
    - [x] copy_backward
    - [x] move
    - [x] move_backward
    - [x] remove
    - [x] remove_if
    - [x] remove_copy
    - [x] remove_copy_if
    - [x] replace
    - [x] replace_if
    - [x] replace_copy
    - [x] replace_copy_if
    - [x] swap
    - [x] swap_ranges
    - [x] iter_swap
    - [x] reverse
    - [x] reverse_copy
    - [x] rotate
    - [x] rotate_copy
    - [x] unique
    - [x] unique_copy
    - [x] is_partitioned
    - [x] partition
    - [x] partition_copy
    - [x] stable_partition
    - [x] partition_point
    - [x] is_sorted
    - [x] is_sorted_until
    - [x] sort
    - [x] partial_sort
    - [x] partial_sort_copy
    - [x] stable_sort
    - [x] nth_element
    - [x] lower_bound
    - [x] merge
    - [x] inplace_merge
    - [x] max
    - [x] max_element
    - [x] min
    - [x] min_element
    - [x] equal
    - [x] lexicographical_compare
    - [x] move
- [x] functional
    - [x] hash
    - [x] plus
    - [x] minus
    - [x] multiplies
    - [x] divides
    - [x] modulus
    - [x] negate
    - [x] equal_to
    - [x] not_equal_to
    - [x] greater
    - [x] less
    - [x] greater_equal
    - [x] less_equal
    - [x] logical_and
    - [x] logical_or
    - [x] logical_not
    - [x] bit_and
    - [x] bit_or
    - [x] bit_xor
    - [x] identity
    - [x] select1st
    - [x] select2nd
- [x] utility
    - [x] initializer_list
    - [x] forward
    - [x] move
    - [x] pair
    - [x] make_pair
- [x] numeric
    - [x] iota
    - [x] accumulate
    - [x] inner_product
    - [x] adjacent_difference
    - [x] partial_sum
- [x] memory（简单实现）
    - [x] shared_ptr
    - [x] weak_ptr
    - [x] unique_ptr
    - [x] addressof
    - [x] uninitialized_copy
    - [x] uninitialized_copy_n
    - [x] uninitialized_fill
    - [x] uninitialized_fill_n
- [x] cstring    
    - [x] strcpy
    - [x] strncpy
    - [x] strcat
    - [x] strncat
    - [x] strxfrm
    - [x] strlen
    - [x] strcmp
    - [x] strncmp
    - [x] strchr
    - [x] strrchr
    - [x] strspn
    - [x] strcspn
    - [x] strpbrk
    - [x] strstr
    - [x] strtok
    - [x] memcpy

## TODO

- copy（uninitialized_copy、memcpy） 系列
- bind、function
- 异常保证
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
- dynamic_bitset
- string