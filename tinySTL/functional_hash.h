#ifndef TINYSTL_FUNCTIONAL_HASH_H
#define TINYSTL_FUNCTIONAL_HASH_H

#include <cstddef>
#include <functional>

#include "functional_base.h"

namespace tinySTL {
    // hashtable 无法处理下面所述类型以外的类型，需要用户提供随影类型的特化版本 hash。
    template <class Key>
    struct hash : public unarg_function<Key, std::size_t> {};

    template <>
    struct hash<char*> {
        std::size_t operator()(const char* c) const {
            std::size_t h = 0;
            while (*c != '\0') {
                h = h * 5 + *c++;
            }

            return h;
        }
    };

    template <>
    struct hash<const char*> {
        std::size_t operator()(const char* c) const {
            std::size_t h = 0;
            while (*c != '\0') {
                h = h * 5 + *c++;
            }

            return h;
        }
    };

    template <>
    struct hash<char> {
        std::size_t operator()(char x) const {
            return x;
        }
    };

    template <>
    struct hash<signed char> {
        std::size_t operator()(signed x) const {
            return x;
        }
    };

    template <>
    struct hash<unsigned char> {
        std::size_t operator()(unsigned x) const {
            return x;
        }
    };

    template <>
    struct hash<short> {
        std::size_t operator()(short x) const {
            return x;
        }
    };

    template <>
    struct hash<unsigned short> {
        std::size_t operator()(unsigned short x) const {
            return x;
        }
    };

    template <>
    struct hash<int> {
        std::size_t operator()(int x) const {
            return x;
        }
    };

    template <>
    struct hash<unsigned int> {
        std::size_t operator()(unsigned int x) const {
            return x;
        }
    };

    template <>
    struct hash<long> {
        std::size_t operator()(long x) const {
            return x;
        }
    };

    template <>
    struct hash<unsigned long> {
        std::size_t operator()(unsigned long x) const {
            return x;
        }
    };

    template <>
    struct hash<long long> {
        std::size_t operator()(long long x) const {
            return x;
        }
    };

    template <>
    struct hash<unsigned long long > {
        std::size_t operator()(unsigned long long x) const {
            return x;
        }
    };

    // 对于浮点数，逐位哈希
    inline size_t bitwise_hash(const unsigned char* first, size_t count) {
    #if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
    #else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
    #endif
        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i)
        {
            result ^= (size_t)first[i];
            result *= fnv_prime;
        }
        return result;
    }

    template <>
    struct hash<float> {
        std::size_t operator()(const float& x) const {
            // TODO 可以使用 reinterpret_cast 代替 类型转换吗？
            return x == 0.0f ? 0 : bitwise_hash((const unsigned char*)&x, sizeof(float));
        }
    };

    template <>
    struct hash<double > {
        std::size_t operator()(const double& x) const {
            return x == 0.0f ? 0 : bitwise_hash((const unsigned char*)&x, sizeof(double));
        }
    };

    template <>
    struct hash<long double> {
        std::size_t operator()(const long double& x ) const {
            return x == 0.0f ? 0 : bitwise_hash((const unsigned char*)&x, sizeof(long double));
        }
    };
} // namespace tinySTL

#endif //TINYSTL_FUNCTIONAL_HASH_H
