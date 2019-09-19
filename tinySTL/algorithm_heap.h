#ifndef TINYSTL_ALGORITHM_HEAP_H
#define TINYSTL_ALGORITHM_HEAP_H

#include <functional>
#include <algorithm>

#include "iterator_base.h"

namespace tinySTL {
    template<class RandomAccessIterator, class Compare>
    void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        push_heap(first, last, std::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto value = std::move(*--last);
        auto currentIndex = distance(first, last);
        auto parentIndex = (currentIndex - 1) / 2;
        while (currentIndex > 0 && comp(first[parentIndex], value)) {
            first[currentIndex] = std::move(first[parentIndex]);
            currentIndex = parentIndex;
            parentIndex = (currentIndex - 1) / 2;
        }

        first[currentIndex] = std::move(value);
    }


    namespace detail {
        template<class RandomAccessIterator, class Distance, class T, class Compare>
        void pop_heap_aux(RandomAccessIterator first, Distance currentIndex, Distance maxIndex, T &&value, Compare comp) {
            while (true) {
                auto leftIndex = currentIndex * 2 + 1;
                auto rightIndex = leftIndex + 1;
                // 不包括 maxIndex，即 [currentIndex, maxIndex)
                if (leftIndex >= maxIndex) {
                    break;
                }
                if (rightIndex >= maxIndex) {
                    if (comp(value, first[leftIndex])) {
                        first[currentIndex] = std::move(first[leftIndex]);
                        currentIndex = leftIndex;
                    }
                    break;
                }

                if (!comp(value, first[leftIndex]) && !comp(value, first[rightIndex])) { // 两个子结点都比 value 大
                    break;
                } else if (comp(first[leftIndex], first[rightIndex])) { // 有且只有一个子结点比 value 大
                    // 右子结点大
                    first[currentIndex] = std::move(first[rightIndex]);
                    currentIndex = rightIndex;
                } else {
                    // 左子结点大
                    first[currentIndex] = std::move(first[leftIndex]);
                    currentIndex = leftIndex;
                }
            }

            first[currentIndex] = std::move(value);
        }
    }

    template<class RandomAccessIterator, class Compare>
    void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
        pop_heap(first, last, std::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto value = std::move(*(last - 1));
        *(last - 1) = std::move(first); // 将要弹出的元素放到末尾

        auto maxIndex = distance(first, last);
        decltype(maxIndex) currentIndex = 0;
        detail::pop_heap_aux(first, currentIndex, maxIndex, std::move(value), comp);
    }

    template<class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
        sort_heap(first, last, std::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        while (distance(first, last) > 1) {
            pop_heap(first, last--, comp);
        }
    }

    template<class RandomAccessIterator>
    void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
        make_heap(first, last, std::less<typename iterator_traits<RandomAccessIterator>::value_type>());
    }

    template<class RandomAccessIterator, class Compare>
    void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
        if (distance(first, last) <= 1) {
            return;
        }

        auto maxIndex = distance(first, last);
        // 对每个堆（每颗子树）进行"下沉"操作。
        // 只需要处理一般元素，即跳过大小为 1 的堆（叶子结点）。
        // 对大小为 1 的堆进行"下沉"操作，实际为空操作。
        for (auto currentIndex = (maxIndex - 2) / 2; currentIndex >= 0; -- currentIndex) {
            detail::pop_heap_aux(first, currentIndex, maxIndex, std::move(first[currentIndex]), comp);
        }
    }
}



#endif //TINYSTL_ALGORITHM_HEAP_H
