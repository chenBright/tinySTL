#ifndef TINYSTL_UTILITY_MOVE_H
#define TINYSTL_UTILITY_MOVE_H

#include "type_traits.h"

namespace tinySTL {

    // 完美转发
    // 返回类型使用了引用折叠技术：
    // X& &、X& && 和 X&& & => X&
    // X&& && => X&&
    //
    // 在 C++11 中，nullptr 是右值，所以 move(nullptr) 的值是 nullptr。
    template <class T>
    T&& forward(typename tinySTL::remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <class T>
    T&& forward(typename tinySTL::remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }

    // 移动
    template <class T>
    typename tinySTL::remove_reference<T>::type&& move(T&& t) noexcept {
        using return_type = typename tinySTL::remove_reference<T>::type&&;
        return static_cast<return_type>(t);
    }
}

#endif //TINYSTL_UTILITY_MOVE_H
