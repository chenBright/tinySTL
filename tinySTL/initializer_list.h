#ifndef TINYSTL_INITIALIZER_LIST_H
#define TINYSTL_INITIALIZER_LIST_H


#include <cstddef>

namespace tinySTL {
    // 访问 const T 类型对象数组的轻量代理对象
    // 接口功能见：https://zh.cppreference.com/w/cpp/utility/initializer_list
    template <class T>
    class initializer_list {
    public:
        using value_type             = T;               // 数据类型
        using reference              = T&;              // 引用
        using const_reference        = const T&;        // 常量引用
        using size_type              = std::size_t;     // 数量类型
        using iterator               = T*;              // 迭代器
        using const_iterator         = const T*;

    private:
        // 不保证底层数组在原始 initializer_list 对象的生存期结束后继续存在。
        // std::initializer_list 的存储是未指定的（即它可以是自动、临时或静态只读内存，依赖场合）。
        iterator array_;
        size_type size_;

    public:
        initializer_list() : array_(nullptr), size_(0) {}

        size_type size() const {
            return size_;
        }

        const_iterator begin() const {
            return array_;
        }

        const_iterator end() const {
            return array_ + size_;
        }

    private:
        // 编译器可以调用private的构造函数！！！
        // 在构造函数调用之前，编译器会先在外部准备好一个 array，同时把 array 的地址传入构造函数。
        initializer_list(const_iterator array, size_type arraySize) : array_(array), size_(arraySize) {}

    }; // class initializer_list

    template <class T>
    typename initializer_list<T>::const_iterator begin(initializer_list<T> ilist) {
        return ilist.begin();
    }

    template <class T>
    typename initializer_list<T>::const_iterator end(initializer_list<T> ilist) {
        return ilist.end();
    }
}

#endif //TINYSTL_INITIALIZER_LIST_H
