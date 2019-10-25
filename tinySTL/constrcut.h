#ifndef TINYSTL_CONSTRCUT_H
#define TINYSTL_CONSTRCUT_H

#include <new> // placement new
#include <utility>

#include "iterator_base.h"
#include "type_traits.h"
#include "utility_move.h"

namespace tinySTL {
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
        // TODO 学习 new 的高级用法
        // placement new
        // 在 ptr 指向的空间上调用 T 的构造函数
        // 参考 https://zh.wikipedia.org/wiki/New_(C%2B%2B)#%E5%B8%A6%E4%BD%8D%E7%BD%AE%E7%9A%84new%E8%BF%90%E7%AE%97%E7%AC%A6%E8%A1%A8%E8%BE%BE%E5%BC%8F
        ::new(ptr) T1(value);
    }

    // 可以接受多个 value 参数，支持右值（前提为 Args 类型定义了移动构造函数）。
    // 其他同上
    template <class T, class... Args>
    inline void construct(T *ptr, Args&&... args) {
        ::new(ptr) T(tinySTL::forward<Args>(args)...);
    }

    // T 的构造函数没有形参，其他同上
    template <class T>
    inline void construct(T *ptr) {
        ::new(ptr) T();
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

    // 针对迭代器的 destory 版本
    namespace detail {
        /**
         * 析构函数不重要，不做任何操作
         * @tparam ForwardIterator 迭代器
         * @param first 起始迭代器
         * @param last 最后一个要析构的迭代器的后一个迭代器
         */
        template <class ForwardIterator>
        inline void _destory(ForwardIterator first, ForwardIterator last, true_type) {

        }

        /**
         * 析构 [first, last) 范围内所指的对象
         * @tparam ForwardIterator 迭代器
         * @param first 起始迭代器
         * @param last 最后一个要析构的迭代器的后一个迭代器
         */
        template <class ForwardIterator>
        inline void _destory(ForwardIterator first, ForwardIterator last, false_type) {
            while (first != last) {
                destory(&*first);
                ++first;
            }
        }
    }

    /**
     * 析构 [first, last) 范围内所指的对象
     * 根据 has_trivial_destructor 选择 _destory 的偏特化版本
     * @tparam ForwardIterator 迭代器
     * @param first 起始迭代器
     * @param last 最后一个要析构的迭代器的后一个迭代器
     */
    template <class ForwardIterator>
    inline void destory(ForwardIterator first, ForwardIterator last) {
        using valueType = typename iterator_traits<ForwardIterator>::value_type;
        using trivialDestructor = typename __type_traits<valueType>::has_trivial_destructor;
        detail::_destory(first, last, trivialDestructor());
    }

}

#endif //TINYSTL_CONSTRCUT_H
