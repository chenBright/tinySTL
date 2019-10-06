#ifndef TINYSTL_UTILITY_H
#define TINYSTL_UTILITY_H

#include "type_traits.h"

namespace tinySTL {

    // 移动
    template <class T>
    typename tinySTL::remove_reference<T>::type&& move(T&& t) noexcept {
        using return_type = typename tinySTL::remove_reference<T>::type&&;
        return static_cast<return_type>(t);
    }
}

#endif //TINYSTL_UTILITY_H
