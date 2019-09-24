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
- [x] 算法
    - [x] min 
- [x] functional
    - [x] less
    - [x] less_equal
- [x] utility
    - [x] pair
    - [x] make_pair

## TODO

- 单元测试
- vector、list、deque 中存在大量的依靠创建临时对象再swap的低效代码，特别是在operator=和insert中。
- std::get(std::pair)
- std::tuple_size<std::pair>
- std::tuple_element<std::pair>
- 异常保证
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
- dynamic_bitset
