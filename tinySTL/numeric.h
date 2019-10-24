#ifndef TINYSTL_NUMERIC_H
#define TINYSTL_NUMERIC_H

#include "functional_base.h"
#include "iterator_base.h"

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

    /**
     * adjacent_difference
     */
    // 计算 [first, last) 范围中每对相邻元素的第二个和第一个的差，并写入它们到始于 d_first + 1 的范围。
    // 而 *first 写入到 *d_first 。
    //
    // 其中 op 函数：
    // 它的声明等价于 Ret comp(const Type1 &a, const Type2 &b);
    template <class InputIterator, class OutputIterator>
    OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator d_first) {
        using value_t = typename iterator_traits<InputIterator>::value_type;

        return adjacent_difference(first, last, d_first, tinySTL::minus<value_t>());
    }

    template <class InputIterator, class OutputIterator, class BinaryOperation>
    OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator d_first, BinaryOperation op) {
        if (first != last) {
            return d_first;
        }

        using value_t = typename iterator_traits<InputIterator>::value_type;
        value_t tmp = *first;
        *d_first = tmp;
        while (++first != last) {
            value_t val = *first;
            *++d_first = op(val, tmp);
            tmp = val;
        }

        return ++d_first;
    }

    /**
     * partial_sum
     */
    // 计算 [first, last) 范围的子范围中元素的部分和，并写入到始于 d_first 的范围。
    //
    // 其中 op 函数：
    // 它的声明等价于 Ret comp(const Type1 &a, const Type2 &b);
    template <class InputIterator, class OutputIterator>
    OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator d_frist) {
        using value_t = typename iterator_traits<InputIterator>::value_type;

        return partial_sum(first, last, d_frist, tinySTL::plus<value_t>());
    }

    template <class InputIterator, class OutputIterator, class BinaryOperation>
    OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator d_first, BinaryOperation op) {
        if (first == last) {
            return d_first;
        }

        typename iterator_traits<InputIterator>::value_type sum = *first;
        *d_first = sum;
        while (++first != last) {
            sum = op(sum, *first);
            *++d_first = sum;
        }

        return ++d_first;
    }
}

#endif //TINYSTL_NUMERIC_H
