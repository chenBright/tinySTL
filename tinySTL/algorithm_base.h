#ifndef TINYSTL_ALGORITHM_BASE_H
#define TINYSTL_ALGORITHM_BASE_H

#include <initializer_list>
#include <algorithm>
#include <cstddef>

#include "functional_base.h"
#include "iterator_base.h"
#include "utility_pair.h"
#include "utility_move.h"
#include "algorithm_heap.h"

namespace tinySTL {







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
    // 若 p(*it) == true / false。
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
    // 若 d_last 在 (first, last] 内则行为未定义。该情况下必须用 tinySTL::copy 代替 tinySTL::copy_backward。
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
    // 若 p(*it) == true / false。
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
    // 若 p(*it) == true / false。
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
     * replace_copy
     * replace_copy_if
     */
    // 复制 [first, last) 范围的元素到以 d_first 为起点的范围，并以 new_value 替换所有满足特定判别标准的元素。
    //
    // 其中 p 函数：
    // 若 p(*it) == true / false。
    template <class InputIterator, class OutputIterator, class T>
    OutputIterator replace_copy(InputIterator first, InputIterator last, OutputIterator d_first, const T& old_value, const T& new_value) {
        while (first != last) {
            *d_first++ = *first == old_value ? new_value : *first;
        }

        return d_first;
    }

    template <class InputIterator, class OutputIterator, class UnaryPredicate, class T>
    OutputIterator replace_copy(InputIterator first, InputIterator last, OutputIterator d_first, UnaryPredicate p, const T& new_value) {
        while (first != last) {
            *d_first++ = p(*first) ? new_value : *first;
        }

        return d_first;
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
     * reverse
     */
    // 翻转 [first, last) 范围的元素。
    template <class BidirIterator>
    void reverse(BidirIterator first, BidirIterator last) {
        while (first != last && first != --last) {
            iter_swap(first, last);
            ++first;
        }
    }

    /**
     * reverse_copy
     */
    // 逆序复制 [first, last) 范围的元素到以 d_first 为起点的范围。
    template <class BidirIterator, class OutputIterator>
    OutputIterator reverse_copy(BidirIterator first, BidirIterator last, OutputIterator d_first) {
        while (first != last) {
            *d_first++ = *--last;
        }

        return d_first;
    }

    /**
     * rotate
     */
    // 旋转 [first, last) 范围的元素，使得 n_first 成为首元素。
    // 返回原 first 元素旋转后的迭代器，即 first + (last - n_first)。
    template <class ForwardIterator>
    ForwardIterator rotate(ForwardIterator first, ForwardIterator n_first, ForwardIterator last) {
        if (first == n_first) {
            return last;
        }

        if (n_first == last) {
            return first;
        }

        auto n = last - n_first;

        reverse(first, n_first);
        reverse(n_first, last);
        reverse(first, last);

        return first + n;
    }

    /**
     * rotate_copy
     */
    // 从范围 [first, last) 复制元素到始于 d_first 的另一范围，
    // 使得 n_first 成为新范围的首元素，而 n_first - 1 成为末元素，即达到旋转的效果。
    template <class ForwardIterator, class OutputIterator>
    OutputIterator rotate_copy(ForwardIterator first, ForwardIterator n_first, ForwardIterator last, OutputIterator d_first) {
        d_first = copy(n_first, last, d_first);
        return copy(first, n_first, d_first);
    }

    /**
     * unique
     */
    // 删除连续的元素，只剩下一个元素。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class ForwardIterator>
    ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
        using element_type = typename iterator_traits<ForwardIterator>::value_type;
        return unique(first, last, tinySTL::equal_to<element_type>());
    }

