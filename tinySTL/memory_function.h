#ifndef TINYSTL_MEMORY_FUNCTION_H
#define TINYSTL_MEMORY_FUNCTION_H

#include <new>

#include "iterator_base.h"

namespace tinySTL {
    /**
     * addressof
     */
    // 获得对象或函数 arg 的实际地址。
    template <class T>
    T* addressof(T& arg) noexcept {
        return reinterpret_cast<T*>( // 当做 T*
                &const_cast<char&>( // 去 const
                        reinterpret_cast<const volatile char&>(arg) // 当做 char
                        )
                );
    }

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
                ::new (static_cast<void*>(addressof(*current))) Value(*first);

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

    /**
     * uninitialized_copy_n
     */
    // 从始于 first 的范围复制 count 个元素到始于 d_first 的未初始化内存区域。
    template <class InputIterator, class Size, class ForwardIterator>
    ForwardIterator uninitialized_copy_n(InputIterator first, Size count, ForwardIterator d_first) {
        using Value = typename tinySTL::iterator_traits<ForwardIterator>::value_type;
        ForwardIterator current = d_first;
        try {
            while (count-- > 0) {
                ::new (static_cast<void*>(addressof(*current))) Value(*first);

                ++first;
                (void) ++current;
            }
        } catch (...) {
            while (d_first != current) {
                d_first->~Value();
            }
            throw;
        }

        return current;
    }

    /**
     * uninitialized_fill
     */
    // 复制给定的 value 到 [first, last) 范围的未初始化内存区域。
    template <class ForwardIterator, class T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
        using Value = typename tinySTL::iterator_traits<ForwardIterator>::value_type;
        ForwardIterator current = first;
        try {
            while (current != last) {
                ::new (static_cast<void*>(*current)) Value(value);
                ++current;
            }
        } catch (...) {
            while (first != last) {
                first->~Value();
                ++first;
            }
            throw;
        }
    }

    /**
     * uninitialized_fill_n
     */
    // 复制给定的 value 到 [first, first + count) 范围的未初始化内存区域。
    template <class ForwardIterator, class Size, class T>
    void uninitialized_fill_n(ForwardIterator first, Size count, const T& value) {
        using Value = typename tinySTL::iterator_traits<ForwardIterator>::value_type;
        ForwardIterator current = first;
        try {
            while (count-- > 0) {
                ::new (static_cast<void*>(*current)) Value(value);

                ++current;
            }
        } catch (...) {
            while (first != current) {
                first->~Value();
            }
            throw;
        }
    }
}

#endif //TINYSTL_MEMORY_FUNCTION_H
