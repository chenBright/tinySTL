#ifndef TINYSTL_NUMERIC_H
#define TINYSTL_NUMERIC_H

#include "functional_base.h"

namespace tinySTL {

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
}

#endif //TINYSTL_NUMERIC_H