    template <class ForwardIterator, class BinaryPredicate>
    ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate p) {
        if (first == last) {
            return last;
        }

        ForwardIterator result = first;
        while (first != last) {
            // result + 1 与 first 相邻，则不需要移动元素
            if (!p(*result, *first) && ++result != first) {
                *result = tinySTL::move(*first);
            }
        }

        return ++result;
    }

    /**
     * unique_copy
     */
    // 复制 [first, last) 范围元素到以 d_first 为起点的另一范围，使得无连续的相等元素。
    // 只复制每组等价元素的首元素。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class InputIterator, class OutputIterator>
    OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator d_first) {
        if (first == last) {
            return d_first;
        }

        *d_first = *first;
        while (++first == last) {
            if (!(*d_first == *first)) {
                *++d_first = *first;
            }
        }

        return ++d_first;
    }

    template <class InputIterator, class OutputIterator, class BinaryPredicate>
    OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator d_first, BinaryPredicate p) {
        if (first == last) {
            return d_first;
        }

        *d_first = *first;
        while (++first == last) {
            if (!p(*d_first, *first)) {
                *++d_first = *first;
            }
        }

        return ++d_first;
    }

    /**
     * is_partitioned
     */
    // 如果 [first, last) 范围中的所有满足 p 的元素（即 p(*it) == true）都出现在所有不满足的元素前则返回 true 。
    // 如果 [first, last) 为空亦返回 true 。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class InputIterator, class UnaryPredicate>
    bool is_partitioned(InputIterator first, InputIterator last, UnaryPredicate p) {
        while (first != last) {
            if (!p(*first)) {
                // 找到分界点
                break;
            }
            ++first;
        }

        // 分界点为 last，则直接返回 true。
        if (first == last) {
            return true;
        }

        // 此时 p(*first) == fase。

        while (++first != last) {
            if (p(*first)) {
                // 存在满足 p 的元素，则直接返回 false。
                return false;
            }
        }

        return true;
    }

    /**
     * partition
     */
    // 重排序范围 [first, last) 中的元素，使得所有满足 p 的元素（即 p(*it) == true）在所有不满足 p 的元素之前。
    // 不保持相对顺序。
    // 指向不满足 p 的元素的首元素的迭代器。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class ForwardIterator, class UnaryPredicate>
    ForwardIterator partition(ForwardIterator first, ForwardIterator last, UnaryPredicate p) {
        first = find_if_not(first, last, p); // 找到第一个不满足 p 的元素。
        if (first != last) {
            return first;
        }

        for (auto next = tinySTL::next(first); next != last; ++next) {
            if (p(*next)) {
                iter_swap(first, next);
            }
        }

        return first;
    }

    /**
     * partition_copy
     */
    // 从 [first, last) 范围复制元素到两个不同范围。
    // 复制满足谓词 p 的元素到始于 d_first_true 的范围。
    // 复制剩余元素到始于 d_first_false 的范围。
    // 若输入范围与任一输出范围重叠，则行为未定义。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class InputIterator, class OutputIterator1, class OutputIterator2, class UnaryPredicate>
    tinySTL::pair<OutputIterator1, OutputIterator2>
            partition_copy(InputIterator first,
                           InputIterator last,
                           OutputIterator1 d_first_true,
                           OutputIterator2 d_first_false,
                           UnaryPredicate p) {
        while (first != last) {
            if (p(*first)) {
                *d_first_true++ = *first;
            } else {
                *d_first_false++ = *first;
            }
            ++first;
        }

        return tinySTL::make_pair(d_first_true, d_first_false);
    }

    /**
     * stable_partition
     */
    // partition 的稳定版本
    template <class BidirIterator, class UnaryPredicate>
    BidirIterator stable_partition(BidirIterator first, BidirIterator last, UnaryPredicate p) {
        while (first != last && p(*first)) {
            ++first;
        }

        if (first == last) {
            return last;
        }

        auto n = tinySTL::distance(first, last);
        // 只需要为 [first, last) 准备临时存储空间即可，用于存储 p(*it) == false 的元素。
        auto* tmp_array = new typename tinySTL::iterator_traits<BidirIterator>::value_type[n];
        decltype(n) index = 0;
        auto current = first++;

        while (first != last) {
            if (p(*first)) {
                *current++ = *first++;
            } else {
                tmp_array[index++] = *first++;
            }
        }

        copy_n(tmp_array, index, current);
        delete[] tmp_array;

        return current;
    }

    /**
     * partition_point
     */
    // 返回 partition 的分界点，即第一个不满足 p 的元素。
    //
    // 其中 p 比较函数：
    // 它的声明等价于 bool pred(const Type1 &a, const Type2 &b);
    // 如果 a 等于 b，则返回 true。
    template <class ForwardIterator, class UnaryPredicate>
    ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, UnaryPredicate p) {
        while (first != last && p(*first)) {
            ++first;
        }

        return first;
    }

    /**
     * is_sorted
     */
    // 检查 [first, last) 范围的元素是否以非递减排序。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator>
    bool is_sorted(ForwardIterator first, ForwardIterator last) {
        return is_sorted_until(first, last);
    }

    template <class ForwardIterator, class Compare>
    bool is_sorted(ForwardIterator first, ForwardIterator last, Compare comp) {
        return is_sorted_until(first, last, comp);
    }

    /**
     * is_sorted_until
     */
    // 检查 [first, last) 范围的元素以非递减排序的最大范围。
    // 返回第一个不符合非递减排序的元素的迭代器。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator>
    ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last) {
        reutrn is_sorted_until(first, last, tinySTL::less<iterator_traits<ForwardIterator>::value_type>());
    }

    template <class ForwardIterator, class Compare>
    ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last, Compare comp) {
        if (first == last) {
            return last;
        }

        auto *next = first + 1;
        while (next != last) {
            if (!comp(*first, *next)) {
                return next;
            }
            ++first;
        }

        return last;
    }

    /**
     * sort
     */
    // 非稳定排序
    template <class RandomIterator>
    void sort(RandomIterator first, RandomIterator last) {
        using value_type = iterator_traits<RandomIterator>::value_type;
        sort(first, last, tinySTL::less<value_type>());
    }

    template <class RandomIterator, class Compare>
    void sort(RandomIterator first, RandomIterator last, Compare comp) {
        if (last - first <= 1) {
            return;
        }

        // 快排
        auto start = first;
        auto end = last - 1;
        auto pivot = first;
        while (start < end) {
            while (start < end && comp(*pivot, *end)) {
                --end;
            }
            *start = tinySTL::move(*end);

            while (start < end && !(*pivot, *start)) {
                ++start;
            }
            *end = tinySTL::move(*start);
        }

        *start = tinySTL::move(*pivot);
        sort(first, start, comp);
        sort(start + 1, last, comp);
    }

    /**
     * partial_sort
     */
    // 重排元素，使得 [first, middle) 含有范围 [first, last) 中已排序的 middle - first 个最小元素。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class RandomIterator>
    void partial_sort(RandomIterator first, RandomIterator middle, RandomIterator last) {
        using value_type = iterator_traits<RandomIterator>::value_type;
        partial_sort(first, middle, last, tinySTL::less<value_type>());
    }

    template <class RandomIterator, class Compare>
    void partial_sort(RandomIterator first, RandomIterator middle, RandomIterator last, Compare comp) {
        tinySTL::make_heap(first, middle); // 最大堆
        for (auto it = middle; it != last; ++it) {
            if (comp(*it, *first)) {
                auto tmp = tinySTL::move(*it);
                *it = tinySTL::move(*first);
                tinySTL::detail::pop_heap_aux(first, 0, tinySTL::distance(first, middle), tinySTL::move(tmp), comp);
            }
        }
        // 在构建好的最大堆的基础上进行堆排序。
        tinySTL::sort_heap(first, middle);
    }

    /**
     * partial_sort_copy
     */
    // 以升序排序范围 [first, last) 中的某些元素，存储结果于范围 [d_first, d_last) 。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template class InputIterator, class RandomIterator>
    RandomIterator partial_sort_copy(InputIterator first, InputIterator last,
                                     RandomIterator d_first, RandomIterator d_last) {
        using value_type = iterator_traits<RandomIterator>::value_type;
        return partial_sort_copy(first, last, d_first, d_last, tinySTL::less<value_type>());
    }

    template class InputIterator, class RandomIterator, class Compare>
    RandomIterator partial_sort_copy(InputIterator first, InputIterator last,
                                     RandomIterator d_first, RandomIterator d_last, Compare comp) {
        auto d_middle = d_first;
        while (first != last && d_middle != last) {
            *d_middle++ = *first++;
        }
        tinySTL::make_heap(first, d_middle); // 构建最大堆
        // 更新最大堆，找出最小的 (last - first) 个元素
        for (auto it = first; it != last; ++it) {
            if (comp(*it, *d_first)) {
                // 下沉操作
                tinySTL::detail::pop_heap_aux(first, 0, tinySTL::distance(first, d_middle), *it, comp);
            }
        }

        // 堆排序
        tinySTL::sort_heap(d_first, d_middle, comp);

        return d_middle;
    }

    /**
     * stable_sort
     */
    // 稳定排序
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class RandomIterator>
    void stable_sort(RandomIterator first, RandomIterator last) {
        using value_type = iterator_traits<RandomIterator>::value_type;
        stable_sort(first, last, tinySTL::less<value_type>());
    }

    template <class RandomIterator, class Compare>
    void stable_sort(RandomIterator first, RandomIterator last, Compare comp) {
        if (last - first <= 1) {
            return;
        }

        auto middle = first + (last - first) / 2;
        // 归并排序
        stable_sort(first, middle, comp);
        stable_sort(middle, last, comp);
        // 原地归并
        inplace_merge(first, middle, last, comp);
    }

    /**
     * nth_element
     */
    // 使用快排的 partition 使得 [first, nth) 范围元素满足 comp(*it, *nth) == true，(nth, last) 范围元素不满足。
    // partial_sort() 与 nth_element() 的区别：
    // partial_sort() 会将 [first, middle) 范围的元素排序，而 nth_element() 不会。
    // partial_sort() 的时间复杂度为 O(nlogn)，而 nth_element() 时间复杂度为 O(n)。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class RandomIterator>
    void nth_element(RandomIterator first, RandomIterator nth, RandomIterator last) {
        using value_type = iterator_traits<RandomIterator>::value_type;
        nth_element(first, nth, last, tinySTL::less<value_type>());
    }

    template <class RandomIterator, class Compare>
    void nth_element(RandomIterator first, RandomIterator nth, RandomIterator last, Compare comp) {
        if (last - first <= 1) {
            return;
        }

        // 快排的 partition
        auto start = first;
        auto end = last - 1;
        auto pivot = first;
        while (start < end) {
            while (start < end && comp(*pivot, *end)) {
                --end;
            }
            *start = tinySTL::move(*end);

            while (start < end && !comp(*pivot, *start)) {
                ++start;
            }
            *end = tinySTL::move(*start);
        }
        *start = tinySTL::move(*pivot);

        if (start == nth) {
            return;
        } else if (start < nth) {
            nth_element(start + 1, nth, last, comp);
        } else {
            nth_element(first, nth, start, comp);
        }
    }

    /**
     * lower_bound
     */
    // 返回指向范围 [first, last) 中第一个不小于（即大于或等于） value 的元素的迭代器，或若找不到这种元素则返回 last 。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator, class T>
    ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value) {
        auto count = tinySTL::distance(first, last);
        while (count > 0) {
            auto half = count / 2;
            auto mid = tinySTL::next(first, half);
            if (*mid < value) {
                first = tinySTL::next(mid);
                count -= half + 1;
            } else {
                count = half;
            }
        }

        return first;
    }

    template <class ForwardIterator, class T, class Compare>
    ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
        auto count = tinySTL::distance(first, last);
        while (count > 0) {
            auto half = count / 2;
            auto mid = tinySTL::next(start, half);
            if (comp(*mid, value)) {
                start = tinySTL::next(mid);
                count -= half + 1;
            } else {
                count = half;
            }
        }

        return first;
    }

    /**
     * upper_bound
     */
    // 返回指向范围 [first, last) 中第一个大于 value 的元素的迭代器，或若找不到这种元素则返回 last 。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value) {
        auto count = tinySTL::distance(first, last);
        while (count > 0) {
            auto half = count / 2;
            auto mid = tinySTL::next(first, half);
            // 与 lower_bound 相比，value == *mid 情况，也当做查找的元素在后半部分。
            // 而在 lower_bound 中，value == *mid 情况，则当做查找的元素在前半部分。
            if (!(value < *mid)) {
                first = tinySTL::next(mid);
                count -= half + 1;
            } else {
                count = half;
            }
        }

        return first;
    }

    template <class ForwardIterator, class T, class Compare>
    ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
        auto count = tinySTL::distance(first, last);
        while (count > 0) {
            auto half = count / 2;
            auto mid = tinySTL::next(start, half);
            if (!comp(value, *mid)) {
                start = tinySTL::next(mid);
                count -= half + 1;
            } else {
                count = half;
            }
        }

        return first;
    }

    /**
     * binary_search
     */
    // 二分查找
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator, class T>
    bool binary_search(ForwardIterator first, ForwardIterator last, const T& value) {
        auto result = lower_bound(first, last, value, comp);
        if (result == last) {
            return false;
        }

        // 此时，value <= *result

        return !(value < *result);
    }

    template <class ForwardIterator, class T, class Compare>
    bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
        auto result = lower_bound(first, last, value, comp);
        if (result == last) {
            return false;
        }

        return !comp(value, *result);
    }

    /**
     * equal_range
     */
    // 返回 [first, last) 范围中所有等于 value 的元素的范围，
    // 即 [lower_bound, upper_bound)。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator, class T>
    tinySTL::pair<ForwardIterator, ForwardIterator>
    equal_range(ForwardIterator first, ForwardIterator last, const T& value) {
        return {lower_bound(first, last, value, comp), upper_bound(first, last, value, comp)};
    }

    template <class ForwardIterator, class T, class Compare>
    tinySTL::pair<ForwardIterator, ForwardIterator>
    equal_range(ForwardIterator first, ForwardIterator last, const T& value, Compare comp) {
        return {lower_bound(first, last, value, comp), upper_bound(first, last, value, comp)};
    }

    /**
     * merge
     */
    // 归并二个已排序范围 [first1, last1) 和 [first2, last2) 到始于 d_first 的一个已排序范围中。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2, class OutputIterator>
    OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2, OutputIterator d_first) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                *d_first++ = *first1;
            } else {
                *d_first++ = *first2;
            }
        }

        if (first1 != last1) {
            return tinySTL::copy(first1, last1, d_first);
        } else {
            return tinySTL::copy(first2, last2, d_first);
        }
    }

    template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
    OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2, OutputIterator d_first, Compare comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) {
                *d_first++ = *first1;
            } else {
                *d_first++ = *first2;
            }
        }

        if (first1 != last1) {
            return tinySTL::copy(first1, last1, d_first);
        } else {
            return tinySTL::copy(first2, last2, d_first);
        }
    }

    /**
     * inplace_merge
     */
    // 归并二个相继的已排序范围 [first, middle) 及 [middle, last) 为一个已排序范围 [first, last) 。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class BidirIterator>
    void inplace_merge(BidirIterator first, BidirIterator middle, BidirIterator last) {
        using value_type = iterator_traits<BidirIterator>::value_type;
        inplace_merge(first, middle, last, tinySTL::less<value_type>());
    }

    template <class BidirIterator, class Compare>
    void inplace_merge(BidirIterator first, BidirIterator middle, BidirIterator last, Compare comp) {
        auto n = tinySTL::distance(first, last);
        if (n < 1) {
            return;
        }

        auto halfN = tinySTL::distance(first, middle);
        auto* tmp = new typename iterator_traits<BidirIterator>::value_type[halfN];
        copy(first, middle, tmp);
        merge(tmp, tmp + halfN, middle, last, first);

        delete[] tmp;
    }

    /**
     * includes
     */
    // 判断[first2, last2) 是否是 [first1, last1) 的子序列（子序列不必连续）。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2>
    bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                return false; // 在 [first1, last1) 范围肯定找不到等于 *first2 的元素
            } else if (*first1 < *first2) {
                ++first1;
            } else {
                ++first1;
                ++first2;
            }
        }

        return first2 != last2;
    }

    template <class InputIterator1, class InputIterator2, class Compare>
    bool includes(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                return false; // 在 [first1, last1) 范围肯定找不到等于 *first2 的元素
            } else if (comp(*first1, *first2)) {
                ++first1;
            } else {
                ++first1;
                ++first2;
            }
        }

        return first2 != last2;
    }

    /**
     * set_difference
     */
    // 差集
    // [first1, last1) 和 [first2, last2) 都是已排序的序列。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2, class OutputIterator>
    OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                                  InputIterator2 first2, InputIterator2 last2,
                                  OutputIterator d_first) {
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                ++first2;
            } else if (*first1 < *first2) {
                *d_first++ = *first1++;
            } else {
                ++first1;
                ++first2;
            }
        }

        return tinySTL::copy(first1, last1, d_first);
    }

    template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
    OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                                  InputIterator2 first2, InputIterator2 last2,
                                  OutputIterator d_first, Compare comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                ++first2;
            } else if (comp(*first1, *first2)) {
                *d_first++ = *first1++;
            } else {
                ++first1;
                ++first2;
            }
        }

        return tinySTL::copy(first1, last1, d_first);
    }

    /**
     * set_intersection
     */
    // 交集
    // [first1, last1) 和 [first2, last2) 都是已排序的序列。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2, class OutputIterator>
    OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                    InputIterator2 first2, InputIterator2 last2,
                                    OutputIterator d_first) {
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                ++first2;
            } else if (*first1 < *first2) {
                ++first1;
            } else {
                *d_first++ = *first1++;
                ++first2;
            }
        }

        return d_first;
    }

    template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
    OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                    InputIterator2 first2, InputIterator2 last2,
                                    OutputIterator d_first, Compare comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first2, *first1)) {
                ++first2;
            } else if (comp(*first1, *first2)) {
                ++first1;
            } else {
                *d_first++ = *first1++;
                ++first2;
            }
        }

        return d_first;
    }

    /**
     * set_symmetric_difference
     */
    // 将不同时存在 [first1, last1) 和 [first2, last2) 范围的元素复制到以 d_first 为起点的范围。
    // [first1, last1) 和 [first2, last2) 都是已排序的序列。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2, class OutputIterator>
    OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                            InputIterator2 first2, InputIterator2 last2,
                                            OutputIterator d_first) {
        while (first1 != last1) {
            if (first2 == last2) {
                return tinySTL::copy(first1, last2, d_first);
            }

            if (*first1 < *first2) {
                *d_first++ = *first1++;
            } else {
                if (*first2 < *first1) {
                    *d_first++ = *first2++;
                } else {
                    ++first1;
                    ++first2;
                }
            }
        }

        return tinySTL::copy(first2, last2, d_first);
    }

    template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
    OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                            InputIterator2 first2, InputIterator2 last2,
                                            OutputIterator d_first, Compare comp) {
        while (first1 != last1) {
            if (first2 == last2) {
                return tinySTL::copy(first1, last2, d_first);
            }

            if (comp(*first1, *first2)) {
                *d_first++ = *first1++;
            } else {
                if (comp(*first2, *first1)) {
                    *d_first++ = *first2++;
                } else {
                    ++first1;
                    ++first2;
                }
            }
        }

        return tinySTL::copy(first2, last2, d_first);
    }

    /**
     * set_union
     */
    // 将存在 [first1, last1) 或者 [first2, last2) 范围的元素不重复地复制到以 d_first 为起点的范围。
    // [first1, last1) 和 [first2, last2) 都是已排序的序列。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class InputIterator1, class InputIterator2, class OutputIterator>
    OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             OutputIterator d_first) {
        while (first1 != last1) {
            if (*first1 < *first2) {
                *d_first++ = *first1++;
            } else if (*first2 < *first1) {
                *d_first++ = *first2++;
            } else {
                *d_first++ = *first1++;
                ++first2;
            }
        }

        return tinySTL::copy(first2, last2, d_first);
    }

    template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
    OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             OutputIterator d_first, Compare comp) {
        while (first1 != last1) {
            if (comp(*first1, *first2)) {
                *d_first++ = *first1++;
            } else if (comp(*first2, *first1)) {
                *d_first++ = *first2++;
            } else {
                *d_first++ = *first1++;
                ++first2;
            }
        }

        return tinySTL::copy(first2, last2, d_first);
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
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
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
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate p) {
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
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
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
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp) {
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

    /**
     * is_permutation
     */
    // 判断 [first2, last2) 是否为 [first1, last1) 的一个排列。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class ForwardIterator1, class ForwardIterator2>
    bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2) {
        if (tinySTL::distance(first1, last1) != tinySTL::distance(first2, last2)) {
            return false;
        }

        for (auto it = first1; it != last1; ++it) {
            // 如果前面已经存在等于 *it 的元素，表示统计过个数了，则不需要统计了。
            if (tinySTL::find(first1, it, *it) != it) {
                continue;
            }

            // 统计元素个数
            if (tinySTL::count(it, last1, *it) != tinySTL::count(first2, last2, *it)) {
                return false;
            }
        }

        return true;
    }

    template <class ForwardIterator1, class ForwardIterator2, class Compare>
    bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2, Compare comp) {
        if (tinySTL::distance(first1, last1) != tinySTL::distance(first2, last2)) {
            return false;
        }

        for (auto it = first1; it != last1; ++it) {
            // 如果前面已经存在等于 *it 的元素，表示统计过个数了，则不需要统计了。
            if (tinySTL::find(first1, it, [i, comp](auto x) { return comp(*it, x); }) != it) {
                continue;
            }

            // 统计元素个数
            if (tinySTL::count(it,     last1, [i, comp](auto x) { return comp(*it, x); }) !=
                tinySTL::count(first2, last2, [i, comp](auto x) { return comp(*it, x); })) {
                return false;
            }
        }

        return true;
    }

    /**
     * next_permutation
     */
    // 变换 [first, last)，得到下个排列。
    // 如果存在，则返回 true；
    // 如果不存在（没有元素、只有一个元素或者元素降序排序），则返回 false。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class BidirIterator>
    bool next_permutation(BidirIterator first, BidirIterator last) {
        using element_type = typename iterator_traits<BidirIterator>::value_type;

        return next_permutation(first, last, tinySTL::less<element_type>());
    }

    template <class BidirIterator, class Compare>
    bool next_permutation(BidirIterator first, BidirIterator last, Compare comp) {
        if (first == last) { // 没有元素
            return false;
        }

        auto it = last;
        if (first == --it) { // 只有一个元素
            return false;
        }

        while (true) {
            --it;
            if (comp(*it, *tinySTL::next(it))) { // 从后往前找打第一个 *it < *(it + 1) 的元素。
                auto tmp_it = last;
                while (!comp(*it, *--tmp_it)); // 从后往前找到第一个大于 *it 的元素。
                tinySTL::iter_swap(it, tmp_it);
                tinySTL::reverse(++it, last); // 交换之后，[++it, last) 还是降序排序。直接 reverse，即可得到升序序列。

                return true;
            }

            if (it == first) { // 序列已经是降序序列。
                tinySTL::reverse(first, last);

                return false;
            }
        }
    }

    /**
     * prev_permutation
     */
    //
    // 变换 [first, last)，得到上个排列。
    // 如果存在，则返回 true；
    // 如果不存在（没有元素、只有一个元素或者元素升序排序），则返回 false。
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool comp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    template <class BidirIterator>
    bool prev_permutation(BidirIterator first, BidirIterator last) {
        using element_type = typename iterator_traits<BidirIterator>::value_type;

        return prev_permutation(first, last, tinySTL::less<element_type>());
    }

    template <class BidirIterator, class Compare>
    bool prev_permutation(BidirIterator first, BidirIterator last, Compare comp) {
        if (first == last) { // 没有元素
            return false;
        }

        auto it = last;
        if (first == --it) { // 只有一个元素
            return false;
        }

        while (true) {
            --it;
            if (comp(*tinySTL::next(it), *it)) { // 从后往前找打第一个 *it > *(it + 1) 的元素。
                auto tmp_it = last;
                while (!comp(*--tmp_it, *it)); // 从后往前找到第一个小于 *it 的元素。
                tinySTL::iter_swap(it, tmp_it);
                tinySTL::reverse(++it, last); // 交换之后，[++it, last) 还是升序排序。直接 reverse，即可得到降序序列。

                return true;
            }

            if (it == first) { // 序列已经是降序序列。
                tinySTL::reverse(first, last);

                return false;
            }
        }
    }

}

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

    template <class InputIterator1, class InputIterator2>
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
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
    bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, BinaryPredicate p) {
        while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 == last2;
    }
}

#endif //TINYSTL_ALGORITHM_BASE_H
