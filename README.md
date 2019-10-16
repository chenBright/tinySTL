# tinySTL

STL 的子集。

本项目为本人的练习项目，所以未提供测试，不能保证完全正确。

## 已完成

- [x] 空间配置器
- [x] type traits
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
- [x] hashtable
- [x] unordered_set
- [x] unordered_map
- [x] unordered_multiset
- [x] unordered_multimap
- [x] 算法
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
    - [x] swap
    - [x] swap_ranges
    - [x] iter_swap
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
    - [x] identity
    - [x] select1st
    - [x] select2nd
- [x] utility
    - [x] initializer_list
    - [x] forward
    - [x] move
    - [x] pair
    - [x] make_pair
-[x] memory（简单实现）
    - [x] shared_ptr
    - [x] weak_ptr
    - [x] unique_ptr

## TODO

- copy（uninitialized_copy） 系列
- bind、function
- [补充 hash 函数](https://zh.cppreference.com/w/cpp/utility/hash)
- 异常保证
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
- dynamic_bitset
- string