#ifndef TINYSTL_NUMERIC_H
#define TINYSTL_NUMERIC_H

#include "functional_base.h"

namespace tinySTL {

    /**
     * iota
     */
    // 以始于 value 并重复地求值 ++value 的顺序递增值填充范围 [first, last) 。
    template <class ForwardIterator, class T>
    void iota(ForwardIterator first, ForwardIterator last, T value) {
        while (first != last) {
            *first++ = value++;
        }
    }

    /**
     * accumulate
     */
    // 计算给定值 init 与给定范围 [first, last) 中元素的和。
    //
    // 其中 op 函数：
    // 它的声明等价于 Ret comp(const Type1 &a, const Type2 &b);
    // op 必须不非法化涉及范围的任何迭代器，含尾迭代器，且不修改其所涉及范围的任何元素及 *last 。
    template <class InputIterator, class T>
    T accumulate(InputIterator first, InputIterator last, T init) {
        return accumulate(first, last, init, tinySTL::plus<T>());
    }

    template <class InputIterator, class T, class BinaryOperation>
    T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation op) {
        while (first != last) {
            init = op(init, *first++);
        }

        return init;
    }

    /**
     * inner_product
     */
    // 计算内积（即积之和）或在范围 [first1, last1) 和始于 first2 的范围上进行有序映射/规约操作。
    //
    // 其中 op 函数：
    // 它的声明等价于 Ret comp(const Type1 &a, const Type2 &b);
    // op 必须不非法化涉及范围的任何迭代器，含尾迭代器，且不修改其所涉及范围的任何元素及 *last 。
    template <class InputIterator1, class InputIterator2, class T>
    T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
        while (first1 != last1) {
            init += *first1 * *first2;
            ++first1;
            ++first2;
        }

        return init;
    }

    template <class InputIterator1, class InputIterator2, class T, class BinaryOperation1, class BinaryOperation2>
    T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOperation1 op1, BinaryOperation2 op2) {
        while (first1 != last1) {
            init = op1(init, op2(*first1, *first2));
            ++first1;
            ++first2;
        }

        return init;
    }
}

#endif //TINYSTL_NUMERIC_H
