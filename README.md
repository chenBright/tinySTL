# tinySTL
a subset of STL

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
- [x] initializer_list
- [x] 算法
    - [x] all_of
    - [x] any_of
    - [x] none_of
    - [x] for_each
    - [x] count
    - [x] count_if
    - [x] find
    - [x] find_if
    - [x] find_if_not
    - [x] max
    - [x] max_element
    - [x] min
    - [x] min_element
    - [x] equal
    - [x] lexicographical_compare
- [x] functional
    - [x] hash
    - [x] equal_to
    - [x] not_equal_to
    - [x] greater
    - [x] less
    - [x] greater_equal
    - [x] less_equal
- [x] utility
    - [x] pair
    - [x] make_pair

## TODO

- 单元测试
- std::get(std::pair)
- std::tuple_size\<std::pair\>
- std::tuple_element\<std::pair\>
- [补充 hash 函数](https://zh.cppreference.com/w/cpp/utility/hash)
- 异常保证
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
- dynamic_bitset
- string