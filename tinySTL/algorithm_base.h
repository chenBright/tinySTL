#ifndef TINYSTL_ALGORITHM_BASE_H
#define TINYSTL_ALGORITHM_BASE_H

#include <initializer_list>
#include <algorithm>

#include "functional.h"
#include "iterator_base.h"

namespace tinySTL {

    /**
     * for_each
     */
    // 对 [first, last) 范围的元素调用 f，即 f(*it)
    template <class InputIterator, class UnaryFunction>
    UnaryFunction for_each(InputIterator first, InputIterator last, UnaryFunction f) {
        while (first != last) {
            f(*first++);
        }

        return f; // （C++11 起）隐式移动
    }

    /**
     * count
     * count_if
     */
    // 统计 [first, last) 范围内等于 value 的元素的个数。
    template <class InputIterator, class T>
    typename iterator_traits<InputIterator>::difference_type
    count(InputIterator first, InputIterator last, const T& value) {
        typename iterator_traits<InputIterator>::difference_type result = 0;
        while (first != last) {
            if (*first == last) {
                ++result;
            }
            ++first;
        }

        return result;
    }
    // 统计 [first, last) 范围内 op(*it) == true 的元素的个数。
    template <class InputIterator, class UnaryPredicate>
    typename iterator_traits<InputIterator>::difference_type
    count(InputIterator first, InputIterator last, UnaryPredicate op) {
        typename iterator_traits<InputIterator>::difference_type result = 0;
        while (first != last) {
            if (op(*first)) {
                ++result;
            }
            ++first;
        }

        return result;
    }

    /**
     * all_of
     * any_of
     * none_of
     */
    // 对于[first, last) 范围内所有的元素，op(*it) == true。
    template <class InputIterator, class UnaryPredicate>
    bool all_of(InputIterator first, InputIterator last, UnaryPredicate op) {
        return std::find_if_not(first, last, op) == last;
    }

    // 对于[first, last) 范围内，存在元素，使得 op(*it) == true。
    template <class InputIterator, class UnaryPredicate>
    bool any_of(InputIterator first, InputIterator last, UnaryPredicate op) {
        return std::find_if(first, last, op) != last;
    }

    // 对于[first, last) 范围内所有的元素，op(*it) != true，即 op(*it) == false。
    template <class InputIterator, class UnaryPredicate>
    bool none_of(InputIterator first, InputIterator last, UnaryPredicate op) {
        return std::find_if(first, last, op) == last;
    }

    /**
     * find
     * find_if
     * find_if_not
     */
    // 在 [first, last) 范围内查找等于 value 的元素。
    template <class InputIterator, class T>
    InputIterator find(InputIterator first, InputIterator last, const T& value) {
        while (first != last && *first != value) {
            ++first;
        }

        return first;
    }

