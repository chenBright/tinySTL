#ifndef TINYSTL_ALGORITHM_H
#define TINYSTL_ALGORITHM_H

#include <functional>
#include <initializer_list>
#include <algorithm>

namespace tinySTL {

    /**
     * std::min
     */

    // 返回最小值
    //
    // 其中 comp 比较函数：
    // 它的声明等价于 bool cmp(const Type1 &a, const Type2 &b);
    // 如果 a 小于 b，则返回 true。
    // 虽然形参的声明不一定是 const&，但是函数不能修改传递给它的对象，
    // 以及能接受各种形式的（包括 const）Type1 和 Type2 。
    // 所以，Type1& 是不合法的。除非 Type1 的移动等价于拷贝，否则 Type1 也是不合法的。

    // 如果 a == b，则返回 a。
    template <class T>
    const T& min(const T &a, const T &b) {
        return min(a, b, std::less<T>());
    }

    template <class T, class Compare>
    const T& min(const T &a, const T &b, Compare comp) {
        return comp(a, b) ? a : b;
    }

    // 时间复杂福：O(n)，n = ilist.size() - 1
    // 如果有多个相等最小值，则返回第一个最小值。

    template <class T>
    const T& min(std::initializer_list<T> ilist) {
        return min(ilist, std::less<T>());
    }

    template <class T, class Compare>
    const T& min(std::initializer_list<T> ilist, Compare comp) {
        return *(std::min_element(ilist.begin(), ilist.end(), comp));
    }
}

#endif //TINYSTL_ALGORITHM_H