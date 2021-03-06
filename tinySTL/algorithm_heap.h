#ifndef TINYSTL_ALGORITHM_HEAP_H
#define TINYSTL_ALGORITHM_HEAP_H

#include <functional>
#include <algorithm>

#include "iterator_base.h"
#include "functional_base.h"

#include "utility_move.h"

namespace tinySTL {
    template<class RandomAccessIterator, class Compare>
    void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        push_heap(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto value = tinySTL::move(*--last);
        auto currentIndex = distance(first, last);
        auto parentIndex = (currentIndex - 1) / 2;
        // 上浮操作
        while (currentIndex > 0 && comp(first[parentIndex], value)) {
            first[currentIndex] = tinySTL::move(first[parentIndex]);
            currentIndex = parentIndex;
            parentIndex = (currentIndex - 1) / 2;
        }

        first[currentIndex] = tinySTL::move(value);
    }


    namespace detail {
        // T&&：引用折叠，接受的实参可以是右值，也可以是左值，所以需要对 value 做完美转发（forward）。
        // 参考《C++ Primer》P609
        template<class RandomAccessIterator, class Distance, class T, class Compare>
        void pop_heap_aux(RandomAccessIterator first, Distance currentIndex, Distance maxIndex, T &&value, Compare comp) {
            // 下沉操作
            while (true) {
                auto leftIndex = currentIndex * 2 + 1;
                auto rightIndex = leftIndex + 1;
                // 不包括 maxIndex，即 [currentIndex, maxIndex)
                if (leftIndex >= maxIndex) {
                    break;
                }
                if (rightIndex >= maxIndex) {
                    if (comp(value, first[leftIndex])) {
                        first[currentIndex] = tinySTL::forward<T>(first[leftIndex]);
                        currentIndex = leftIndex;
                    }
                    break;
                }

                if (!comp(value, first[leftIndex]) && !comp(value, first[rightIndex])) { // 两个子结点都比 value 大
                    break;
                } else if (comp(first[leftIndex], first[rightIndex])) { // 有且只有一个子结点比 value 大
                    // 右子结点大
                    first[currentIndex] = tinySTL::forward<T>(first[rightIndex]);
                    currentIndex = rightIndex;
                } else {
                    // 左子结点大
                    first[currentIndex] = tinySTL::forward<T>(first[leftIndex]);
                    currentIndex = leftIndex;
                }
            }

            first[currentIndex] = tinySTL::forward<T>(value);
        }
    }

    template<class RandomAccessIterator, class Compare>
    void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
        pop_heap(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto value = tinySTL::move(*(last - 1));
        *(last - 1) = tinySTL::move(first); // 将要弹出的元素放到末尾

        auto maxIndex = distance(first, last);
        decltype(maxIndex) currentIndex = 0;
        detail::pop_heap_aux(first, currentIndex, maxIndex, tinySTL::move(value), comp);
    }

    template<class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
        sort_heap(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        while (distance(first, last) > 1) {
            pop_heap(first, last--, comp);
        }
    }

    template<class RandomAccessIterator>
    void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
        make_heap(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto maxIndex = distance(first, last);
        // 对每个堆（每棵子树）进行"下沉"操作。
        // 只需要处理一般元素，即跳过大小为 1 的堆（叶子结点）。
        // 对大小为 1 的堆进行"下沉"操作，实际为空操作。
        for (auto currentIndex = (maxIndex - 2) / 2; currentIndex >= 0; --currentIndex) {
            detail::pop_heap_aux(first, currentIndex, maxIndex, tinySTL::move(first[currentIndex]), comp);
        }
    }

    template<class RandomAccessIterator>
    bool is_heap(RandomAccessIterator first, RandomAccessIterator last) {
        return is_heap(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    bool is_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        auto n = distance(first, last);
        decltype(n) i = 0;
        while (true) {
            if (2 * i >= n) {
                return true;
            }
            if (comp(first[i], first[2 * i])) {
                return false;
            }

            if (2 * i + 1 >= n) {
                return true;
            }

            if (comp(first[i], first[2 * i + 1])) {
                return false;
            }

            ++i;
        }
    }

    template<class RandomAccessIterator>
    bool is_heap_until(RandomAccessIterator first, RandomAccessIterator last) {
        return is_heap_until(first, last, tinySTL::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    bool is_heap_until(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        auto n = distance(first, last);
        decltype(n) i = 0;
        while (true) {
            if (2 * i >= n) {
                return last;
            }
            if (comp(first[i], first[2 * i])) {
                return first[i];
            }

            if (2 * i + 1 >= n) {
                return last;
            }
            if (comp(first[i], first[2 * i + 1])) {
                return first[2 * i + 1];
            }

            ++i;
        }
    }
}



#endif //TINYSTL_ALGORITHM_HEAP_H
