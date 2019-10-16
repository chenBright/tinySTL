#ifndef TINYSTL_ALGORITHM_BASE_H
#define TINYSTL_ALGORITHM_BASE_H

#include <initializer_list>
#include <algorithm>
#include <cstddef>

#include "functional_base.h"
#include "iterator_base.h"
#include "utility_pair.h"
#include "utility_move.h"

namespace tinySTL {

    /**
     * all_of
     * any_of
     * none_of
     */
    // 对于[first, last) 范围内所有的元素，p(*it) == true。
    template <class InputIterator, class UnaryPredicate>
    bool all_of(InputIterator first, InputIterator last, UnaryPredicate p) {
        return find_if_not(first, last, p) == last;
    }

    // 对于[first, last) 范围内，存在元素，使得 p(*it) == true。
    template <class InputIterator, class UnaryPredicate>
    bool any_of(InputIterator first, InputIterator last, UnaryPredicate p) {
        return find_if(first, last, p) != last;
    }

    // 对于[first, last) 范围内所有的元素，p(*it) != true，即 p(*it) == false。
    template <class InputIterator, class UnaryPredicate>
    bool none_of(InputIterator first, InputIterator last, UnaryPredicate p) {
        return find_if(first, last, p) == last;
    }

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
    // 统计 [first, last) 范围内 p(*it) == true 的元素的个数。
    template <class InputIterator, class UnaryPredicate>
    typename iterator_traits<InputIterator>::difference_type
    count(InputIterator first, InputIterator last, UnaryPredicate p) {
        typename iterator_traits<InputIterator>::difference_type result = 0;
        while (first != last) {
            if (p(*first)) {
                ++result;
            }
            ++first;
        }

        return result;
    }

    /**
     * mismatch
     */
    // 找到第一个 !(*it1 == *it2) 迭代器 pair。
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class InputIterator1, class InputIterator2>
    tinySTL::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
        while (first1 != last1 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    template <class InputIterator1, class InputIterator2, class BinaryPredicate>
    tinySTL::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate p) {
        while (first1 != last1 && p(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    template <class InputIterator1, class InputIterator2>
    tinySTL::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
        while (first1 != last1 && first2 == last2 && *first1 == *first2) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }

    template <class InputIterator1, class InputIterator2, class BinaryPredicate>
    tinySTL::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate p) {
        while (first1 != last1 && first2 == last2 && p(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return {first1, first2};
    }



    /**
     * find
     * find_if
     * find_if_not
     */
    // 在 [first, last) 范围内查找等于 value 的元素。
    //
    // 其中 p 函数：
    // 若 p(*it) == true，则返回 true。
    template <class InputIterator, class T>
    InputIterator find(InputIterator first, InputIterator last, const T& value) {
        while (first != last && *first != value) {
            ++first;
        }

        return first;
    }

    // 在 [first, last) 范围内查找等于 p(*it) == true 的元素。
    template <class InputIterator, class UnaryPredicate>
    InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate p) {
        while (first != last && !p(*first)) {
            ++first;
        }

        return first;
    }

    // 在 [first, last) 范围内查找等于 p(*it) == false 的元素。
    template <class InputIterator, class UnaryPredicate>
    InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate p) {
        while (first != last && p(*first)) {
            ++first;
        }

        return first;
    }

    /**
     * find_end
     */
    // 在 [first1, last1) 中搜索序列 [first2, last2)，返回最后一次出现的迭代器。
    // 若 [first1, last1) 为空或若找不到这种序列，则返回 last1 。(C++11 起)
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class ForwardIterator1, class ForwardIterator2>
    ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2) {
        // 若 [first1, last1) 为空或若找不到这种序列，则返回 last1 。(C++11 起)
        if (first2 == last2) {
            return last1;
        }

        ForwardIterator1 result = last1;
        while (true) {
            auto tmpResult = search(first1, last1, first2, last2);
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
                              ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate p) {
        if (first2 == last2) {
            return last1;
        }

        ForwardIterator1 result = last1;
        while (true) {
            auto tmpResult = search(first1, last1, first2, last2, p);
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
     * find_first_of
     */
    // 在 [first, last) 范围内搜索 [s_first, s_last) 范围内的任何元素。
    // 返回找到的第一个元素的迭代器。
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class InputIterator, class ForwardIterator>
    InputIterator find_first_of(InputIterator first1, InputIterator last1,
                                ForwardIterator first2, ForwardIterator last2) {
        while (first1 != last1) {
            for (auto it = first2; it != last2; ++it) {
                if (*first1 == *it) {
                    return first1;
                }
            }
            ++first1;
        }

        return last1;
    }

    template <class InputIterator, class ForwardIterator, class BinaryPredicate>
    InputIterator find_first_of(InputIterator first1, InputIterator last1,
                                ForwardIterator first2, ForwardIterator last2, BinaryPredicate p) {
        while (first1 != last1) {
            for (auto it = first2; it != last2; ++it) {
                if (p(*first1, *it)) {
                    return first1;
                }
            }
            ++first1;
        }

        return last1;
    }

    /**
     * adjacent_find
     */
    // 在 [first, last) 范围内搜索连续相等的两个元素，返回第一个元素的迭代器。
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class ForwardIterator>
    ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
        return adjacent_find(first, last, tinySTL::equal_to<typename iterator_traits<ForwardIterator>::value_type>());
    }


    template <class ForwardIterator, class BinaryPredicate>
    ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last, BinaryPredicate p) {
        if (first == last) {
            return last;
        }

        for (auto next = first++; next != first; ++first, ++next) {
            if (p(*first, *next)) {
                return first;
            }
        }

        return last;
    }

    /**
     * search
     */
    // 在 [first1, last1 - (last2 - first2))（确保最后一次搜索的长度等于 last2  - first2） 中搜索序列 [first2, last2)，返回第一次出现的迭代器。
    // 若 [last2, first2) 为空，则返回 first1 。(C++11 起)
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class ForwardIterator1, class ForwardIterator2>
    ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2) {
        while (true) {
            auto tmp = first1;
            for (auto it = first2;; ++it, ++tmp) {
                if (it == last2) {
                    return first1;
                }
                if (tmp == last1) {
                    return first1;
                }
                // 不能用 operator!= 比较元素，STL 中只假设用 opeator== 来比较元素。
                if (!(*tmp == *it)) {
                    break;
                }
            }
            ++first1;

            return first1;
        }
    }

