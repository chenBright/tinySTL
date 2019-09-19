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

## TODO

- 单元测试
- priority_queue
- vector、list、deque 中存在大量的依靠创建临时对象再swap的低效代码，特别是在operator=和insert中。
- 异常保证
- [allocator static成员函数问题](https://www.zhihu.com/question/53085291/answer/133516400)
- dynamic_bitset
