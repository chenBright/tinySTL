#ifndef TINYSTL_CONSTRCUT_H
#define TINYSTL_CONSTRCUT_H

#include <new> // placement new

namespace tinySTL {
    // type traits
    class trueType_ {};
    class falseTypr_ {};

    // 定义了全局函数 construct() 和 destory()。
    // 它们分别负责对象的构造和析构。

    /**
     * 在 ptr 指向的内存空间上构造 T1 对象，并传入 value 作为实参
     * @tparam T1 构造的类型
     * @tparam T2 构造函数形参类型
     * @param ptr  用于构造 T1 对象的内存空间
     * @param value 构造函数的实参
     */
    template <class T1, class T2>
    inline void construct(T1 *ptr, const T2 &value) {
        new(ptr) T1(value);
    }

    // T 的构造函数没有形参，其他同上
    template <class T>
    inline void construct(T *ptr) {
        new(ptr) T();
    }

    /**
     * 析构 ptr 所指的对象
     * @tparam T 被析构的对象类型
     * @param ptr 被析构的对象的指针
     */
    template <class T>
    inline void destory(T *ptr) {
        ptr->~T(); // 调用对象的析构函数
    }

    // 针对迭代器为 char* 和 wchar_t* 的特化版本，不做任何操作
    inline void destory(char*, char*) {}
    inline void destory(wchar_t*, wchar_t*) {}

    template <class ForwardIterator>
    inline void destory(ForwardIterator first, ForwardIterator last) {
        // TODO type traits
    }

    /**
     * 析构函数不重要，不做任何操作
     * @tparam ForwardIterator 迭代器
     * @param first 起始迭代器
     * @param last 最后一个要析构的迭代器的后一个迭代器
     */
    template <class ForwardIterator>
    inline void destory_(ForwardIterator first, ForwardIterator last, trueType_) {

    }

    /**
     * 析构 [first, last) 范围内所指的对象
     * @tparam ForwardIterator 迭代器
     * @param first 起始迭代器
     * @param last 最后一个要析构的迭代器的后一个迭代器
     */
    template <class ForwardIterator>
    inline void destory_(ForwardIterator first, ForwardIterator last, falseTypr_) {
        while (first != last) {
            destory(&*first);
            ++first;
        }
    }

}

#endif //TINYSTL_CONSTRCUT_H