    template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
    ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2, BinaryPredicate p) {
        while (true) {
            auto tmp = first1;
            for (auto it = first2;; ++it, ++tmp) {
                if (it == last2) {
                    return first1;
                }
                if (tmp == last1) {
                    return first1;
                }
                if (!p(*tmp, *it)) {
                    break;
                }
            }
            ++first1;

            return first1;
        }
    }

    /**
     * search_n
     */
    // 在 [first, last)中搜索 count 个连续等于 value 的元素序列，返回序列的第一个的迭代器。
    // 如果 count <= 0 或者找不到序列，则返回 last。
    //
    // 其中 p 函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 若元素相等，则返回 true。
    template <class ForwardIterator, class Size, class T>
    ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value) {
        return search_n(first, last,count, value, tinySTL::equal_to<T>());
    }

    template <class ForwardIterator, class Size, class T, class BinaryPredicate>
    ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count, const T& value, BinaryPredicate p) {
        for (; first != last; ++first) {
            if (!p(*first, value)) {
                continue;
            }

            auto candiate = first;
            Size currentCount = 0;
            while (true) {
                ++currentCount;
                if (currentCount >= count) {
                    return candiate;
                }

                ++first;
                if (first == last) {
                    return last;
                }
                if (!p(*first, value)) {
                    break;
                }
            }
        }

        return first;
    }

    /**
     * copy
     * copy_if
     */
    // 复制 [first, last) 范围的元素到以 d_first 为起点的范围，省略满足特定判别标准的元素。
    // 注意：d_first 不能在 [first, last) 内。
    template <class InputIterator, class OutputIterator>
    OutputIterator copy(InputIterator first, InputIterator last, OutputIterator d_first) {
        while (first != last) {
            *d_first++ = *first++;
        }

        return d_first;
    }


    // 复制 [first, last) 范围内满足特定判别标准的元素到以 d_first 为起点的范围。
    // 注意：d_first 不能在 [first, last) 内。
    //
    // 其中 p 函数：
    // 若 p(*it) == true，则返回 true。
    template <class InputIterator, class OutputIterator, class UnaryPredicate>
    OutputIterator copy_if(InputIterator first, InputIterator last, OutputIterator d_first, UnaryPredicate pred) {
        while (first != last) {
            if (pred(*first)) {
                *d_first++ = *first;
            }
            ++first;
        }

        return d_first;
    }

    /**
     * copy_n
     */
    // 复制 [first, first + count) 范围的元素到以 result 为起点的范围。
    template <class InputIterator, class Size, class OutputIterator>
    OutputIterator copy_n(InputIterator first, Size count, OutputIterator result) {
        while (count-- >= 0) {
            *result++ = *first++;
        }

        return result;
    }

    /**
     * copy_backward
     */
    // 将 [first, last) 范围的元素复制到以 d_first 为终点（不包括 d_last）的范围内。
    // 以逆序移动元素（首先复制末元素），但保持其相对顺序。
    // 若 d_last 在 (first, last] 内则行为未定义。该情况下必须用 std::copy 代替 std::copy_backward。
    template <class BidirIterator1, class BidirIterator2>
    BidirIterator2 copy_backward(BidirIterator1 first, BidirIterator1 last, BidirIterator2 d_last) {
        while (first != last) {
            *(--d_last) = *(--last);
        }

        return d_last;
    }

    /**
     * move
     */
    // 将 [first, last) 范围的元素移动到以 d_first 为起点的范围内。
    template <class InputIterator, class OutputIterator>
    OutputIterator move(InputIterator first, InputIterator last, OutputIterator d_first) {
        while (first != last) {
            *d_first++ = tinySTL::move(*first++);
        }
    }

    /**
     * move_backward
     */
    // 将 [first, last) 范围的元素移动到以 d_first 为终点（不包括 d_last）的范围内。
    // 以逆序移动元素（首先移动末元素），但保持其相对顺序。
    // 若 d_last 在 (first, last] 内则行为未定义。该情况下必须用 std::move 代替 std::move_backward。
    template <class BindirIterator1, class BindirIterator2>
    BindirIterator2 move_backward(BindirIterator1 first, BindirIterator1 last, BindirIterator2 d_last) {
        while (first != last) {
            *--d_last = tinySTL::move(*--last);
        }

        return d_last;
    }

    /**
     * remove
     * remove_if
     * remove_copy
     * remove_copy_if
     */
    // 从 [first, last) 范围移除所有满足特定判别标准的元素，并返回范围新结尾的尾后迭代器。
    //
    // 其中 p 函数：
    // 若 p(*it) == true，则返回 true。
    template <class ForwardIterator, class T>
    ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value) {
        auto it = first;
        while (first != last) {
            // TODO 结合 bind1st 和 equal_to，调用下面的版本，来避免代码重复
            if (!(*first == value)) {
                *it++ = *first;
            }
            ++first;
        }

        return it;
    }

    template <class ForwardIterator, class UnaryPredicate>
    ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, UnaryPredicate p) {
        auto it = first;
        while (first != last) {
            if (!p(*first)) {
                *it++ = *first;
            }
            ++first;
        }

        return it;
    }

    // 复制 [first, last) 范围的元素到以 d_first 为起点的范围，省略满足特定判别标准的元素。
    // 源与目标范围不能重叠。
    template <class InputIterator, class OutputIterator, class T>
    OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator d_first, const T& value) {
        while (first != last) {
            if (*first == value) {
                ++first;
            } else {
                *d_first++ = *first++;
            }
        }
        return d_first;
    }

    template <class InputIterator, class OutputIterator, class UnaryPredicate>
    OutputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator d_first, UnaryPredicate p) {
        while (first != last) {
            if (p(*first)) {
                ++first;
            } else {
                *d_first++ = *first++;
            }
        }
        return d_first;
    }

    /**
     * replace
     * replace_if
     */
    // 用 new_value 替换范围 [first, last) 中所有满足特定判别标准的元素。
    //
    // 其中 p 函数：
    // 若 p(*it) == true，则返回 true。
    template <class ForwardIterator, class T>
    void replace(ForwardIterator first, ForwardIterator last, const T& old_value, const T& new_value) {
        while (first != last) {
            if (*first == old_value) {
                *first = new_value;
            }
            ++first;
        }
    }

    template <class ForwardIterator, class UnaryPredicate, class T>
    void replace_if(ForwardIterator first, ForwardIterator last, UnaryPredicate p, const T& new_value) {
        while (first != last) {
            if (p(*first)) {
                *first = new_value;
            }
            ++first;
        }
    }

    /**
     * swap
     */
    template <class T>
    void swap(T& a, T& b) noexcept {
        T tmp(tinySTL::move(a));
        a = tinySTL::move(b);
        b = tinySTL::move(tmp);
    }

    template <class T, std::size_t N>
    void swap(T (&a)[N], T (&b)[N]) noexcept {
        swap_ranges(a, a + N, b);
    }

    /**
     * swap_ranges
     */
    // 范围 [first1, last1) 和以 first2 为起点的另一范围间交换元素。
    template <class ForwardIterator1, class ForwardIterator2>
    ForwardIterator2 swap_ranges(ForwardIterator1 first1, ForwardIterator2 last1, ForwardIterator2 first2) {
        while (first1 != last1) {
            iter_swap(first1++, first2++);
        }

        return first2;
    }

    /**
     * iter_swap
     */
    // 交换给定的迭代器所指向的元素的值。
    template <class ForwardIterator1, class ForwardIterator2>
    void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
        using tinySTL::swap;
        swap(*a, *b);
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


    // 其中 p 比较函数：
    // 它的声明等价于 bool p(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    // 虽然形参的声明不一定是 const&，但是函数不能修改传递给它的对象，
    // 以及能接受各种形式的（包括 const）Type1 和 Type2 。
    // 所以，Type1& 是不合法的。除非 Type1 的移动等价于拷贝，否则 Type1 也是不合法的。
    // InputIterator1 与 InputIterator2 类型的对象在解引用后分别能隐式转换到 Type1 与 Type2
    template <class InputIterator1, class InputIterator2, class BinaryPredicate>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate p) {
        while (first1 != last1 && p(*first1, *first2)) {
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
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator1 first2, InputIterator1 last2, BinaryPredicate p) {
        while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
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
