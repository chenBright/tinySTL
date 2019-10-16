#ifndef TINYSTL_MEMORY_FUNCTION_H
#define TINYSTL_MEMORY_FUNCTION_H

#include <new>

#include "iterator_base.h"

namespace tinySTL {
    /**
     * uninitialized_copy
     */
    // 将 [first, last) 范围的元素复制到始于 d_first 的未初始化内存。
    template <class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator d_first) {
        using Value =  typename  tinySTL::iterator_traits<ForwardIterator>::value_type;
        ForwardIterator current = d_first;
        // 异常保证
        try {
            while (first != last) {
                // placement new
                ::new (static_cast<void*>(std::addressof(*current))) Value(*first);

                ++first;
                (void) ++current;
            }

            return current;
        } catch (...) {
            while (d_first != current) {
                d_first->~Value();

                ++d_first;
            }
            throw;
        }
    }
}

#endif //TINYSTL_MEMORY_FUNCTION_H