    // 在 [first, last) 范围内查找等于 op(*it) == true 的元素。
    template <class InputIterator, class UnaryPredicate>
    InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate op) {
        while (first != last && !op(*first)) {
            ++first;
        }

        return first;
    }

    // 在 [first, last) 范围内查找等于 op(*it) == false 的元素。
    template <class InputIterator, class UnaryPredicate>
    InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate op) {
        while (first != last && op(*first)) {
            ++first;
        }

        return first;
    }

    /**
     * find_end
     */
    // 在 [first1, last1) 中搜索序列 [first2, last2)，返回最后一次出现的迭代器。
    //
    // 其中 op 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素应被当做相等则返回，则返回 true。
    template <class ForwardIterator1, class ForwardIterator2>
    ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2) {
        // 若 [first1, last1) 为空或若找不到这种序列，则返回 last1 。(C++11 起)
        if (first2 == last2) {
            return last1;
        }

        ForwardIterator1 result = last1;
        while (true) {
            auto tmpResult = std::search(first1, last1, first2, last2);
            if (tmpResult == last1) {
                break;
            } else {
                first1 = result = tmpResult;
                ++first1;
            }
        }

        return result;
    }

    template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
    ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate op) {
        // 若 [first1, last1) 为空或若找不到这种序列，则返回 last1 。(C++11 起)
        if (first2 == last2) {
            return last1;
        }

        ForwardIterator1 result = last1;
        while (true) {
            auto tmpResult = std::search(first1, last1, first2, last2, op);
            if (tmpResult == last1) {
                break;
            } else {
                first1 = result = tmpResult;
                ++first1;
            }
        }

        return result;
    }

    /**
     * move
     */
    // 将 [first, last) 范围的元素移动到 以 d_first 为起点的范围内
    template <class InputIterator, class OutputIterator>
    OutputIterator move(InputIterator first, InputIterator last, OutputIterator d_first) {
        while (first != last) {
            *d_first++ = tinySTL::move(*first++);
        }
    }

    /**
     * max
     */
    // 返回最大值
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    // （对于使用者来说，应该反过来想，返回 true，则表示 a 小于 b，再根据实际的类型来确定什么时候返回 true）
    // 虽然形参的声明不一定是 const&，但是函数不能修改传递给它的对象，
    // 以及能接受各种形式的（包括 const）Type1 和 Type2 。
    // 所以，Type1& 是不合法的。除非 Type1 的移动等价于拷贝，否则 Type1 也是不合法的。
    //
    // 如果 a == b，则返回 a。
    template <class T>
    const T& max(const T &a, const T &b) {
        return max(a, b, tinySTL::less<T>());
    }

    template <class T, class Compare>
    const T& max(const T &a, const T &b, Compare comp) {
        return comp(a, b) ? b : a;
    }

    // 时间复杂福：O(n)，n = ilist.size() - 1
    // 如果有多个相等最小值，则返回第一个最小值。

    template <class T>
    const T& max(std::initializer_list<T> ilist) {
        return max(ilist, tinySTL::less<T>());
    }

    template <class T, class Compare>
    const T& max(std::initializer_list<T> ilist, Compare comp) {
        return *(max_element(ilist.begin(), ilist.end(), comp));
    }

    /**
     * max_element
     */

    // 返回 [first, last) 范围内的最大值
    //
    // comp 比较函数的用法同上。
    //
    // 如果有多个相等的最大值，则返回第一个最大值。
    template <class ForwardIt>
    ForwardIt max_element(ForwardIt first, ForwardIt last) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        return max_element(first, last, tinySTL::less<value_type>());
    }

    template <class ForwardIt, class Compare>
    ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp) {
        if (first == last) {
            return last;
        }

        ForwardIt maxIt = first;
        while (++first != last) {
            if (comp(*maxIt, *first)) {
                maxIt = first;
            }
        }

        return maxIt;
    }

    /**
     * min
     */
    // 返回最小值
    //
    // comp 比较函数的用法同上。
    //
    // 如果 a == b，则返回 a。
    template <class T>
    const T& min(const T &a, const T &b) {
        return min(a, b, tinySTL::less<T>());
    }

    template <class T, class Compare>
    const T& min(const T &a, const T &b, Compare comp) {
        return comp(a, b) ? a : b;
    }

    // 时间复杂福：O(n)，n = ilist.size() - 1
    // 如果有多个相等最小值，则返回第一个最小值。

    template <class T>
    const T& min(std::initializer_list<T> ilist) {
        return min(ilist, tinySTL::less<T>());
    }

    template <class T, class Compare>
    const T& min(std::initializer_list<T> ilist, Compare comp) {
        return *(min_element(ilist.begin(), ilist.end(), comp));
    }

    /**
     * min_element
     */
    // 返回 [first, last) 范围内的最小值
    //
    // comp 比较函数的用法同上。
    //
    // 如果有多个相等的最小值，则返回第一个最小值。
     template <class ForwardIt>
     ForwardIt min_element(ForwardIt first, ForwardIt last) {
        using value_type = typename iterator_traits<ForwardIt>::value_type;
        return min_element(first, last, tinySTL::less<value_type>());
     }

    template <class ForwardIt, class Compare>
    ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp) {
        if (first == last) {
            return last;
        }

        ForwardIt minIt = first;
        while (++first != last) {
            if (comp(*first, *minIt)) {
                minIt = first;
            }
        }

        return minIt;
    }

    /**
     * equal
     */
    // 如果范围 [first1, last1) 和范围 [first2, first2 + (last1 - first1)) 相等，返回 true ，否则返回 false。
    template <class InputIterator1, class InputIterator2>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2) {
        while (first1 != last1 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return first1 == last1;
    }


    // 其中 op 比较函数：
    // 它的声明等价于 bool op(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    // 虽然形参的声明不一定是 const&，但是函数不能修改传递给它的对象，
    // 以及能接受各种形式的（包括 const）Type1 和 Type2 。
    // 所以，Type1& 是不合法的。除非 Type1 的移动等价于拷贝，否则 Type1 也是不合法的。
    // InputIterator1 与 InputIterator2 类型的对象在解引用后分别能隐式转换到 Type1 与 Type2
    template <class InputIterator1, class InputIterator2, class BinaryPredicate>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate op) {
        while (first1 != last1 && op(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return first1 == last1;
    }

    // 如果范围 [first1, last1) 和范围 [first2, last2) 相等，返回 true ，否则返回 false。
    template <class InputIterator1, class InputIterator2>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2, InputIterator1 last2) {
        while (first1 != last1 && first2 != last2 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 == last2;
    }

    template <class InputIterator1, class InputIterator2,  class BinaryPredicate>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2, InputIterator1 last2, BinaryPredicate op) {
        while (first1 != last1 && first2 != last2 && op(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 == last2;
    }


    /**
     * lexicographical_compare
     */
    // 检查第范围 [first1, last1) 是否按字典序小于范围 [first2, last2)。
    // 找到一个小于，就返回 true。
    template <class InputIterator1, class InputIterator2>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2, InputIterator1 last2) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                return true;
            } else if (*first2 > *first1) {
                return false;
            }

            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 == last2;
    }


    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    // 虽然形参的声明不一定是 const&，但是函数不能修改传递给它的对象，
    // 以及能接受各种形式的（包括 const）Type1 和 Type2 。
    // 所以，Type1& 是不合法的。除非 Type1 的移动等价于拷贝，否则 Type1 也是不合法的。
    // InputIterator1 与 InputIterator2 类型的对象在解引用后分别能隐式转换到 Type1 与 Type2
    template <class InputIterator1, class InputIterator2, class Compare>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2, InputIterator1 last2, Compare comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                return true;
            } else if (comp(*first2, *first1)) {
                return false;
            }

            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 == last2;
    }
}

#endif //TINYSTL_ALGORITHM_BASE_H
