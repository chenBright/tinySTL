#ifndef TINYSTL_ALGORITHM_H
#define TINYSTL_ALGORITHM_H

#include <functional>

namespace tinySTL {
    template <class T>
    const T& min(const T &a, const T &b) {
        return min(a, b, std::less<T>());
    }

    template <class T, class Compare>
    const T& min(const T &a, const T &b, Compare comp) {
        return comp(a, b) ? a : b;
    }
}

#endif //TINYSTL_ALGORITHM_H
